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
#include "bullet.h"

const int MAX_PLANE = 5;
const int PLANE_DURATION = 500;
const double GRAVITY = 0.4;
const float PARACHUTE_INIT_VELOCITY = 5;
const float BULLET_VELOCITY = 10;
const float BULLET_LENGTH = 20;
const int MAX_BULLET = 40;
const int EXPLOSION_DURATION = 50;
const float EXPLOSION_VELOCITY = 0.5;

typedef struct {
    double x, y, t;
} plane_t;

typedef struct {
    double x, y, t;
} explosion_t;

typedef struct  {
    double x, y, v, s;
} parachute_t;

void draw_plane(screen* scr, int x, int y, double t) {
    static const int PLANE_WIDTH = 40;
    static const int PLANE_HEIGHT = 8;

    static const int MACHINE_WIDTH = 10;
    static const int MACHINE_HEIGHT = 10;

    static const int TIRE_WIDTH = 2;
    static const int TIRE_HEIGHT = 4;

    static mat3 translate_first = NULL;
    static mat3 rotate_then = NULL;
    static mat3 scale_then = NULL;
    static mat3 translate_again = NULL;
    static mat3 mat_ab = NULL;
    static mat3 result_matrix = NULL;
    static image pesawat_template_img;
    static image machine_template_img;
    static image tire_template_img;

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
    if (tire_template_img.n_cmd == 0)
        load_image_from_file("data/tire.txt", &tire_template_img);

    /**
     * translate_again * scale * translate_first * pesawat
     * translate_again * scale * translate_again * rotate * translate_first * machine
     */

    double s = pow(t, 0.3);

    image pesawat_img, machine_left_img, machine_right_img, tire_left_img, tire_right_img;
    pesawat_img.n_cmd = machine_left_img.n_cmd = 0, machine_right_img.n_cmd = tire_left_img.n_cmd = tire_right_img.n_cmd = 0;

    copy_image(pesawat_template_img, &pesawat_img);
    copy_image(machine_template_img, &machine_left_img);
    copy_image(machine_template_img, &machine_right_img);
    copy_image(tire_template_img, &tire_left_img);
    copy_image(tire_template_img, &tire_right_img);

    // transform plane
    mat3_scale(s, s, scale_then);
    mat3_translate(x, y, translate_again);
    mul_mat3(translate_again, scale_then, mat_ab);
    
    mat3_translate(-PLANE_WIDTH / 2.0, -PLANE_HEIGHT / 2.0, translate_first);
    mul_mat3(mat_ab, translate_first, result_matrix);
    transform_image(pesawat_img, result_matrix);

    // transform machine
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

    // transform tire
    mat3_translate(-TIRE_WIDTH / 2.0 - 5, -TIRE_HEIGHT / 2.0 + 5, translate_first);
    mul_mat3(mat_ab, translate_first, result_matrix);
    transform_image(tire_left_img, result_matrix);

    mat3_translate(-TIRE_WIDTH / 2.0 + 5, -TIRE_HEIGHT / 2.0 + 5, translate_first);
    mul_mat3(mat_ab, translate_first, result_matrix);
    transform_image(tire_right_img, result_matrix);

    draw_image(scr, 0, 0, 0xffffff, pesawat_img);
    draw_image(scr, 0, 0, 0xffffff, machine_left_img);
    draw_image(scr, 0, 0, 0xffffff, machine_right_img);
    draw_image(scr, 0, 0, 0xffffff, tire_left_img);
    draw_image(scr, 0, 0, 0xffffff, tire_right_img);
    
    free_image(&pesawat_img);
    free_image(&machine_left_img);
    free_image(&machine_right_img);
}

void draw_tire(screen* scr, int x, int y){
    static image tire_template_img;
    if (tire_template_img.n_cmd == 0)
        load_image_from_file("data/tire.txt", &tire_template_img);

    image tire_img;
    tire_img.n_cmd = 0;

    copy_image(tire_template_img, &tire_img);

    draw_image(scr, 0, 0, 0xffffff, tire_img);
    free_image(&tire_img);
}

void draw_explosion(screen* scr, int x, int y, double t) {
    static const int collision_detail = 7;
    float angle = 2.0 * acos(-1) / collision_detail;
    for (int j = 0; j < collision_detail; j++) {
        float _y = sin(angle * j);
        float _x = cos(angle * j);
        
        float x0, y0, x1, y1;
        x0 = x + _x * t * 2 * EXPLOSION_VELOCITY;
        y0 = y + _y * t * 2 * EXPLOSION_VELOCITY;
        x1 = x0 + _x * t * EXPLOSION_VELOCITY;
        y1 = y0 + _y * t * EXPLOSION_VELOCITY;

        draw_line(scr, x0, y0, x1, y1, 0xff0000);
    }
}

void draw_parachute(screen* scr, int x, int y, double s) {
    image img_parachute;
    load_image_from_file("data/parasut.txt", &img_parachute);

    mat3 transform_first = mat3_translate(-5, -8, NULL);
    mat3 scale_then = mat3_scale(s, s, NULL);
    mat3 transform_again = mat3_translate(5 + x, 8 + y, NULL);
    mat3 _ = mul_mat3(transform_again, scale_then, NULL);
    mat3 __ = mul_mat3(_, transform_first, NULL);
    transform_image(img_parachute, __);

    draw_image(scr, 0, 0, 0xffffff, img_parachute);
    // draw_line(scr, x, y, x, y + 10, 0xffffff);
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
    if (rand() % 100 < 5)
        if (*n_plane < MAX_PLANE) {
            plane_arr[*n_plane].x = rand() % width;
            plane_arr[*n_plane].y = rand() % height;
            plane_arr[*n_plane].t = 0;
            (*n_plane)++;
        }
}

void update_explosion(int *n_explosion, explosion_t* explosion_arr) {
    for (int i = 0; i < *n_explosion; i++)
        explosion_arr[i].t++;
    
    // delete unused plane
    int delete_num = 0;
    for (int i = 0; i < *n_explosion; i++)
        if (explosion_arr[i].t > EXPLOSION_DURATION)
            delete_num++;
        else
            explosion_arr[i - delete_num] = explosion_arr[i];
    *n_explosion -= delete_num;
}

void update_parachute(screen *scr, int *n_parachute, parachute_t* parachute_arr) {
    // get screen width and height
    int width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);

    for (int i = 0; i < *n_parachute; i++) {
        parachute_arr[i].v -= GRAVITY;
        parachute_arr[i].y -= parachute_arr[i].v;
    }
    
    // delete unused parachute
    int delete_num = 0;
    for (int i = 0; i < *n_parachute; i++)
        if (parachute_arr[i].y > height)
            delete_num++;
        else
            parachute_arr[i - delete_num] = parachute_arr[i];
    *n_parachute -= delete_num;
}

void add_explosion(int* n_explosion, explosion_t* explosion_arr, float x, float y) {
    explosion_arr[*n_explosion].x = x;
    explosion_arr[*n_explosion].y = y;
    explosion_arr[*n_explosion].t = 0;

    (*n_explosion)++;
}

void add_parachute(int* n_parachute, parachute_t* parachute_arr, float x, float y, float t){
    parachute_arr[*n_parachute].x = x;
    parachute_arr[*n_parachute].y = y;
    parachute_arr[*n_parachute].v = PARACHUTE_INIT_VELOCITY;
    parachute_arr[*n_parachute].s = pow(t, 0.3);

    (*n_parachute)++;
}

int main(int argc, char** argv) {
    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    bullet* bullets_array = (bullet*) malloc(MAX_BULLET * sizeof(bullet));
    int n_bullet = 0;

    plane_t* plane_arr = (plane_t*) malloc(MAX_PLANE * sizeof(plane_t));
    int n_plane = 0;
    
    explosion_t* explosion_arr = (explosion_t*) malloc(MAX_PLANE * 5 * sizeof(explosion_t));
    int n_explosion = 0;

    parachute_t* parachute_arr = (parachute_t*) malloc(MAX_PLANE * 5 * sizeof(parachute_t));
    int n_parachute = 0;


    
    while (1) {
        clear_screen(&scr);
        // draw_plane(&scr, width / 2 + time*cos(sqrt(time)), height / 2 + time*sin(sqrt(time)), time);
        // draw_tire(&scr, 50, 50);

        if ((rand() % 100 < 70) && (n_bullet < MAX_BULLET)){
            bullets_array[n_bullet] = add_bullet(width/2, 
                                                 height,
                                                 rand() % (2*width) - width,
                                                 -height,
                                                 8,
                                                 GRAVITY);
            n_bullet += 1;
        }

        int i;
        for (i = 0; i < n_bullet; i++){
            move_bullet(&(bullets_array[i]), BULLET_VELOCITY);
            draw_bullet(&scr, bullets_array[i], BULLET_LENGTH);
        }

        //delete out of bound bullet
        int removed = 0;
        for (i = 0; i < n_bullet; i++)
            if (bullets_array[i].x < 0 || bullets_array[i].x > width || bullets_array[i].y < 0 || bullets_array[i].y > height)
                removed++;
            else
                bullets_array[i - removed] = bullets_array[i];
        n_bullet -= removed;

        for (i = 0; i < n_plane; i++)
            draw_plane(&scr, plane_arr[i].x, plane_arr[i].y, plane_arr[i].t);

        //detect collision
        int j;
        for (i = 0; i < n_bullet; i++) {
            removed = 0;
            for (j = 0; j < n_plane; j++) {
                double offset = 20.0 * pow(plane_arr[j].t, 0.3);
                if (bullets_array[i].x >= plane_arr[j].x - offset  && bullets_array[i].x <= plane_arr[j].x + offset  &&
                    bullets_array[i].y > plane_arr[j].y - offset  && bullets_array[i].y < plane_arr[j].y + offset ) {
                    add_explosion(&n_explosion, explosion_arr, plane_arr[j].x, plane_arr[j].y);
                    add_parachute(&n_parachute, parachute_arr, plane_arr[j].x, plane_arr[j].y, plane_arr[j].t);
                    removed++;
                } else
                    plane_arr[j - removed] = plane_arr[j];
            }
            n_plane -= removed;
        }

        for (i = 0; i < n_explosion; i++)
            draw_explosion(&scr, explosion_arr[i].x, explosion_arr[i].y, explosion_arr[i].t);
        for (i = 0; i < n_parachute; i++)
            draw_parachute(&scr, parachute_arr[i].x, parachute_arr[i].y, parachute_arr[i].s);

        update_plane(&scr, &n_plane, plane_arr);
        update_explosion(&n_explosion, explosion_arr);
        update_parachute(&scr, &n_parachute, parachute_arr);
        
        flush_screen(&scr);
        usleep(5000);
    }

    free_screen(&scr);
    free(plane_arr);
    free(explosion_arr);
    free(parachute_arr);

    return 0;
}
