#include <stdio.h>
#include <stdlib.h>
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
#define NUM_CIRCULOS_BRILHANTES 1

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


struct pixel* CreateScreenPixels(){
    struct pixel* array = malloc(LARGURA * ALTURA * sizeof(struct pixel));

    if (!array) return NULL;

    for (int i = 0; i < ALTURA; i++){

        for (int j = 0; j < LARGURA; j++){

            int idx = i * LARGURA + j;
            //printf("ANTES XX: %.2lf\n", array[idx].x);
            array[idx].x = j;
            array[idx].y = i;
            array[idx].r = 255;
            array[idx].g = 255;
            array[idx].b = 0;
            array[idx].a = 100; // Porcentagem

            //printf("DEPOIS XX: %.2lf\n", array[idx].x);

        }

    }

    return array;

}


void DrawCircle(struct pixel* arrayPixels, struct Circle circle){
    
    // int halfRadius = circle.r/2;

    for (int y = circle.y - circle.r; y <= circle.y + circle.r; y++){
        for (int x = circle.x - circle.r; x <= circle.x + circle.r; x++){
            double distX = x - circle.x;
            double distY = y - circle.y;
            double dist_squared = pow(distX, 2) + pow(distY, 2);

            if (dist_squared <= pow(circle.r, 2)){

                int idx = y * LARGURA + x;

                struct pixel* pxl = &arrayPixels[idx];
                
                pxl->r = circle.color[0];
                pxl->g = circle.color[1];
                pxl->b = circle.color[2];
                
                // SDL_Rect rect = (SDL_Rect) {x, y, 1, 1};
                // Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, circle.color[0], circle.color[1], circle.color[2]);
                // SDL_FillSurfaceRect(surface, &rect, cor);
            }

        }
    }
    
}


void DrawCircles(struct pixel* arrayPixels, struct Circle circles[], int num_circles){
    for (int i = 0; i < num_circles; i++){
        DrawCircle(arrayPixels, circles[i]);
    }
}


void Light(struct pixel* arrayPixels, struct Circle op_circles[], struct Circle br_circles[]){


    for (int y = 0; y < ALTURA; y++){
        for (int x = 0; x < LARGURA; x++){
            
            int idx = y * LARGURA + x;

            struct pixel* pxl = &arrayPixels[idx]; 

            double pxl_alpha = 0.0; 
            
            for (int c = 0; c < NUM_CIRCULOS_BRILHANTES; c++){
                
                double distX = x - br_circles[c].x;
                double distY = y - br_circles[c].y;
                double dist = pow(pow(distX, 2) + pow(distY, 2), 0.5); // Elevando a 0.5 que é mesma coisa que fazer a raiz quadrada

                // printf("idx: %d  x: %d  y: %d  a: %.2f\n", idx, x, y, pxl->a);
                double a = 0.0;
                a = dist <= br_circles[c].r ? 100.0 : 100 * pow(.997, dist);


                // if (dist != 0 ){
                //     // a = 100/(dist * 0.01);
                    
                // }

                if (a > pxl_alpha){
                    pxl_alpha = a;
                }

            }
            
            pxl_alpha = pxl_alpha <= 0 ? 0.0 : pxl_alpha;
            // printf("idx: %d AA: %.2lf\n", idx, pxl_alpha);
            pxl->a = pxl_alpha;
            


            // SDL_Rect p = (SDL_Rect) {x, y, 1, 1};
            // Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 255, 255, 0);
            // SDL_FillSurfaceRect(surface, &p, cor);
        }
    }
        
}


void DrawScreenPixels(SDL_Surface* surface, struct pixel* arrayPixels){

    for (int i = 0; i < ALTURA; i++){
        for (int j = 0; j < LARGURA; j++){

            int idx = i * LARGURA + j;

            struct pixel pxl = arrayPixels[idx]; 

            SDL_Rect p = (SDL_Rect) {j, i, 1, 1};
            
            Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, pxl.r * (pxl.a * 0.01), pxl.g * (pxl.a * 0.01), pxl.b * (pxl.a * 0.01));
            // Uint32 cor = SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 255, 255, 255);

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
        (struct Circle) {400, 200, 50, {255,140,0}}
    };


    int qtd_pixels = LARGURA * ALTURA;

    struct pixel* ScreenPixels = CreateScreenPixels();

    bool running = true;

    SDL_Event event;

    while(running){

        while(SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }


        

        DrawCircles(ScreenPixels, opaque_circles, NUM_CIRCULOS_OPACOS);
        DrawCircles(ScreenPixels, bright_circles, NUM_CIRCULOS_BRILHANTES);
        Light(ScreenPixels, opaque_circles, bright_circles);
        
        DrawScreenPixels(surface, ScreenPixels);

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

    free(ScreenPixels);
    
    return 0;
}