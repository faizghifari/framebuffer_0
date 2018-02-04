#include "screen_util.h"
#include <stdlib.h>

void fill(screen* scr);

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r<<16) | (g<<8) | b;
}

void draw_image(screen* scr, int x, int y, int initial_color, image img) {
    int i;
    int cursor_x = x;
    int cursor_y = y;
    int border_color = initial_color; 
    int fill_color = 0xffffff;   
    int width, height;
    get_screen_width(scr, &width);
    get_screen_height(scr, &height);
    screen *tmp_screen = NULL;
    for (i = 0; i < img.n_cmd; i++) {
        command cmd = img.p_cmd[i];
        if (cmd.type == COMMAND_TYPE_PUT_PIXEL)
            put_pixel(scr, x + cmd.x1, y + cmd.y1, initial_color);
        else if (cmd.type == COMMAND_TYPE_LINE_TO) {
            draw_line(scr, cursor_x, cursor_y, x + cmd.x1, y + cmd.y1, border_color);
            draw_line(tmp_screen, cursor_x, cursor_y, x + cmd.x1, y + cmd.y1, 1);
            cursor_x = x + cmd.x1;
            cursor_y = y + cmd.y1;
        } else if (cmd.type == COMMAND_TYPE_MOVE_TO) {
            cursor_x = x + cmd.x1;
            cursor_y = y + cmd.y1;
        } else if (cmd.type == COMMAND_COLOR) {
            border_color = pixel_color(cmd.r, cmd.g, cmd.b);
        } else if (cmd.type == COMMAND_START_FILL) {
            fill_color = pixel_color(cmd.r, cmd.g, cmd.b);
            tmp_screen = (screen*) malloc(sizeof(screen));
            *tmp_screen = create_screen(width,height);
        } else if (cmd.type == COMMAND_END_FILL) {
            fill(tmp_screen);
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    if (get_screen_pixel(tmp_screen,x,y) == 2)
                        put_pixel(scr, x, y, fill_color);

            free_screen(tmp_screen);
            tmp_screen = NULL;
        }
    }
}

void fill(screen* scr) {
    int width, height;
    get_screen_width(scr, &width);
    get_screen_height(scr, &height);

    for (int y = 0; y < height; y++) {
        int last = 0; // 1 up, 2 down
        char flag = 0;
        int last_pixel = 0;
        for (int x = 0; x < width; x++) {
            int status = 0;
            if (get_screen_pixel(scr, x-1, y+1) == 1 || get_screen_pixel(scr, x, y+1) == 1 || get_screen_pixel(scr, x+1, y+1) == 1)
                status |= 2;
            if (get_screen_pixel(scr, x-1, y-1) == 1 || get_screen_pixel(scr, x, y-1) == 1 || get_screen_pixel(scr, x+1, y-1) == 1)
                status |= 1;

            int cur_pixel = get_screen_pixel(scr, x, y);
            if (last_pixel == 1 && cur_pixel == 0 && (status == 3 || (status & last == 0 && status > 0)))
                flag = !flag;
            last_pixel = cur_pixel;

            if (flag)
                put_pixel(scr, x, y, 2);
        }
    }
}

void __draw_line_x(screen* scr, int x1, int y1, int x2, int y2, int color) {
    if (x1 > x2) {
        int t = x1;
        x1 = x2; 
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
    }
    char inc = y2 >= y1 ? 1 : 0;
    int dx = x2 - x1;
    int dy = abs(y2 - y1);
    int e = 0;
    for (;x1 <= x2; x1++) {
        if (e <= 0) {
            put_pixel(scr, x1, y1, color);
            e = e + 2*dy;
        } else {
            put_pixel(scr, x1, inc ? ++y1 : --y1, color);
            e = e + 2*dy - 2*dx;
        }
    }
}

void __draw_line_y(screen* scr, int x1, int y1, int x2, int y2, int color) {
    if (y1 > y2) {
        int t = y1;
        y1 = y2; 
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    char inc = x2 >= x1 ? 1 : 0;
    int dy = y2 - y1;
    int dx = abs(x2 - x1);
    int e = 0;
    for (;y1 <= y2; y1++) {
        if (e <= 0) {
            put_pixel(scr, x1, y1, color);
            e = e + 2*dx;
        } else {
            put_pixel(scr, inc ? ++x1 : --x1, y1, color);
            e = e + 2*dx - 2*dy;
        }
    }
}

void draw_line(screen* scr, int x1, int y1, int x2, int y2, int color) {
    if (scr == NULL)
        return;
    if (abs(x2 - x1) > abs(y2 - y1))
        __draw_line_x(scr, x1, y1, x2, y2, color);
    else
        __draw_line_y(scr, x1, y1, x2, y2, color);
}