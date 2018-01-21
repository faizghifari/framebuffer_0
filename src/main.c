#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define MAX_NAME_SZ 256
// const char* title_text = "Hello World";
struct fb_var_screeninfo var_screen_info;
struct fb_fix_screeninfo fix_screen_info;

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo);
void put_pixel_color(uint8_t* mem, int x, int y, uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo);
void put_image_color(char* file, uint8_t* mem, int x, int y, uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo);
void clear_screen(uint8_t *backbuff);

void init_framebuffer_setting(int* fb_fd, struct fb_fix_screeninfo* fix_screen_info,
                              struct fb_var_screeninfo* var_screen_info) {
    int fd = open("/dev/fb0",O_RDWR);
    if (fb_fd != NULL)
        *fb_fd = fd;
    
    struct fb_var_screeninfo vinfo;
    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.grayscale=0;
    vinfo.bits_per_pixel=32;
    ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    if (var_screen_info != NULL)
        *var_screen_info = vinfo;

    struct fb_fix_screeninfo finfo;
    ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    if (fix_screen_info != NULL)
        *fix_screen_info = finfo;
}

int main() {
    int fb_fd;
    uint8_t *fbp, *backbuff;

    init_framebuffer_setting(&fb_fd, &fix_screen_info, &var_screen_info);
    long screensize = var_screen_info.yres_virtual * fix_screen_info.line_length;
    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t) 0);
    backbuff = (uint8_t*) malloc(var_screen_info.yres_virtual * fix_screen_info.line_length);

    int cur_y = 0;

    // input
    char title_text[MAX_NAME_SZ];
    printf("Enter your name : \n");
    fgets(title_text, MAX_NAME_SZ, stdin);
    if ((strlen(title_text) > 0) && (title_text[strlen (title_text) - 1] == '\n'))
        title_text[strlen (title_text) - 1] = '\0';
    while (1) {
        // clear screen
        clear_screen(backbuff);
        
        // for (x = var_screen_info.xres/8*3; x < var_screen_info.xres/8*5; x++)
        //     put_pixel_color(backbuff, x, cur_y, 0xff, 0xff, 0xff, &var_screen_info, &fix_screen_info);

        int i;
        for (i = 0; i < strlen(title_text); i++) {
            char* filename = (char*) malloc(256);
            sprintf(filename, "data/%c.txt", title_text[i]);
            put_image_color(filename, backbuff, 10+22*i, cur_y, 0xff, 0xff, 0xff, &var_screen_info, &fix_screen_info);
            free(filename);
        }
        
        memcpy(fbp, backbuff, var_screen_info.yres_virtual * fix_screen_info.line_length);

        cur_y = (cur_y + 4) % (var_screen_info.yres - 20);
        usleep(1);
    }

	return 0;
}

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo) {
    return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

void put_pixel_color(uint8_t* mem, int x, int y, uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo) {
    uint32_t value = pixel_color(r, g, b, vinfo);
    long mem_loc = (x + vinfo->xoffset) * (vinfo->bits_per_pixel/8) + (y + vinfo->yoffset) * finfo->line_length;
    *((uint32_t*) (mem + mem_loc)) = value;
}

void put_image_color(char* file, uint8_t* mem, int x, int y, uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo) {
    FILE* fd = fopen(file, "r");
    char* s = (char*) malloc(256);
    while (fscanf(fd, "%s", s) != EOF ) {
        int i;
        for (i = 0; i < strlen(s); i++)
            if (s[i] == '1')
                put_pixel_color(mem, x + i, y, r, g, b, vinfo, finfo);
        y++;    
    }
    free(s);
    fclose(fd);
}

void clear_screen(uint8_t *backbuff){
    int x, y;
    for (x = 0; x < var_screen_info.xres; x++)
        for (y = 0; y < var_screen_info.yres; y++)
            put_pixel_color(backbuff, x, y, 0x0, 0, 0x0, &var_screen_info, &fix_screen_info);
}