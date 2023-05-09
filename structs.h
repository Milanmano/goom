#ifndef GOOM_STRUCTS_H
#define GOOM_STRUCTS_H

#define LEFT_SIDE (-1)
#define RIGHT_SIDE 1
#define ON_SPLIT_LINE 0

typedef struct {
    double x, y;
    int rotation;
} player;

typedef struct {
    int w, s, a, d;
} key;

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point start;
    Point end;
    int color[3];
} Line;

typedef struct vec2 {
    float x;
    float y;
} vec2;

vec2 *vec2_init(float _x, float _y) {
    vec2 *v = (vec2 *) malloc(sizeof(vec2));
    v->x = _x;
    v->y = _y;
    return v;
}

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

vec3 *vec3_init(float _x, float _y, float _z) {
    vec3 *v = (vec3 *) malloc(sizeof(vec3));
    v->x = _x;
    v->y = _y;
    v->z = _z;
    return v;
}

typedef struct line {
    unsigned int VBO, VAO;
    float *vertices;
    vec3 *lineColor;
} line;

typedef struct Node {
    Line line;
    struct Node *left;
    struct Node *right;
} Node;

Node *createNode(Line line) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->line = line;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

struct Node *readTree(FILE *file) {
    char marker[4];
    fread(marker, sizeof(char), 4, file);
    if (strncmp(marker, "NULL", 4) == 0) {
        return NULL;
    } else {
        fseek(file, -4, SEEK_CUR);
    }

    Line line;
    fread(&line.start.x, sizeof(double), 1, file);
    fread(&line.start.y, sizeof(double), 1, file);
    fread(&line.end.x, sizeof(double), 1, file);
    fread(&line.end.y, sizeof(double), 1, file);
    fread(&line.color[0], sizeof(int), 1, file);
    fread(&line.color[1], sizeof(int), 1, file);
    fread(&line.color[2], sizeof(int), 1, file);

    struct Node *newNode = createNode(line);

    newNode->left = readTree(file);
    newNode->right = readTree(file);

    return newNode;
}

#endif //GOOM_STRUCTS_H
