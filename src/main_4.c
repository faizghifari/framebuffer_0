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

int main(int argc, char** argv) {
    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    int time = 1;
    while (1) {
        clear_screen(&scr);
        draw_plane(&scr, width / 2 + time*cos(sqrt(time)), height / 2 + time*sin(sqrt(time)), time);
        flush_screen(&scr);
        usleep(5000);
        time += 1;
    }

    free_screen(&scr);

    return 0;
}
