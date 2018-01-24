#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "screen.h"
#include "screen_util.h"

const int char_width = 15;
#define MAX_TXT_SZ 256
#define UP -4
#define DOWN 4
// const char* title_text = "Hello World";

void put_image_color(char* file, screen* scr, int x, int y, uint8_t r, uint8_t g, uint8_t b);
void clear_screen(screen* scr);
void animate_text(screen* scr, char* title_text, int* cur_y, int opt);

int main() {
    screen scr;
    init_screen(&scr);

    int cur_y = 0;
    int opt = DOWN;
    char c_opt;

    // input
    char title_text[MAX_TXT_SZ];
    printf("Enter text : ");
    fgets(title_text, MAX_TXT_SZ, stdin);
    printf("up/down? (u/D) : ");
    scanf("%c", &c_opt);
    if(c_opt == 'u' || c_opt == 'U') { opt = UP; }
    if ((strlen(title_text) > 0) && (title_text[strlen (title_text) - 1] == '\n'))
        title_text[strlen (title_text) - 1] = '\0';
    while (1) {
        // clear screen
        clear_screen(&scr);
        animate_text(&scr, title_text, &cur_y, opt);
        usleep(1);
    }
    free_screen(&scr);

    return 0;
}

void put_image_color(char* file, screen* scr, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    FILE* fd = fopen(file, "r");
    if (fd < 0) return;
    char* s = (char*) malloc(256);
    while (fscanf(fd, "%s", s) != EOF ) {
        int i;
        for (i = 0; i < strlen(s); i++)
            if (s[i] == '1')
                put_pixel(scr, x + i, y, pixel_color(r,g,b));
        y++;    
    }
    free(s);
    fclose(fd);
}

void clear_screen(screen* scr){
    int x, y, width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);
    
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            put_pixel(scr, x, y, 0);
}

void animate_text(screen* scr, char* title_text, int* cur_y, int opt){
    int i, width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);

    long mid = (width - (strlen(title_text) * char_width)) / 2;
    for (i = 0; i < strlen(title_text); i++) {
        char* filename = (char*) malloc(256);
        sprintf(filename, "data/%c.txt", title_text[i]);
        put_image_color(filename, scr, mid+(char_width+2)*i, *cur_y, 0xff, 0xff, 0xff);
        free(filename);
    }
    flush_screen(scr);

    if(opt == UP)
        *cur_y += height - char_width;
    *cur_y = (*cur_y + opt) % (height - char_width);
}