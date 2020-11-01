#include <SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>

#include "game.h"
#include "raycaster.h"
#include "raycaster_fixed.h"
#include "raycaster_float.h"
#include "renderer.h"

using namespace std;

class fps_renderer
{
public:
    fps_renderer(SDL_Renderer *renderer) : renderer(renderer)
    {
        font = TTF_OpenFont("FreeMono.ttf", 24);
        loc.x = 0;
        loc.y = 0;
    }

    ~fps_renderer() { SDL_DestroyTexture(texture); }

    void update(int fps)
    {
        if (fps != _fps) {
            _fps = fps;
            update_texture();
        }
    }

    void render() { SDL_RenderCopy(renderer, texture, NULL, &loc); }

private:
    void update_texture()
    {
        const auto text = std::to_string(_fps);
        auto surf = TTF_RenderText_Solid(font, text.c_str(), {0, 0, 255});
        SDL_DestroyTexture(texture);
        texture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_QueryTexture(texture, 0, 0, &loc.w, &loc.h);
        SDL_FreeSurface(surf);
    }
    SDL_Renderer *renderer;
    TTF_Font *font;
    int _fps = 0;
    SDL_Texture *texture = nullptr;
    SDL_Rect loc;
};

static void DrawBuffer(SDL_Renderer *sdlRenderer,
                       SDL_Texture *sdlTexture,
                       uint32_t *fb,
                       int dx)
{
    int pitch = 0;
    void *pixelsPtr;
    if (SDL_LockTexture(sdlTexture, NULL, &pixelsPtr, &pitch)) {
        throw runtime_error("Unable to lock texture");
    }
    memcpy(pixelsPtr, fb, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    SDL_UnlockTexture(sdlTexture);
    SDL_Rect r;
    r.x = dx * SCREEN_SCALE;
    r.y = 0;
    r.w = SCREEN_WIDTH * SCREEN_SCALE;
    r.h = SCREEN_HEIGHT * SCREEN_SCALE;
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &r);
}

static bool ProcessEvent(const SDL_Event &event,
                         int *moveDirection,
                         int *rotateDirection)
{
    if (event.type == SDL_QUIT) {
        return true;
    } else if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) &&
               event.key.repeat == 0) {
        auto k = event.key;
        auto p = event.type == SDL_KEYDOWN;
        switch (k.keysym.sym) {
        case SDLK_ESCAPE:
            return true;
            break;
        case SDLK_UP:
            *moveDirection = p ? 1 : 0;
            break;
        case SDLK_DOWN:
            *moveDirection = p ? -1 : 0;
            break;
        case SDLK_LEFT:
            *rotateDirection = p ? -1 : 0;
            break;
        case SDLK_RIGHT:
            *rotateDirection = p ? 1 : 0;
            break;
        default:
            break;
        }
    }
    return false;
}
int main(int argc, char *args[])
{
    if ((SDL_Init(SDL_INIT_VIDEO) < 0) || (TTF_Init() < 0)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    } else {
        SDL_Window *sdlWindow =
            SDL_CreateWindow("RayCaster [fixed-point vs. floating-point]",
                             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_SCALE * (SCREEN_WIDTH * 2 + 1),
                             SCREEN_SCALE * SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (sdlWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n",
                   SDL_GetError());
        } else {
            Game game;
            RayCasterFloat floatCaster;
            Renderer floatRenderer(&floatCaster);
            uint32_t floatBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
            RayCasterFixed fixedCaster;
            Renderer fixedRenderer(&fixedCaster);
            uint32_t fixedBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
            int moveDirection = 0;
            int rotateDirection = 0;
            bool isExiting = false;
            const auto tickFrequency = SDL_GetPerformanceFrequency();
            auto tickCounter = SDL_GetPerformanceCounter();
            auto fpsCounter = tickCounter;
            auto framecount = 0;
            SDL_Event event;

            SDL_Renderer *sdlRenderer =
                SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
            SDL_Texture *fixedTexture = SDL_CreateTexture(
                sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
            SDL_Texture *floatTexture = SDL_CreateTexture(
                sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

            fps_renderer fps(sdlRenderer);

            auto count2sec = [=](auto start, auto end) {
                return (end - start) / static_cast<float>(tickFrequency);
            };

            while (!isExiting) {
                ++framecount;
                floatRenderer.TraceFrame(&game, floatBuffer);
                fixedRenderer.TraceFrame(&game, fixedBuffer);

                DrawBuffer(sdlRenderer, fixedTexture, fixedBuffer, 0);
                DrawBuffer(sdlRenderer, floatTexture, floatBuffer,
                           SCREEN_WIDTH + 1);
                if (count2sec(fpsCounter, SDL_GetPerformanceCounter()) >=
                    1.0f) {
                    auto n = SDL_GetPerformanceCounter();
                    fps.update(framecount / count2sec(fpsCounter, n));
                    fpsCounter = n;
                    framecount = 0;
                }
                fps.render();
                SDL_RenderPresent(sdlRenderer);

                if (SDL_PollEvent(&event)) {
                    isExiting =
                        ProcessEvent(event, &moveDirection, &rotateDirection);
                }
                const auto nextCounter = SDL_GetPerformanceCounter();
                const auto seconds = count2sec(tickCounter, nextCounter);
                tickCounter = nextCounter;
                game.Move(moveDirection, rotateDirection, seconds);
            }
            SDL_DestroyTexture(floatTexture);
            SDL_DestroyTexture(fixedTexture);
            SDL_DestroyRenderer(sdlRenderer);
            SDL_DestroyWindow(sdlWindow);
        }
    }

    SDL_Quit();
    return 0;
}
