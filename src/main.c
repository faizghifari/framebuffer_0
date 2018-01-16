#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo);
void put_pixel_color(uint8_t* mem, int x, int y, uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo);

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
    struct fb_var_screeninfo var_screen_info;
    struct fb_fix_screeninfo fix_screen_info;
    int fb_fd;
    uint8_t *fbp;

    init_framebuffer_setting(&fb_fd, &fix_screen_info, &var_screen_info);
    long screensize = var_screen_info.yres_virtual * fix_screen_info.line_length;
    fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t) 0);

    int cur_y = 0;
    while (1) {
        // clear screen
        int x,y;
        for (x = 0; x < var_screen_info.xres; x++)
            for (y = 0; y < var_screen_info.yres; y++)
                put_pixel_color(fbp, x, y, 0x0, 0, 0x0, &var_screen_info, &fix_screen_info);
        
        for (x = var_screen_info.xres/8*3; x < var_screen_info.xres/8*5; x++)
            put_pixel_color(fbp, x, cur_y, 0xff, 0xff, 0xff, &var_screen_info, &fix_screen_info);
        
        cur_y = (cur_y + 1) % var_screen_info.yres;
        usleep(10000);
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