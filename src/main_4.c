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

void draw_machine(screen* scr, int x, int y, int s, int t) {
    static const int MACHINE_WIDTH = 20;
    static const int MACHINE_HEIGHT = 20;

    static mat3 translate_first = NULL;
    static mat3 transformer_matrix = NULL;
    static mat3 translate_again = NULL;
    static mat3 mat_ab = NULL;
    static mat3 result_matrix = NULL;

    if (translate_first == NULL)
        translate_first = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (transformer_matrix == NULL)
        transformer_matrix = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (translate_again == NULL)
        translate_again = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (mat_ab == NULL)
        mat_ab = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (result_matrix == NULL)
        result_matrix = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);

    image img;
    load_image_from_file("data/baling-baling.txt", &img);
    
    mat3_translate(-MACHINE_WIDTH / 2.0, - MACHINE_HEIGHT / 2.0, translate_first);
    mat3_rotate(t, transformer_matrix);
    mat3_translate(MACHINE_WIDTH / 2.0, MACHINE_HEIGHT / 2.0, translate_again);
    mul_mat3(translate_again, transformer_matrix, mat_ab);
    mul_mat3(mat_ab, translate_first, result_matrix);
    
    transform_image(img, result_matrix);
    
    draw_image(scr, x, y, 0xffffff, img);
    
    free_image(&img);
}

void draw_plane(screen* scr, int x, int y, int s, int t) {

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
        usleep(50);
        time += 1;
    }

    free_screen(&scr);

    return 0;
}
