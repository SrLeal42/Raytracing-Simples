#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>

#define LARGURA 1000
#define ALTURA 600
#define BRANCO 0xffffffff
#define AMARELO 0xffffff00
#define LARANJA 0xffff8800
#define RGB_BRANCO = (int[3]){255,255,255}
#define RGB_AMARELO = {255,255,0}
#define RGB_LARANJA = {255,140,0}
#define NUM_CIRCULOS_OPACOS 2
#define NUM_CIRCULOS_BRILHANTES 2

struct pixel {
    double x;
    double y;
    int r;
    int g; 
    int b;
    double a;
};

struct Circle{
    double x;
    double y;
    double r;
    // Uint32 color;
    int color[3];
};


void DrawCircle(SDL_Surface* surface, struct Circle circle){
    
    // int halfRadius = circle.r/2;

    for (int y = circle.y - circle.r; y <= circle.y + circle.r; y++){
        for (int x = circle.x - circle.r; x <= circle.x + circle.r; x++){
            double distX = x - circle.x;
            double distY = y - circle.y;
            double dist_squared = pow(distX, 2) + pow(distY, 2);

            if (dist_squared <= pow(circle.r, 2)){
                SDL_Rect rect = (SDL_Rect) {x, y, 1, 1};
                Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, circle.color[0], circle.color[1], circle.color[2]);
                SDL_FillSurfaceRect(surface, &rect, cor);
            }

        }
    }

}


void DrawCircles(SDL_Surface* surface, struct Circle circles[], int num_circles){
    for (int i = 0; i < num_circles; i++){
        DrawCircle(surface, circles[i]);
    }
}


void Light(SDL_Surface* surface, struct Circle op_circles[], struct Circle br_circles[]){

    for (int y = 0; y <= ALTURA; y++){
        for (int x = 0; x <= LARGURA; x++){
            SDL_Rect p = (SDL_Rect) {x, y, 1, 1};
            Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 255, 255, 0);
            SDL_FillSurfaceRect(surface, &p, cor);
        }
    }

}


int main(){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", LARGURA, ALTURA, 0);
    SDL_Renderer* render = SDL_CreateRenderer(window, NULL);
    SDL_Surface* surface = SDL_CreateSurface(LARGURA, ALTURA, SDL_PIXELFORMAT_RGBA8888);// SDL_GetWindowSurface(window);

    struct Circle opaque_circles[NUM_CIRCULOS_OPACOS] = {
        (struct Circle) {700, 200, 50, {255,255,255}},
        (struct Circle) {700, 400, 50, {255,255,255}}
    };

    struct Circle bright_circles[NUM_CIRCULOS_BRILHANTES] = {
        (struct Circle) {200, 200, 50, {255,140,0}}
    };


    int qtd_pixels = LARGURA * ALTURA;

    bool running = true;

    SDL_Event event;

    while(running){

        while(SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }


        Light(surface, opaque_circles, bright_circles);

        DrawCircles(surface, opaque_circles, NUM_CIRCULOS_OPACOS);
        DrawCircles(surface, bright_circles, NUM_CIRCULOS_BRILHANTES);    

        SDL_Texture* textura = SDL_CreateTextureFromSurface(render, surface);
        SDL_RenderClear(render);
        SDL_RenderTexture(render, textura, NULL, NULL);
        SDL_RenderPresent(render);
        //SDL_UpdateWindowSurface(window);
    
        SDL_Delay(10);
    }



    //SDL_Delay(2000);
    SDL_DestroySurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}