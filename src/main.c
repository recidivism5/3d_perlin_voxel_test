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
    SDL_SetRelativeMouseMode(SDL_TRUE);

    printf("Running...\n");
    for (should_run = 1; should_run; ) {
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                should_run = 0;
                break;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_e:
                    block_break();
                    break;

                    case SDLK_w:
                    move_direction_z = 1;
                    break;
                    
                    case SDLK_s:
                    move_direction_z = -1;
                    break;

                    case SDLK_d:
                    move_direction_x = 1;
                    break;
                    
                    case SDLK_a:
                    move_direction_x = -1;
                    break;

                    case SDLK_p:
                    toggle_full_draw = -toggle_full_draw;
                    break;

                    //change perlin period:
                    case SDLK_KP_PLUS:
                    perlin_period += 1;
                    world_init();
                    break;

                    case SDLK_KP_MINUS:
                    perlin_period -= 1;
                    world_init();
                    break;

                    //change perlin cutoff:
                    case SDLK_KP_MULTIPLY:
                    perlin_cutoff += 0.05;
                    world_init();
                    break;

                    case SDLK_KP_DIVIDE:
                    perlin_cutoff -= 0.05;
                    world_init();
                    break;

                    //per-chunk shift:
                    case SDLK_KP_1:
                    i_shift -= 0.05;
                    world_init();
                    break;

                    case SDLK_KP_2:
                    i_shift += 0.05;
                    world_init();
                    break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_w:
                    move_direction_z = 0;
                    break;

                    case SDLK_s:
                    move_direction_z = 0;
                    break;

                    case SDLK_d:
                    move_direction_x = 0;
                    break;
                    
                    case SDLK_a:
                    move_direction_x = 0;
                    break;
                }
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