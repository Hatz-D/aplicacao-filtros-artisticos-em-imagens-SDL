#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_FRect textureRect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f };

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Filtros Artísticos", 768, 512, 0, &window, &renderer);
    SDL_Surface *surface = surface = IMG_Load("kodim23.png");
    surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_GetTextureSize(texture, &textureRect.w, &textureRect.h);
    
    SDL_Event event;

    while(1) {
        if(SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT) {break;}

        SDL_LockSurface(surface);

        const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
        const size_t pixelCount = surface->w * surface->h;

        Uint32 *pixels = (Uint32 *)surface->pixels;
        Uint8 r = 0;
        Uint8 g = 0;
        Uint8 b = 0;
        Uint8 a = 0;
        Uint8 m = 0;

        for (size_t i = 0; i < pixelCount; ++i) {
            SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);
        
            m = r * 0.2126 + g * 0.7152 + b * 0.0722;
        
            pixels[i] = SDL_MapRGBA(format, NULL, m, m, m, a);
        }

        SDL_UnlockSurface(surface);

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderTexture(renderer, texture, &textureRect, &textureRect);
        SDL_RenderPresent(renderer);
    }

    return 0;
}
