#ifndef GOOM_STRUCTS_H
#define GOOM_STRUCTS_H

typedef struct {
    double x, y;
    int rotation;
} player;

typedef struct {
    int w, s, a, d;
} key;

typedef struct {
    int x1, y1, x2, y2;
    int c[3];
} wall;

#endif //GOOM_STRUCTS_H
