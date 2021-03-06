/**
 *  theGame
 *  Author: Tomas K.
 *
 *
 *  linker options: -std=c99 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


#include "theGame.h"

#define HEIGHT 1080 //540
#define WIDTH 1920 //960
#define SPEED 3
#define BULLETSPEED 700
#define MAXBULLETS 1000
#define GRAVITY 0.14f
#define MAXLEDGES 100

#define MAXSPECIALS 40


//************************************MAIN******************
int main()
{
    // initializing
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // create a SDL window
    SDL_Window *window = SDL_CreateWindow(
                                          "The game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH,
                                          HEIGHT,
                                          SDL_WINDOW_OPENGL
                                         );
    // set window fullscreen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;

    // init action
    Action *p_action = malloc(sizeof(Action));
    p_action->down = p_action->up = p_action->left = p_action->right = p_action->p1Shooting = false;
    p_action->down2 = p_action->up2 = p_action->left2 = p_action->right2 = p_action->p2Shooting = p_action->mouseOn = false;



    bool running = true;

    // init audio
    Mix_Chunk *ak47 = Mix_LoadWAV("ak47.wav");
    Mix_Chunk *deathSound = Mix_LoadWAV("death.wav");
    Mix_Music *backgroundm = Mix_LoadMUS("background.mp3");

    Mix_PlayMusic(backgroundm, -1);

    // init specials
    Special specials[MAXSPECIALS];

    // init bullets
    Bullet bullets[MAXBULLETS];
    for(int i = 0; i < MAXBULLETS; i++)
        bullets[i].display = false;

    // setting up the players
    Man player1 = {.x = 54, .y = HEIGHT - 105, .facingLeft = false, .alive = 2, .ammo = 100,
                   .currentSprite = 4 , .hp = 50, .dy = 0.00f, .jump = false, .onLedge = true};
    Man player2 = {.x = WIDTH - 104, .y = HEIGHT - 105, .facingLeft = true, .ammo = 100,
                   .alive = 2, .currentSprite = 4 , .hp = 50, .dy = 0.00f, .jump = false};

    // starttime
    time_t startTime = time(NULL);

    // gameState init
    gameState game = {.p_p1 = &player1, .p_p2 = &player2, .action = p_action, .bullets = bullets, .frames = 0,
                      .gameIsOver = false, .renderer = renderer, .ak47 = ak47,
                      .startTime = startTime, .p_texts = malloc(sizeof(Texts)), .lastHit = 0, .deathSound = deathSound

                     };

    game.specials = specials;

    // init specials
    initSpecials(specials);

    // init ledges
    Ledge ledges[MAXLEDGES];
    game.ledges = ledges;
    setLedges(ledges);

    // init of texts
    initTexts(game);

    // loading images
    loadImages(&game);

    long long start, end;
    // Animation loop
    while(running)
    {
        start = current_timestamp();
        // events processing
        while(SDL_PollEvent(&event))
        {
            eventsDetection(&event, p_action, &running, &game, window);
        }

        // logic stuff
        logicStuff(&game);

        // rendering
        renderStuff(renderer, &game);

        // Show what was drawn
        SDL_RenderPresent(renderer);

        game.frames = game.frames + 1;

        end = current_timestamp();
        // not so fast
        SDL_Delay((500/60 - (end - start)) > 0 ? (500/60 - (end - start)) : 0 );

    } // end of animation loop

    // releasing resources
    free(p_action);
    SDL_DestroyTexture(game.p_texts->gameOver);
    SDL_DestroyTexture(game.p_texts->p1Won);
    SDL_DestroyTexture(game.p_texts->p2Won);
    SDL_DestroyTexture(game.p_texts->time);
    free(game.p_texts);
    SDL_DestroyTexture(game.bulletTexture);
    SDL_DestroyTexture(player1.sheetTexture);
    SDL_DestroyTexture(player2.sheetTexture);
    SDL_DestroyTexture(game.backTexture);
    SDL_DestroyTexture(game.ledgeTexture);
    SDL_DestroyTexture(game.ledgeTextureYX);
    SDL_DestroyTexture(game.helpTexture);
    SDL_DestroyTexture(game.heartTexture);
    SDL_DestroyTexture(game.ammoTexture);
    Mix_FreeChunk(ak47);
    Mix_FreeChunk(deathSound);
    Mix_FreeMusic(backgroundm);
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
