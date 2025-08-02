//
// Created by vanho on 01/08/2025.
//

#ifndef LIBMP3PLAYER_H
#define LIBMP3PLAYER_H

#endif //LIBMP3PLAYER_H

#pragma once

void initMP3Player();
void playMP3(const char* path);
void updateMP3();
void stopMP3();
void pauseMP3();
void resumeMP3();
void exitMP3Player();

bool isMP3Playing();
bool isMP3Paused();