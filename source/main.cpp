#include <3ds.h>
#include <citro2d.h>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include "libmp3player.h"
#include "Metadata_Handler.h"

enum class MenuLevel {
    Main,
    ArtistList,
    AlbumList
};

int main() {
    romfsInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    fsInit();
	initMP3Player();

	auto metadata = Mp3MetadataReader::ReadMetadata("sdmc:/music/[sic].mp3");
	printf("Title: %s\n", metadata.title.c_str());
	printf("Artist: %s\n", metadata.artist.c_str());
	printf("Album: %s\n", metadata.album.c_str());
	printf("Genre: %s\n", metadata.genre.c_str());
	printf("Year: %d\n", metadata.year);
	printf("Track: %d\n", metadata.track);
	printf("Disc: %d\n", metadata.disc);

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        gspWaitForVBlank();
    };
	exitMP3Player();
    gfxExit();
    romfsExit();
    return 0;
};
