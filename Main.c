#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL3/SDL.h>

#define LARGURA 700
#define ALTURA 600
#define BRANCO 0xffffffff
#define AMARELO 0xffffff00
#define LARANJA 0xffff8800
#define RGB_BRANCO = {255,255,255}
#define RGB_AMARELO = {255,255,0}
#define RGB_LARANJA = {255,140,0}
#define NUM_CIRCULOS_OPACOS 1
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
    int color[3];
};


struct pixel* CreateScreenPixels(){
    struct pixel* array = malloc(LARGURA * ALTURA * sizeof(struct pixel));

    if (!array) return NULL;

    for (int i = 0; i < ALTURA; i++){

        for (int j = 0; j < LARGURA; j++){

            int idx = i * LARGURA + j;
            array[idx].x = j;
            array[idx].y = i;
            array[idx].r = 0;
            array[idx].g = 0;
            array[idx].b = 255;
            array[idx].a = 100; // Porcentagem

        }

    }

    return array;

}


void ResetScreenPixels(struct pixel* array){

    for (int i = 0; i < ALTURA; i++){

        for (int j = 0; j < LARGURA; j++){

            int idx = i * LARGURA + j;

            array[idx].r = 0;
            array[idx].g = 0;
            array[idx].b = 255;
            array[idx].a = 100;

        }
    
    }

}

void DrawCircle(struct pixel* arrayPixels, struct Circle circle){
    
    for (int y = circle.y - circle.r; y <= circle.y + circle.r; y++){
        for (int x = circle.x - circle.r; x <= circle.x + circle.r; x++){

            if (x > LARGURA || x < 0 || y > ALTURA || y < 0){
                continue;
            }

            double distX = x - circle.x;
            double distY = y - circle.y;
            double dist_squared = pow(distX, 2) + pow(distY, 2);

            if (dist_squared <= pow(circle.r, 2)){

                int idx = y * LARGURA + x;
                
                if (idx >= LARGURA * ALTURA){
                    break;
                }

                struct pixel* pxl = &arrayPixels[idx];
                
                pxl->r = circle.color[0];
                pxl->g = circle.color[1];
                pxl->b = circle.color[2];
                
            }

        }
    }
    
}


void DrawCircles(struct pixel* arrayPixels, struct Circle circles[], int num_circles){
    for (int i = 0; i < num_circles; i++){
        DrawCircle(arrayPixels, circles[i]);
    }
}


double DistanciaPontoSegmento(struct Circle op_circle, struct Circle br_circle, double x, double y) {
    double dx = x - br_circle.x;
    double dy = y - br_circle.y;

    if (dx == 0 && dy == 0)
        return hypot(op_circle.x - br_circle.x, op_circle.y - br_circle.y);  // segmento é um ponto

    double t = ((op_circle.x - br_circle.x) * dx + (op_circle.y - br_circle.y) * dy) / (dx*dx + dy*dy);
    t = fmax(0, fmin(1, t));

    double projX = br_circle.x + t * dx;
    double projY = br_circle.y + t * dy;

    return hypot(op_circle.x - projX, op_circle.y - projY);
}

int IsBehindOpaqueCircles(double x, double y, struct Circle op_circles[], struct Circle br_circle){
    
    int isBehind = 0;

    for (int i = 0; i < NUM_CIRCULOS_OPACOS; i++){
        
        double dist = DistanciaPontoSegmento(op_circles[i], br_circle, x, y);

        if (dist < op_circles[i].r){
            isBehind = 1;
        }


    }
    

    return isBehind;

}

void Light(struct pixel* arrayPixels, struct Circle op_circles[], struct Circle br_circles[]){


    for (int y = 0; y < ALTURA; y++){
        for (int x = 0; x < LARGURA; x++){
            
            int idx = y * LARGURA + x;

            struct pixel* pxl = &arrayPixels[idx]; 

            double pxl_alpha = 0.0; 
            
            for (int c = 0; c < NUM_CIRCULOS_BRILHANTES; c++){

                int isBehind = IsBehindOpaqueCircles(x, y, op_circles, br_circles[c]);

                double distX = x - br_circles[c].x;
                double distY = y - br_circles[c].y;
                double dist = pow(pow(distX, 2) + pow(distY, 2), 0.5); // Elevando a 0.5 que é mesma coisa que fazer a raiz quadrada

                double reductionFactor = isBehind ? .9942 : .995;
                double a = dist <= br_circles[c].r ? 100.0 : 100 * pow(reductionFactor, dist);

                if (a > pxl_alpha){
                    pxl_alpha = a;
                }

                

            }
            
            pxl_alpha = pxl_alpha <= 1 ? 0.0 : pxl_alpha;
            pxl->a = pxl_alpha;
            
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

            SDL_FillSurfaceRect(surface, &p, cor);

        }
    }
}



void handleClickMouse(struct Circle* op_circles, struct Circle* br_circles, struct Circle** dragging){
    float mouse_x, mouse_y;
    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
        for (int i = 0; i < NUM_CIRCULOS_BRILHANTES; i++){
            struct Circle* circle = &br_circles[i];

            if (*dragging != NULL && *dragging != circle){
                continue;
            }

            if ((mouse_x <= circle->x + circle->r && mouse_x >= circle->x - circle->r) && 
            (mouse_y <= circle->y + circle->r && mouse_y >= circle->y - circle->r)){
                circle->x = mouse_x;
                circle->y = mouse_y;

                *dragging = circle;
                break;
            }
            
        }

        for (int i = 0; i < NUM_CIRCULOS_OPACOS; i++){
            struct Circle* circle = &op_circles[i];

            if (*dragging != NULL && *dragging != circle){
                continue;
            }

            if ((mouse_x <= circle->x + circle->r && mouse_x >= circle->x - circle->r) && 
            (mouse_y <= circle->y + circle->r && mouse_y >= circle->y - circle->r)){
                circle->x = mouse_x;
                circle->y = mouse_y;
                
                *dragging = circle;

                break;
            }
            
        }
    }
}

int main(){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", LARGURA, ALTURA, 0);
    SDL_Renderer* render = SDL_CreateRenderer(window, NULL);
    SDL_Surface* surface = SDL_CreateSurface(LARGURA, ALTURA, SDL_PIXELFORMAT_RGBA8888);// SDL_GetWindowSurface(window);

    struct Circle opaque_circles[NUM_CIRCULOS_OPACOS] = {
        (struct Circle) {400, 200, 100, {255,255,255}}
        //(struct Circle) {400, 400, 50, {255,255,255}}
    };

    struct Circle bright_circles[NUM_CIRCULOS_BRILHANTES] = {
        (struct Circle) {250, 300, 50, {255,140,0}}
    };


    int qtd_pixels = LARGURA * ALTURA;

    struct pixel* ScreenPixels = CreateScreenPixels();

    bool running = true;

    SDL_Event event;

    struct Circle* dragging_circle = NULL;

    while(running){

        while(SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging_circle = NULL;
                }
            }
        }


        ResetScreenPixels(ScreenPixels);
        
        handleClickMouse(opaque_circles, bright_circles, &dragging_circle);

        DrawCircles(ScreenPixels, bright_circles, NUM_CIRCULOS_BRILHANTES);
        DrawCircles(ScreenPixels, opaque_circles, NUM_CIRCULOS_OPACOS);
        
        Light(ScreenPixels, opaque_circles, bright_circles);
        
        DrawScreenPixels(surface, ScreenPixels);

        SDL_Texture* textura = SDL_CreateTextureFromSurface(render, surface);
        SDL_RenderClear(render);
        SDL_RenderTexture(render, textura, NULL, NULL);
        SDL_RenderPresent(render);
        //SDL_UpdateWindowSurface(window);
    
        SDL_Delay(10);
    }


    SDL_DestroySurface(surface);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(ScreenPixels);
    
    return 0;
}