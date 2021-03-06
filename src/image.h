#ifndef IMAGE_H_
#define IMAGE_H_

const int COMMAND_TYPE_PUT_PIXEL;
const int COMMAND_TYPE_MOVE_TO;
const int COMMAND_TYPE_LINE_TO;
const int COMMAND_COLOR;
const int COMMAND_START_FILL;
const int COMMAND_END_FILL;

typedef struct {
    int type;
    int x1, y1, x2, y2;
    int r, g, b;
} command;

typedef struct {
    int n_cmd;
    command* p_cmd;
} image;

int load_image_from_file(char* filename, image* p_img);

void copy_image(image src, image* dst);

void free_image(image* p_img);

#endif