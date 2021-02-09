/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include "window.h"

/*
 * Program entry point
 */
int main(int argc, char *argv[])
{
    int should_run;

    printf("Initializing...\n");
    if (Initialize()) {
        return 1;
    }

    printf("Running...\n");
    for (should_run = 1; should_run; ) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
        }

        Update();
    }

    printf("Exiting...\n");
    Cleanup();
    return 0;
}