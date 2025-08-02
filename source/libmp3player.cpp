//
// Created by vanho on 01/08/2025.
//

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include <3ds.h>
#include <3ds/os.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "libmp3player.h"

#define AUDIO_BUFFER_SIZE (4096 * 4)

static drmp3 mp3;
static bool isPlaying = false;
static bool isPaused = false;

static int16_t* audioBuffers[2];
static ndspWaveBuf waveBufs[2];
static int currentBuf = 0;

void initMP3Player() {
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, 44100.0f);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    for (int i = 0; i < 2; ++i) {
        audioBuffers[i] = (int16_t*)linearAlloc(AUDIO_BUFFER_SIZE);
        memset(&waveBufs[i], 0, sizeof(ndspWaveBuf));
        waveBufs[i].data_vaddr = audioBuffers[i];
    }

    isPlaying = false;
    isPaused = false;
}

void playMP3(const char* path) {
    if (isPlaying) stopMP3();

    printf("Opening MP3: %s\n", path);

    if (!drmp3_init_file(&mp3, path, NULL)) {
        printf("Error: failed to load MP3.\n");
        return;
    }

    printf("MP3 loaded: %d Hz, %d channels\n", mp3.sampleRate, mp3.channels);

    isPlaying = true;
    isPaused = false;
    currentBuf = 0;

    // Prime both buffers
    for (int i = 0; i < 2; ++i) {
        size_t frames = drmp3_read_pcm_frames_s16(&mp3, AUDIO_BUFFER_SIZE / (2 * sizeof(int16_t)), audioBuffers[i]);
        if (frames == 0) break;

        waveBufs[i].data_vaddr = audioBuffers[i];
        waveBufs[i].nsamples = frames;
        waveBufs[i].looping = false;

        DSP_FlushDataCache(audioBuffers[i], frames * 2 * sizeof(int16_t));
        ndspChnWaveBufAdd(0, &waveBufs[i]);
    }
}

void updateMP3() {
    if (!isPlaying || isPaused) return;

    ndspWaveBuf* wb = &waveBufs[currentBuf];

    if (wb->status == NDSP_WBUF_DONE || wb->status == 0) {
        size_t frames = drmp3_read_pcm_frames_s16(&mp3, AUDIO_BUFFER_SIZE / (2 * sizeof(int16_t)), audioBuffers[currentBuf]);
        if (frames == 0) {
            stopMP3();
            return;
        }

        wb->data_vaddr = audioBuffers[currentBuf];
        wb->nsamples = frames;
        wb->looping = false;

        DSP_FlushDataCache(audioBuffers[currentBuf], frames * 2 * sizeof(int16_t));
        ndspChnWaveBufAdd(0, wb);

        currentBuf ^= 1;
    }
}

void pauseMP3() {
    if (isPlaying && !isPaused) {
        isPaused = true;
        ndspChnWaveBufClear(0);
    }
}

void resumeMP3() {
    if (isPlaying && isPaused) {
        isPaused = false;
    }
}

void stopMP3() {
    if (!isPlaying) return;

    ndspChnWaveBufClear(0);
    drmp3_uninit(&mp3);
    isPlaying = false;
    isPaused = false;
}

void exitMP3Player() {
    stopMP3();
    for (int i = 0; i < 2; ++i) {
        if (audioBuffers[i]) {
            linearFree(audioBuffers[i]);
            audioBuffers[i] = nullptr;
        }
    }
}

bool isMP3Playing() {
    return isPlaying && !isPaused;
}

bool isMP3Paused() {
    return isPaused;
}
