#ifndef IMAGE_H
#define IMAGE_H

const int COMMAND_TYPE_PUT_PIXEL = 0;
const int COMMAND_TYPE_MOVE_TO = 1;
const int COMMAND_TYPE_LINE_TO = 2;

typedef struct {
    int type;
    int x1, y1, x2, y2;
    int r, g, b;
} command;

typedef struct {
    int n_cmd;
    command* p_cmd;
} image;

void load_image_from_file(char* filename, image* p_img);

#endif