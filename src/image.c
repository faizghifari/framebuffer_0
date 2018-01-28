#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "image.h"

const int COMMAND_TYPE_PUT_PIXEL = 0;
const int COMMAND_TYPE_MOVE_TO = 1;
const int COMMAND_TYPE_LINE_TO = 2;

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