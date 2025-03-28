#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string.h>

static const char *WINDOW_TITLE = "Invert image";
static const char *IMAGE_FILENAME = "kodim23.png";

SDL_Color duotoneColor1 = {0, 0, 128};  
SDL_Color duotoneColor2 = {255, 165, 0}; 

static void convertsepia(void);     
static void convertduotone(void);   
static void loadRGBA32(const char *filename);   
static void applyOilPaintingEffect(void);   
static void applyTintEffect(Uint8 tr, Uint8 tg, Uint8 tb, Uint8 ta);      
static void convertblackcover(void);     
static void convertwhitecover(void);
static void cinzaEquacao(void);

enum constants
{
  WINDOW_WIDTH = 640,
  WINDOW_HEIGHT = 480,
};

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Surface *surface = NULL;
static SDL_Texture *texture = NULL;
static SDL_FRect textureRect = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f };
static int placeholder = 0;

static SDL_AppResult initialize(void)
{
  SDL_Log("Iniciando SDL...\n");
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("*** Erro ao iniciar a SDL: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Log("Criando janela e renderizador...\n");
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &window, &renderer))
  {
    SDL_Log("Erro ao criar a janela e/ou renderizador: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

static void shutdown(void)
{
  SDL_Log("\nDestruindo textura...\n");
  SDL_DestroyTexture(texture);
  texture = NULL;

  SDL_Log("Destruindo superfície...\n");
  SDL_DestroySurface(surface);
  surface = NULL;

  SDL_Log("Destruindo renderizador...\n");
  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_Log("Destruindo janela...\n");
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Log("Encerrando SDL...\n");
  SDL_Quit();
}

static void loop(void) {
    SDL_Event event;
    bool isRunning = true;
    printf("\n************************\nBem-vindo ao aplicador de filtros artísticos!\n\n0. Imagem sem filtro\n1. Sepia\n2. Duotone\n3. Pintura a óleo\n4. Tinto\n5. Tela Branca\n6. Tela Preta\n7. Tela cinza\n");
    int toggleEffect = 0;  // Variável de estado para alternar entre preto e branco

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    isRunning = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_0 && !event.key.repeat && placeholder != 0) {
                        loadRGBA32(IMAGE_FILENAME);
                        placeholder = 0;
                    }

                    if (event.key.key == SDLK_1 && !event.key.repeat && placeholder != 1) {
                        loadRGBA32(IMAGE_FILENAME);
                        convertsepia();
                        placeholder = 1;
                    }

                    if (event.key.key == SDLK_2 && !event.key.repeat && placeholder != 2) {
                        loadRGBA32(IMAGE_FILENAME);
                        convertduotone();
                        placeholder = 2;
                    }

                    if (event.key.key == SDLK_3 && !event.key.repeat && placeholder != 3) {
                        loadRGBA32(IMAGE_FILENAME);
                        applyOilPaintingEffect();
                        placeholder = 3;
                    }

                    if (event.key.key == SDLK_4 && !event.key.repeat && placeholder != 4) {
                        loadRGBA32(IMAGE_FILENAME);
                        applyTintEffect(255, 0, 0, 128);
                        placeholder = 4;
                    }

                    if (event.key.key == SDLK_5 && !event.key.repeat && placeholder != 5) {
                        loadRGBA32(IMAGE_FILENAME);
                        convertwhitecover();
                        placeholder = 5;
                    }
                    if (event.key.key == SDLK_6 && !event.key.repeat && placeholder != 6) {
                        loadRGBA32(IMAGE_FILENAME);
                        convertblackcover();
                        placeholder = 6;
                    }
                    if (event.key.key == SDLK_7 && !event.key.repeat && placeholder != 7 ){
                        loadRGBA32(IMAGE_FILENAME);
                        cinzaEquacao();
                        placeholder = 7;
                    }
                    break;
            }
        }

        // Renderiza a imagem
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, &textureRect, &textureRect);
        SDL_RenderPresent(renderer);
    }
}


void loadRGBA32(const char *filename){
  surface = IMG_Load(filename);
  if (!surface)
  {
    SDL_Log("*** Erro ao carregar a imagem: %s\n", SDL_GetError());
    return;
  }

  SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
  SDL_DestroySurface(surface);
  if (!converted)
  {
    SDL_Log("*** Erro ao converter superfície para formato RGBA32: %s\n", SDL_GetError());
    surface = NULL;
    return;
  }

  surface = converted;

  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
  {
    SDL_Log("*** Erro ao criar textura: %s\n", SDL_GetError());
    return;
  }

  SDL_GetTextureSize(texture, &textureRect.w, &textureRect.h);
}

void convertsepia(void){
   if (!surface) {
    SDL_Log("*** Erro em applySepiaFilter(): Imagem inválida!\n");
    return;
  }

  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = surface->w * surface->h;

  Uint32 *pixels = (Uint32 *)surface->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 0;
  int tr,tg,tb; 

  for (size_t i = 0; i < pixelCount; ++i) {
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);

    
    tr = (int)(0.393 * r + 0.769 * g + 0.189 * b);
    tg = (int)(0.349 * r + 0.686 * g + 0.168 * b);
    tb = (int)(0.272 * r + 0.534 * g + 0.131 * b);

  
    r = (tr > 255) ? 255 : tr;
    g = (tg > 255) ? 255 : tg;
    b = (tb > 255) ? 255 : tb;


    pixels[i] = SDL_MapRGBA(format,NULL, r, g, b ,a);
  }

  SDL_UnlockSurface(surface);

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void convertduotone(void)
{
  if (!surface)
  {
    SDL_Log("*** Erro em convertduotone(): Imagem inválida!\n");
    return;
  }

  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = surface->w * surface->h;

  Uint32 *pixels = (Uint32 *)surface->pixels;
  Uint8 r = 0, g = 0, b = 0, a = 0, m = 0;

  for (size_t i = 0; i < pixelCount; ++i)
  {
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);

    m = (r + g + b) / 3;
    
    if (m < 128) {
	pixels[i] = SDL_MapRGBA(format, NULL, duotoneColor1.r, duotoneColor1.g, duotoneColor1.b, a);
    }
	    
    else {
	pixels[i] = SDL_MapRGBA(format, NULL, duotoneColor2.r, duotoneColor2.g, duotoneColor2.b, a);
    }
  }

  SDL_UnlockSurface(surface);

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void applyOilPaintingEffect(void) {
    if (!surface) {
      SDL_Log("*** Erro em applyOilPaintingEffect(): Imagem inválida!\n");
      return;
    }
  
    SDL_LockSurface(surface);
  
    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
    const size_t pixelCount = surface->w * surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint8 r, g, b, a;
  
    
    for (int y = 1; y < surface->h - 1; y++) {
      for (int x = 1; x < surface->w - 1; x++) {
        int sumR = 0, sumG = 0, sumB = 0;
        int count = 0;
  
        
        for (int j = -1; j <= 1; j++) {
          for (int i = -1; i <= 1; i++) {
            SDL_GetRGBA(pixels[(y + j) * surface->w + (x + i)], format, NULL, &r, &g, &b, &a);
            sumR += r;
            sumG += g;
            sumB += b;
            count++;
          }
        }
  
        r = sumR / count;
        g = sumG / count;
        b = sumB / count;
  
        
        pixels[y * surface->w + x] = SDL_MapRGBA(format, NULL, r, g, b, a);
      }
    }
  
    SDL_UnlockSurface(surface);
  
    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void applyTintEffect(Uint8 tr, Uint8 tg, Uint8 tb, Uint8 ta){
   if (!surface) {
    SDL_Log("*** Erro em applySepiaFilter(): Imagem inválida!\n");
    return;
  }

  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = surface->w * surface->h;

  Uint32 *pixels = (Uint32 *)surface->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 0;
  	
  for (size_t i = 0; i< pixelCount; i++){
	SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);
	
    	r = (r * (255 - ta) + tr * ta) /255;
   	g = (g * (255 - ta) + tg * ta) / 255;
    	b = (b * (255 - ta) + tb * ta) / 255;

	
    	pixels[i] = SDL_MapRGBA(format,NULL, r, g, b ,a);
  }

  SDL_UnlockSurface(surface);

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void convertblackcover(void){
   if (!surface) {
    SDL_Log("*** Erro em applySepiaFilter(): Imagem inválida!\n");
    return;
  }

  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = surface->w * surface->h;

  Uint32 *pixels = (Uint32 *)surface->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 0;
  int tr,tg,tb; 
	
  for (size_t i = 0; i < pixelCount;i++){
      SDL_GetRGBA(pixels[i],format,NULL, &r, &g, &b, &a);
	
	tr =  r * 0.25;
      	tg = g * 0.25;
	tb = b * 0.25;
	
        r = (tr > 255) ? 255 : tr;
        g = (tg > 255) ? 255 : tg;
        b = (tb > 255) ? 255 : tb;
	
       pixels[i] = SDL_MapRGBA(format,NULL, r, g, b ,a);
   }	

  SDL_UnlockSurface(surface);

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void convertwhitecover(void){
   if (!surface) {
    SDL_Log("*** Erro em applySepiaFilter(): Imagem inválida!\n");
    return;
  }

  SDL_LockSurface(surface);

  const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
  const size_t pixelCount = surface->w * surface->h;

  Uint32 *pixels = (Uint32 *)surface->pixels;
  Uint8 r = 0;
  Uint8 g = 0;
  Uint8 b = 0;
  Uint8 a = 0;
  int tr,tg,tb; 
	
  for (size_t i = 0; i< pixelCount; ++i){
    SDL_GetRGBA(pixels[i], format, NULL, &r, &g, &b, &a);
	
    tr = r + (0.25 * (255-r));
    tg = g + (0.25 * (255-g));
    tb = b + (0.25 * (255-b));

    r = (tr > 255) ? 255 : tr;
    g = (tg > 255) ? 255 : tg;
    b = (tb > 255) ? 255 : tb;
	
    pixels[i] = SDL_MapRGBA(format,NULL, r, g, b ,a);
  }

  SDL_UnlockSurface(surface);

  SDL_DestroyTexture(texture);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void cinzaEquacao(void) {
    if (!surface) {
        SDL_Log("*** Erro em invertImage(): Imagem inválida!\n");
        return;
    }

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

    SDL_DestroyTexture(texture);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
}

int main(int argc, char *argv[]){
  atexit(shutdown);

  if (initialize() == SDL_APP_FAILURE)
    return SDL_APP_FAILURE;

  loadRGBA32(IMAGE_FILENAME);

  if (textureRect.w > WINDOW_WIDTH || textureRect.h > WINDOW_HEIGHT) {
    SDL_SetWindowSize(window, (int)textureRect.w, (int)textureRect.h);
    SDL_SyncWindow(window);
  }

  loop();

  return 0;
}
