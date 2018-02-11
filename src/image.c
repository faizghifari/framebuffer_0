#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "image.h"

const int COMMAND_TYPE_PUT_PIXEL = 0;
const int COMMAND_TYPE_MOVE_TO = 1;
const int COMMAND_TYPE_LINE_TO = 2;
const int COMMAND_COLOR = 3;
const int COMMAND_START_FILL = 4;
const int COMMAND_END_FILL = 5;

int load_image_from_file(char* filename, image* p_img) {
    if (p_img == NULL)
        return -1;

    if (access(filename, F_OK) == -1)
        return -1;
    
    FILE* fd = fopen(filename, "r");
    if (fd < 0) return -1;
    char* s = (char*) malloc(256);
    
    command *commands = (command*) malloc(sizeof(command) * 500);
    int n_cmd = 0;
    while (fscanf(fd, "%s", s) != EOF) {
        command cmd;
        cmd.type = -1;

        if (strcmp(s, "put_pixel") == 0) {
            cmd.type = COMMAND_TYPE_PUT_PIXEL;
            fscanf(fd, "%d %d", &cmd.x1, &cmd.y1);
        } else if (strcmp(s, "move_to") == 0) {
            cmd.type = COMMAND_TYPE_MOVE_TO;
            fscanf(fd, "%d %d", &cmd.x1, &cmd.y1);
        } else if (strcmp(s, "line_to") == 0) {
            cmd.type = COMMAND_TYPE_LINE_TO;
            fscanf(fd, "%d %d", &cmd.x1, &cmd.y1);
        } else if (strcmp(s, "color") == 0) {
            cmd.type = COMMAND_COLOR;
            fscanf(fd, "%d %d %d", &cmd.r, &cmd.g, &cmd.b);
        } else if (strcmp(s, "start_fill") == 0) {
            cmd.type = COMMAND_START_FILL;
            fscanf(fd, "%d %d %d", &cmd.r, &cmd.g, &cmd.b);
        } else if (strcmp(s, "end_fill") == 0) {
            cmd.type = COMMAND_END_FILL;
        }

        if (cmd.type > -1)
            commands[n_cmd++] = cmd;
    }

    p_img->n_cmd = n_cmd;
    p_img->p_cmd = (command*) malloc(n_cmd * sizeof(command));
    memcpy(p_img->p_cmd, commands, n_cmd * sizeof(command));

    fclose(fd);
    free(commands);

    return n_cmd;
}

void copy_image(image src, image* dst) {
    if (dst == NULL)
        return;
    if (dst->n_cmd == 0 || dst->p_cmd == NULL)
        dst->p_cmd = (command*) malloc(src.n_cmd * sizeof(command));
    dst->n_cmd = src.n_cmd;
    memcpy(dst->p_cmd, src.p_cmd, src.n_cmd * sizeof(command));
}

void free_image(image* p_img) {
    if (p_img != NULL && p_img->n_cmd > 0)
        free(p_img->p_cmd);
}