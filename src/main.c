/*
 * 2017 SDL2 with OpenGL Boilerplate Code
 * Use this as you wish. Licensed under CC0. No attribution required.
 */
#include "window.h"
#include "time.h"

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

    static const int FPS = 60;

    clock_t timer_start = clock();
    clock_t timer_end = clock();
    double timer_diff = ((double) (timer_end - timer_start)) / CLOCKS_PER_SEC;

    static const float FRAME_TIME = 1/FPS;

    printf("Running...\n");
    for (should_run = 1; should_run; ) {
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
            
        }

        if (timer_diff > FRAME_TIME){
            timer_start = clock();
            Update();
        }

        timer_end = clock();
        timer_diff = ((double) (timer_end - timer_start)) / CLOCKS_PER_SEC;
    }

    printf("Exiting...\n");
    Cleanup();
    return 0;
}