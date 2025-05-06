#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>

#define LARGURA 1000
#define ALTURA 600
#define BRANCO 0xffffffff

struct Circle{
    double x;
    double y;
    double r;
    Uint32 color;
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
                SDL_FillSurfaceRect(surface, &rect, circle.color);
            }

        }
    }

}


int main(){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", LARGURA, ALTURA, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    struct Circle circle1 = (struct Circle) {200, 200, 25, BRANCO};
    DrawCircle(surface, circle1);
    SDL_UpdateWindowSurface(window);



    SDL_Delay(2000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}