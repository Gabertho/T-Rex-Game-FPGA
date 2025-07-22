#include "address_map_arm.h"
#include <string.h>
#include <stdlib.h>

void video_text(int, int, char *);
void video_box(int, int, int, int, short);
void clear_screen(void);
void draw_cloud(int x, int y);
void draw_sun(int x, int y);
void draw_stars(void);     // <-- ADICIONE ISSO
void draw_moon(int x, int y);  // <-- também adicione esse se ainda não tiver
void draw_dino(int, int, int, int);
void clear_text(void);
void draw_obstacle(int, int);
void draw_ground(void);
void draw_score(int);
int  resample_rgb(int, int);
int  get_data_bits(int);
int  check_collision(int, int, int, int);
void delay(int);
int  random_range(int, int);
void draw_bird(int, int, int);  // <-- Aqui

#define STANDARD_X 320
#define STANDARD_Y 240
#define GROUND_Y 200
#define DINO_WIDTH 16
#define DINO_HEIGHT 20
#define OBSTACLE_WIDTH 8
#define OBSTACLE_HEIGHT 20
#define JUMP_HEIGHT 40
#define GAME_SPEED 4
#define BIRD_WIDTH 12
#define BIRD_HEIGHT 10

#define BLACK 0x0000
#define WHITE 0xFFFF
#define PINK 0xF6B6
#define GREEN 0x07E0
#define RED 0xF800
#define GRAY 0x7BEF

int screen_x;
int screen_y;
int res_offset;
int col_offset;
int db;

int dino_x = 30;
int dino_y = GROUND_Y - DINO_HEIGHT;
int dino_jump_state = 0;
int jump_counter = 0;
int obstacle_x = STANDARD_X;
int obstacle_y = GROUND_Y - OBSTACLE_HEIGHT;
int obstacle_x2 = STANDARD_X + 100; // Second obstacle for variety
int score = 0;
int night_mode = 0;  // 0 = dia, 1 = noite
int game_over = 0;
int frame_counter = 0;
int current_speed = GAME_SPEED;
int bird_x = STANDARD_X + 320;
int bird_y = 160; // altura média do voo


#define LOGO_WIDTH 32
#define LOGO_HEIGHT 32
#define LOGO_SCALE 4

const unsigned char logo_bitmap[LOGO_HEIGHT][LOGO_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}


};



void draw_logo(int x, int y) {
    short color = resample_rgb(db, PINK);
    int row, col, dx, dy;
    for (row = 0; row < LOGO_HEIGHT; row++) {
        for (col = 0; col < LOGO_WIDTH; col++) {
            if (logo_bitmap[row][col] == 1) {
                // Desenha um quadrado de tamanho LOGO_SCALE x LOGO_SCALE
                for (dy = 0; dy < LOGO_SCALE; dy++) {
                    for (dx = 0; dx < LOGO_SCALE; dx++) {
                        int x1 = x + col * LOGO_SCALE + dx;
                        int y1 = y + row * LOGO_SCALE + dy;
                        video_box(x1, y1, x1, y1, color);
                    }
                }
            }
        }
    }
}







int main(void) {
    volatile int * video_resolution = (int *)(PIXEL_BUF_CTRL_BASE + 0x8);
    screen_x = *video_resolution & 0xFFFF;
    screen_y = (*video_resolution >> 16) & 0xFFFF;

    volatile int * rgb_status = (int *)(RGB_RESAMPLER_BASE);
    db = get_data_bits(*rgb_status & 0x3F);

    res_offset = (screen_x == 160) ? 1 : 0;
    col_offset = (db == 8) ? 1 : 0;
    srand(42);

    volatile int * key_ptr = (int *)KEY_BASE;

    while (1) {
        short box_color, text_color;
        int box_x1, box_y1, box_x2, box_y2;
        char score_text[30];
        int temp_score, i, start, end;

        dino_x = 30;
        dino_y = GROUND_Y - DINO_HEIGHT;
        dino_jump_state = 0;
        jump_counter = 0;
        obstacle_x = STANDARD_X;
        obstacle_x2 = STANDARD_X + random_range(50, 150);
        obstacle_y = GROUND_Y - OBSTACLE_HEIGHT;
        bird_x = STANDARD_X + 100;   // Inicia bem fora da tela, à direita
        bird_y = 160;                // Altura fixa para voo
        score = 0;
        game_over = 0;
        frame_counter = 0;
        current_speed = GAME_SPEED;

        clear_screen();
        
        clear_text();
        char *title = "Carol Dino Game";
        int x_pos = (80 - strlen(title)) / 2;

        video_text(23, 6, "T H E   D I N O S A U R   G A M E");
        draw_logo(96, 60);






        
        while ((*key_ptr & 0x2) != 0);
        delay(1000);
        while ((*key_ptr & 0x2) == 0);
        
        while (!game_over) {
            clear_screen();

            if (!night_mode) {
                draw_sun(10, 10);
                draw_cloud(60, 30);
                draw_cloud(160, 20);
                draw_cloud(240, 35);
            } else {
                draw_moon(10, 10);
                draw_stars();
            }



            // Controle de pulo
            if ((*key_ptr & 0x1) != 0 && dino_jump_state == 0) {
                dino_jump_state = 1;
                jump_counter = 0;
            }

            if (dino_jump_state == 1) {
                jump_counter++;
                dino_y = (GROUND_Y - DINO_HEIGHT) - (JUMP_HEIGHT * jump_counter / 10);
                if (jump_counter >= 10) {
                    dino_jump_state = 2;
                }
            } else if (dino_jump_state == 2) {
                jump_counter--;
                dino_y = (GROUND_Y - DINO_HEIGHT) - (JUMP_HEIGHT * jump_counter / 10);
                if (jump_counter <= 0) {
                    dino_jump_state = 0;
                    dino_y = GROUND_Y - DINO_HEIGHT;
                }
            }

            // Obstáculo terrestre
            obstacle_x -= current_speed;
            obstacle_x2 -= current_speed;
            current_speed = GAME_SPEED + (score / 50);

            if (obstacle_x < -OBSTACLE_WIDTH)
                obstacle_x = STANDARD_X + random_range(50, 150);

            if (obstacle_x2 < -OBSTACLE_WIDTH)
                obstacle_x2 = STANDARD_X + random_range(50, 150);

            // Movimento e colisão do passarinho
            if (score >= 50) {
                bird_x -= current_speed;

                if (bird_x < -BIRD_WIDTH) {
                    bird_x = STANDARD_X + random_range(50, 200);
                    bird_y = 160; 
                }

                if (check_collision(dino_x, dino_y, bird_x, bird_y)) {
                    game_over = 1;
                }

                draw_bird(bird_x, bird_y, frame_counter);
            }

            // Colisão com obstáculos terrestres
            if (check_collision(dino_x, dino_y, obstacle_x, obstacle_y) ||
                check_collision(dino_x, dino_y, obstacle_x2, obstacle_y)) {
                game_over = 1;
            }

            draw_ground();
            draw_dino(dino_x, dino_y, frame_counter,dino_jump_state);
            draw_obstacle(obstacle_x, obstacle_y);
            draw_obstacle(obstacle_x2, obstacle_y);
            draw_score(score);

            delay(5000);
            frame_counter++;
            night_mode = (score / 50) % 2;
            if (frame_counter % 5 == 0) score++;
        }

        
        clear_screen();
        clear_text();

        
        // Caixa central marrom escura
        box_color = resample_rgb(db, 0x4208); // marrom escuro
        text_color = resample_rgb(db, WHITE);
        box_x1 = 60;
        box_x2 = 260; // continua igual

        box_y1 = 30;  // antes 70
        box_y2 = 170;


        video_box(box_x1, box_y1, box_x2, box_y2, box_color);

        // Texto "GAME OVER!" centralizado
        video_text(31, 12, " G A M E   O V E R !");


        // Mostrar SCORE centralizado
        score_text[30];
        temp_score = score;
        score = 0;
        score_text[0] = 'S'; score_text[1] = 'C'; score_text[2] = 'O';
        score_text[3] = 'R'; score_text[4] = 'E'; score_text[5] = ':'; 
        score_text[6] = ' ';

        
        if (temp_score == 0) {
            score_text[7] = '0';
            score_text[8] = '\0';
        } else {
            int i = 7;
            while (temp_score > 0) {
                score_text[i] = (temp_score % 10) + '0';
                temp_score /= 10;
                i++;
            }
            score_text[i] = '\0';
            int start = 7, end = i - 1;
            while (start < end) {
                char temp = score_text[start];
                score_text[start] = score_text[end];
                score_text[end] = temp;
                start++;
                end--;
            }
        }

                video_text(36, 15, score_text);

        // Mensagem para reinício
        video_text(28, 18, "Pressione KEY1 para reiniciar");

        // (opcional) desenha o dinossauro parado
        draw_dino(140, 170, 0, 0);

        
        
        while ((*key_ptr & 0x2) != 0);
        while ((*key_ptr & 0x2) == 0);
    }
    
    return 0;
}

void clear_text(void) {
    volatile char * character_buffer = (char *)FPGA_CHAR_BASE;
    int y, x;
    for (y = 0; y < 60; y++) {
        for (x = 0; x < 80; x++) {
            int offset = (y << 7) + x;
            character_buffer[offset] = ' ';
        }
    }
}


void clear_screen(void) {
    short bg_color = night_mode ? resample_rgb(db, 0x0010) : resample_rgb(db, BLACK); // Azul escuro à noite
    video_box(0, 0, STANDARD_X, STANDARD_Y, bg_color);
}


void draw_dino(int x, int y, int frame, int is_jumping) {
    short color = night_mode ? resample_rgb(db, 0xC618) : resample_rgb(db, GREEN);

    const unsigned char dino1[20][16] = {
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,1,0,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,0,1,1,1,1,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

    const unsigned char dino2[20][16] = {
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {1,1,0,1,1,1,1,1,1,0,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

    int row;
    int col;
    const unsigned char (*dino_bitmap)[16];
    if (is_jumping != 0) {
        dino_bitmap = dino1; // Usa sprite padrão durante o pulo
    } else {
        if ((frame / 10) % 2 == 0) {
            dino_bitmap = dino1;
        } else {
            dino_bitmap = dino2;
        }
    }

    for (row = 0; row < 20; row++) {
        for (col = 0; col < 16; col++) {
            if (dino_bitmap[row][col] == 1) {
                int x1 = x + col;
                int y1 = y + row;
                video_box(x1, y1, x1, y1, color);
            }
        }
    }
}




void draw_obstacle(int x, int y) {
    const unsigned char cactus[20][16] = {
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,1,1,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
};



    int row, col;
    short color = resample_rgb(db, GREEN);
     for (row = 0; row < 20; row++) {
        for (col = 0; col < 16; col++) {
            if (cactus[row][col] == 1) {
                int x1 = x + col;
                int y1 = y + row;
                video_box(x1, y1, x1, y1, color);
            }
        }
    }
}

void draw_bird(int x, int y, int frame) {
    const unsigned char bird1[20][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
    {0,0,0,1,1,1,0,1,0,1,1,0,0,0,0,0}, // olho: pos [8][8] = 0
    {0,0,1,1,1,1,0,1,1,1,1,1,1,0,0,0}, // bico: pos [9][12] = 1
    {0,0,1,1,1,1,0,1,1,1,1,1,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


     const unsigned char bird2[20][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,0,1,1,1,1,1,1,0,0,0}, // bico: [9][12] = 1
    {0,0,1,1,1,1,0,1,0,1,1,1,0,0,0,0}, // olho: [10][8] = 0
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};



    short color = night_mode ? resample_rgb(db, 0x8410) : resample_rgb(db, WHITE);
    int row, col;
    const unsigned char (*bird)[16];

    if ((frame / 10) % 2 == 0) {
            bird = bird1;
        } else {
            bird = bird2;
        }

    for (row = 0; row < 20; row++) {
        for (col = 0; col < 16; col++) {
            if (bird[row][col] == 1) {
                int x1 = x + col;
                int y1 = y + row;
                video_box(x1, y1, x1, y1, color);
            }
        }
    }
}




void draw_ground(void) {
    short dirt_color = night_mode ? resample_rgb(db, 0x4208) : resample_rgb(db, GRAY);    // marrom escuro à noite
    short grass_color = night_mode ? resample_rgb(db, 0x03E0) : resample_rgb(db, GREEN);  // verde musgo à noite
    short rock_color = resample_rgb(db, WHITE);
    short crack_color = resample_rgb(db, BLACK);
    short bone_color = resample_rgb(db, WHITE); // ossos brancos

    int x, y, height, i;

    // Faixa de terra
    video_box(0, GROUND_Y, STANDARD_X, GROUND_Y + 3, dirt_color);

    // Grama irregular
    for (x = 0; x < STANDARD_X; x += 3) {
        height = random_range(1, 2);
        video_box(x, GROUND_Y - height, x + 1, GROUND_Y - 1, grass_color);
    }

    // Pedras
    for (i = 0; i < 15; i++) {
        x = random_range(0, STANDARD_X - 2);
        y = random_range(GROUND_Y + 1, GROUND_Y + 3);
        video_box(x, y, x + 1, y + 1, rock_color);
    }

    // Rachaduras
    for (i = 0; i < 10; i++) {
        x = random_range(0, STANDARD_X - 5);
        y = GROUND_Y + random_range(2, 3);
        video_box(x, y, x + 3, y, crack_color);
    }

    // Ossos de dinossauro (blocos 4x3)
    for (i = 0; i < 5; i++) {
        x = random_range(0, STANDARD_X - 5);
        y = GROUND_Y + 1;

        // Forma: H com bordas
        video_box(x,     y,     x,     y + 2, bone_color); // lado esquerdo
        video_box(x + 1, y + 1, x + 2, y + 1, bone_color); // meio
        video_box(x + 3, y,     x + 3, y + 2, bone_color); // lado direito
    }
}


void draw_score(int score) {
    char score_display[20] = "Score: ";
    int temp_score = score;
    int i = 7;
    
    if (temp_score == 0) {
        score_display[7] = '0';
        score_display[8] = '\0';
    } else {
        while (temp_score > 0) {
            score_display[i] = (temp_score % 10) + '0';
            temp_score /= 10;
            i++;
        }
        score_display[i] = '\0';
        int start = 7, end = i - 1;
        while (start < end) {
            char temp = score_display[start];
            score_display[start] = score_display[end];
            score_display[end] = temp;
            start++;
            end--;
        }
    }
    video_text(1, 1, score_display);
}

void draw_cloud(int x, int y) {
    short color = resample_rgb(db, WHITE);
    video_box(x + 2, y + 2, x + 6, y + 4, color);   // centro
    video_box(x + 0, y + 3, x + 3, y + 5, color);   // bolha esquerda
    video_box(x + 5, y + 1, x + 9, y + 4, color);   // bolha direita
}

void draw_sun(int x, int y) {
    short sun_color = resample_rgb(db, 0xFFE0);  // amarelo forte
    video_box(x, y, x + 4, y + 4, sun_color);     // centro
    // raios
    video_box(x + 2, y - 2, x + 2, y - 1, sun_color);
    video_box(x + 2, y + 6, x + 2, y + 7, sun_color);
    video_box(x - 2, y + 2, x - 1, y + 2, sun_color);
    video_box(x + 6, y + 2, x + 7, y + 2, sun_color);
}

void draw_moon(int x, int y) {
    short moon_color = resample_rgb(db, WHITE);
    short shadow_color = resample_rgb(db, 0x0010); // tom do céu noturno

    video_box(x, y, x + 5, y + 5, moon_color);         // lua cheia
    video_box(x + 3, y + 1, x + 5, y + 4, shadow_color); // sombra = lua minguante
}

void draw_stars(void) {
    short star_color = resample_rgb(db, WHITE);
    int i;  // <-- declare aqui fora
    for (i = 0; i < 20; i++) {
        int x = random_range(0, STANDARD_X - 2);
        int y = random_range(0, GROUND_Y - 20);
        video_box(x, y, x, y, star_color);
    }
}



int check_collision(int dino_x, int dino_y, int obs_x, int obs_y) {
    if (dino_x < obs_x + OBSTACLE_WIDTH + 3 &&
        dino_x + DINO_WIDTH > obs_x &&
        dino_y < obs_y + OBSTACLE_HEIGHT &&
        dino_y + DINO_HEIGHT > obs_y) {
        return 1;
    }
    return 0;
}

void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++);
}

int random_range(int min, int max) {
    return min + (rand() % (max - min + 1));
}

void video_text(int x, int y, char * text_ptr) {
    int offset;
    volatile char * character_buffer = (char *)FPGA_CHAR_BASE;

    offset = (y << 7) + x;
    while (*(text_ptr)) {
        *(character_buffer + offset) = *(text_ptr);
        ++text_ptr;
        ++offset;
    }
}

void video_box(int x1, int y1, int x2, int y2, short pixel_color) {
    int pixel_buf_ptr = *(int *)PIXEL_BUF_CTRL_BASE;
    int pixel_ptr, row, col;
    int x_factor = 0x1 << (res_offset + col_offset);
    int y_factor = 0x1 << (res_offset);
    x1 = x1 / x_factor;
    x2 = x2 / x_factor;
    y1 = y1 / y_factor;
    y2 = y2 / y_factor;

    for (row = y1; row <= y2; row++)
        for (col = x1; col <= x2; ++col) {
            pixel_ptr = pixel_buf_ptr + (row << (10 - res_offset - col_offset)) + (col << 1);
            *(short *)pixel_ptr = pixel_color;
        }
}

int resample_rgb(int num_bits, int color) {
    return color; // desativa o processamento errado
}


int get_data_bits(int mode) {
    switch (mode) {
    case 0x0:
        return 1;
    case 0x7:
        return 8;
    case 0x11:
        return 8;
    case 0x12:
        return 9;
    case 0x14:
        return 16;
    case 0x17:
        return 24;
    case 0x19:
        return 30;
    case 0x31:
        return 8;
    case 0x32:
        return 12;
    case 0x33:
        return 16;
    case 0x37:
        return 32;
    case 0x39:
        return 40;
    default:
        return 16;
    }
}