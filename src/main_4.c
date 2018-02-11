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

const int MAX_PLANE = 5;
const int PLANE_DURATION = 500;

typedef struct {
    double x, y, t;
} plane_t;

void draw_plane(screen* scr, int x, int y, double t) {
    static const int PLANE_WIDTH = 40;
    static const int PLANE_HEIGHT = 8;

    static const int MACHINE_WIDTH = 10;
    static const int MACHINE_HEIGHT = 10;

    static mat3 translate_first = NULL;
    static mat3 rotate_then = NULL;
    static mat3 scale_then = NULL;
    static mat3 translate_again = NULL;
    static mat3 mat_ab = NULL;
    static mat3 result_matrix = NULL;
    static image pesawat_template_img, machine_template_img;

    if (translate_first == NULL)
        translate_first = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (rotate_then == NULL)
        rotate_then = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (scale_then == NULL)
        scale_then = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (translate_again == NULL)
        translate_again = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (mat_ab == NULL)
        mat_ab = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    if (result_matrix == NULL)
        result_matrix = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);

    if (pesawat_template_img.n_cmd == 0)
        load_image_from_file("data/pesawat_body.txt", &pesawat_template_img);
    if (machine_template_img.n_cmd == 0)
        load_image_from_file("data/baling-baling.txt", &machine_template_img);

    /**
     * translate_again * scale * translate_first * pesawat
     * translate_again * scale * translate_again * rotate * translate_first * machine
     */

    double s = pow(t, 0.3);

    image pesawat_img, machine_left_img, machine_right_img;
    pesawat_img.n_cmd = machine_left_img.n_cmd = 0, machine_right_img.n_cmd = 0;

    copy_image(pesawat_template_img, &pesawat_img);
    copy_image(machine_template_img, &machine_left_img);
    copy_image(machine_template_img, &machine_right_img);

    mat3_scale(s, s, scale_then);
    mat3_translate(x, y, translate_again);
    mul_mat3(translate_again, scale_then, mat_ab);
    
    mat3_translate(-PLANE_WIDTH / 2.0, -PLANE_HEIGHT / 2.0, translate_first);
    mul_mat3(mat_ab, translate_first, result_matrix);
    transform_image(pesawat_img, result_matrix);

    mat3_rotate(t, rotate_then);
    mat3_translate(-MACHINE_WIDTH / 2.0, -MACHINE_HEIGHT / 2.0, translate_first);
    mul_mat3(rotate_then, translate_first, result_matrix);

    mat3_translate(-10, 0, translate_again);
    mul_mat3(mat_ab, translate_again, translate_first);
    mul_mat3(translate_first, result_matrix, rotate_then);
    transform_image(machine_left_img, rotate_then);

    mat3_translate(10, 0, translate_again);
    mul_mat3(mat_ab, translate_again, translate_first);
    mul_mat3(translate_first, result_matrix, rotate_then);
    transform_image(machine_right_img, rotate_then);

    draw_image(scr, 0, 0, 0xffffff, pesawat_img);
    draw_image(scr, 0, 0, 0xffffff, machine_left_img);
    draw_image(scr, 0, 0, 0xffffff, machine_right_img);
    
    free_image(&pesawat_img);
    free_image(&machine_left_img);
    free_image(&machine_right_img);
}

void update_plane(screen* scr, int* n_plane, plane_t* plane_arr) {
    // get screen width and height
    int width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);

    // update time
    for (int i = 0; i < *n_plane; i++)
        plane_arr[i].t++;

    // delete unused plane
    int delete_num = 0;
    for (int i = 0; i < *n_plane; i++)
        if (plane_arr[i].t > PLANE_DURATION)
            delete_num++;
        else
            plane_arr[i - delete_num] = plane_arr[i];
    *n_plane -= delete_num;

    // add random plane
    if (rand() % 100 < 30)
        while (*n_plane < MAX_PLANE) {
            plane_arr[*n_plane].x = rand() % width;
            plane_arr[*n_plane].y = rand() % height;
            plane_arr[*n_plane].t = 0;
            (*n_plane)++;
        }
}

int main(int argc, char** argv) {
    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    plane_t* plane_arr = (plane_t*) malloc(MAX_PLANE * sizeof(plane_t));
    int n_plane = 0;

    int time = 1;
    while (1) {
        clear_screen(&scr);

        int i;
        for (i = 0; i < n_plane; i++)
            draw_plane(&scr, plane_arr[i].x, plane_arr[i].y, plane_arr[i].t);
        update_plane(&scr, &n_plane, plane_arr);
        
        flush_screen(&scr);
        usleep(5000);
        time += 1;
    }

    free_screen(&scr);
    free(plane_arr);

    return 0;
}
