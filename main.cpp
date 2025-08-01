#include <iostream>

class Song {
public:
    string path;
    string album_name;
    string artist_name;
    string genre;
    int release_year;
    int disk_number;
    int track_number;
    void Play(){

        };
};
int main() {
    // Initializations
    srvInit();        // services
    aptInit();        // applets
    hidInit(NULL);    // input
    gfxInitDefault(); // graphics
    gfxSet3D(false);  // stereoscopy (true: enabled / false: disabled)
    u32 kDown;        // keys down
    u32 kHeld;        // keys pressed
    u32 kUp;          // keys up
    while (aptMainLoop())
    {

        // Wait for next frame
        gspWaitForVBlank();

        // Read which buttons are currently pressed or not
        hidScanInput();
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        kUp = hidKeysUp();

        // If START button is pressed, break loop and quit
        if (kDown & KEY_START){
            break;
        }


        /** Your code goes here **/


        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    // Exit
    gfxExit();
    hidExit();
    aptExit();
    srvExit();

    // Return to hbmenu
    return 0;
    }
    return 0;
};