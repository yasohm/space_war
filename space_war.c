#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_BULLETS 50
#define MAX_ENEMIES 10
#define MAX_STARS 100

typedef struct {
    float x, y;
    float vx, vy;
    int active;
} Bullet;

typedef struct {
    float x, y;
    float vx, vy;
    int health;
    int active;
    int type; 
} Enemy;

typedef struct {
    float x, y;
    float size;
} Star;

typedef struct {
    float x, y;
    int health;
    int score;
} Player;

Player player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
Star stars[MAX_STARS];
int gameOver = 0;

void initGame() {
    player.x = WIDTH / 2;
    player.y = HEIGHT - 80;
    player.health = 100;
    player.score = 0;
    gameOver = 0;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }
    
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = rand() % WIDTH;
        stars[i].y = rand() % HEIGHT;
        stars[i].size = 1 + rand() % 2;
    }
}

void shootBullet() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = player.x;
            bullets[i].y = player.y - 20;
            bullets[i].vx = 0;
            bullets[i].vy = -8;
            bullets[i].active = 1;
            break;
        }
    }
}

void spawnEnemy() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].type = rand() % 2;
            enemies[i].x = rand() % (WIDTH - 60) + 30;
            enemies[i].y = -50;
            enemies[i].vx = (rand() % 3 - 1) * 0.5;
            enemies[i].vy = 1 + rand() % 2;
            enemies[i].health = enemies[i].type == 0 ? 1 : 3;
            enemies[i].active = 1;
            break;
        }
    }
}

void drawShip(SDL_Renderer* renderer, float x, float y, int isPlayer) {
    if (isPlayer) {
        
        SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
        SDL_Rect body = {x - 20, y - 10, 40, 20};
        SDL_RenderFillRect(renderer, &body);
        
        SDL_SetRenderDrawColor(renderer, 150, 220, 255, 255);
        SDL_Rect cockpit = {x - 8, y - 15, 16, 10};
        SDL_RenderFillRect(renderer, &cockpit);
        
        
        SDL_SetRenderDrawColor(renderer, 80, 180, 230, 255);
        SDL_Rect wing1 = {x - 30, y, 10, 15};
        SDL_Rect wing2 = {x + 20, y, 10, 15};
        SDL_RenderFillRect(renderer, &wing1);
        SDL_RenderFillRect(renderer, &wing2);
        
        
        SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
        SDL_Rect eng1 = {x - 15, y + 10, 8, 12};
        SDL_Rect eng2 = {x + 7, y + 10, 8, 12};
        SDL_RenderFillRect(renderer, &eng1);
        SDL_RenderFillRect(renderer, &eng2);
    }
}

void drawFighter(SDL_Renderer* renderer, float x, float y) {
    
    SDL_SetRenderDrawColor(renderer, 200, 100, 80, 255);
    SDL_Rect body = {x - 12, y - 8, 24, 16};
    SDL_RenderFillRect(renderer, &body);
    
    SDL_SetRenderDrawColor(renderer, 220, 80, 60, 255);
    SDL_Rect wing1 = {x - 20, y - 5, 8, 10};
    SDL_Rect wing2 = {x + 12, y - 5, 8, 10};
    SDL_RenderFillRect(renderer, &wing1);
    SDL_RenderFillRect(renderer, &wing2);
    
    
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_Rect eng1 = {x - 8, y + 8, 4, 6};
    SDL_Rect eng2 = {x + 4, y + 8, 4, 6};
    SDL_RenderFillRect(renderer, &eng1);
    SDL_RenderFillRect(renderer, &eng2);
}

void drawDestroyer(SDL_Renderer* renderer, float x, float y) {

    SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
    SDL_Rect body = {x - 25, y - 10, 50, 20};
    SDL_RenderFillRect(renderer, &body);
    
    SDL_SetRenderDrawColor(renderer, 140, 140, 150, 255);
    SDL_Rect bridge = {x - 10, y - 15, 20, 10};
    SDL_RenderFillRect(renderer, &bridge);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
    SDL_Rect wing1 = {x - 35, y + 5, 10, 8};
    SDL_Rect wing2 = {x + 25, y + 5, 10, 8};
    SDL_RenderFillRect(renderer, &wing1);
    SDL_RenderFillRect(renderer, &wing2);
}

void update() {

    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].y += 0.5;
        if (stars[i].y > HEIGHT) {
            stars[i].y = 0;
            stars[i].x = rand() % WIDTH;
        }
    }
    

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            
            if (bullets[i].y < 0 || bullets[i].y > HEIGHT) {
                bullets[i].active = 0;
            }
        }
    }
    

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].x += enemies[i].vx;
            enemies[i].y += enemies[i].vy;
            
            if (enemies[i].y > HEIGHT + 50) {
                enemies[i].active = 0;
            }
            
            float dx = enemies[i].x - player.x;
            float dy = enemies[i].y - player.y;
            float dist = sqrt(dx*dx + dy*dy);
            
            if (dist < 35) {
                player.health -= 10;
    
                enemies[i].active = 0;
                if (player.health <= 0) {
                    gameOver = 1;
                }
            }
        }
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active) {
                    float dx = bullets[i].x - enemies[j].x;
                    float dy = bullets[i].y - enemies[j].y;
                    float dist = sqrt(dx*dx + dy*dy);
                    
                    if (dist < 25) {
                        bullets[i].active = 0;
                        enemies[j].health--;
                        if (enemies[j].health <= 0) {
                            enemies[j].active = 0;
                            player.score += enemies[j].type == 0 ? 10 : 30;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < MAX_STARS; i++) {
        SDL_Rect star = {stars[i].x, stars[i].y, stars[i].size, stars[i].size};
        SDL_RenderFillRect(renderer, &star);
    }
    
    drawShip(renderer, player.x, player.y, 1);
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_Rect bullet = {bullets[i].x - 2, bullets[i].y - 8, 4, 16};
            SDL_RenderFillRect(renderer, &bullet);
        }
    }
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            if (enemies[i].type == 0) {
                drawFighter(renderer, enemies[i].x, enemies[i].y);
            } else {
                drawDestroyer(renderer, enemies[i].x, enemies[i].y);
            }
        }
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect healthBar = {10, 10, player.health * 2, 20};
    SDL_RenderFillRect(renderer, &healthBar);
    
    if (gameOver) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect gameOverRect = {WIDTH/2 - 150, HEIGHT/2 - 50, 300, 100};
        SDL_RenderFillRect(renderer, &gameOverRect);
    }
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Space Shooter", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    srand(time(NULL));
    initGame();
    
    int running = 1;
    SDL_Event e;
    Uint32 lastSpawn = SDL_GetTicks();
    Uint32 lastShot = 0;
    
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r && gameOver) {
                initGame();
            }
        }
        
        if (!gameOver) {
            const Uint8* keys = SDL_GetKeyboardState(NULL);
            
            if (keys[SDL_SCANCODE_LEFT] && player.x > 30) {
                player.x -= 5;
            }
            if (keys[SDL_SCANCODE_RIGHT] && player.x < WIDTH - 30) {
                player.x += 5;
            }
            if (keys[SDL_SCANCODE_UP] && player.y > 30) {
                player.y -= 5;
            }
            if (keys[SDL_SCANCODE_DOWN] && player.y < HEIGHT - 30) {
                player.y += 5;
            }
            if (keys[SDL_SCANCODE_SPACE] && SDL_GetTicks() - lastShot > 200) {
                shootBullet();
                lastShot = SDL_GetTicks();
            }
            
            if (SDL_GetTicks() - lastSpawn > 1500) {
                spawnEnemy();
                lastSpawn = SDL_GetTicks();
            }
            
            update();
        }
        
        render(renderer);
        
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}