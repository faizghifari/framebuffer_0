#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <termios.h>

#include "screen.h"
#include "image.h"
#include "screen_util.h"

void clear_screen(screen* scr);

void draw_machine(screen* scr, int x, int y, int s, int t) {
    static const MACHINE_WIDTH = 20;
    static const MACHING_HEIGHT = 20;

    image img;
    load_image_from_file("data/baling-baling.txt", &img);
    
    mat3 translate_first = mat3_translate(-10, -10, NULL);
    mat3 transformer_matrix = mat3_rotate(t, NULL);
    mat3 translate_again = mat3_translate(10,10,NULL);
    mat3 mat_ab = mul_mat3(translate_again, transformer_matrix, NULL);
    mat3 result_matrix = mul_mat3(mat_ab, translate_first, NULL);
    
    transform_image(img, result_matrix);
    
    draw_image(scr, x, y, 0xffffff, img);
    
    free(translate_first);
    free(transformer_matrix);
    free(translate_again);
    free(mat_ab);
    free(result_matrix);
    free_image(&img);
}

int main(int argc, char** argv) {
    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    int time = 0;
    while (1) {
        clear_screen(&scr);
        draw_machine(&scr, 10, 10, 1, time);
        flush_screen(&scr);
        usleep(1);
        time += 1;
    }

    free_screen(&scr);

    return 0;
}

void clear_screen(screen* scr){
    int x, y, width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);
    
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            put_pixel(scr, x, y, 0);
}