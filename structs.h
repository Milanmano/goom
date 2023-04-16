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

typedef struct node {
    double start_x;
    double start_y;
    double end_x;
    double end_y;
    struct node *left;
    struct node *right;
} node;

typedef struct tree {
    struct node *root;
} tree;

struct tree *createTree() {
    struct tree *tree = (struct tree *) malloc(sizeof(struct tree));
    tree->root = NULL;
    return tree;
}


//TODO: move method
void insert_node(struct node **node, double start_x, double start_y, double end_x, double end_y) {
    if (*node == NULL) {
        *node = (struct node *) malloc(sizeof(struct node));
        (*node)->left = NULL;
        (*node)->right = NULL;
        (*node)->start_x = start_x;
        (*node)->start_y = start_y;
        (*node)->end_x = end_x;
        (*node)->end_y = end_y;
    } else {
        double slope = ((*node)->end_y - (*node)->start_y) / ((*node)->end_x - (*node)->start_x);
        double intercept = (*node)->end_y - slope * (*node)->end_x;

        if (start_y - slope * start_x > intercept) {
            insert_node(&(*node)->left, start_x, start_y, end_x, end_y);
        } else {
            insert_node(&(*node)->right, start_x, start_y, end_x, end_y);
        }
    }
}

//TODO: move method
void print_tree(struct node *node) {
    if (node != NULL) {
        print_tree(node->left);
        printf("(%.0f,%.0f) -> (%.0f,%.0f)\n", node->start_x, node->start_y, node->end_x, node->end_y);
        print_tree(node->right);
    }
}

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
    vec2 *startPoint;
    vec2 *endPoint;
    vec3 *lineColor;
} line;


void draw_line(vec2 *start, vec2 *end) {

    line *l = (line *) malloc(sizeof(line));

    l->startPoint = start;
    l->endPoint = end;
    l->lineColor = vec3_init(1.0f, 1.0f, 1.0f);

    // setting vertex data
    l->vertices = (float *) (malloc(sizeof(float) * 6));
    l->vertices[0] = start->x;
    l->vertices[1] = start->y;
    l->vertices[2] = end->x;
    l->vertices[3] = end->y;

    glGenVertexArrays(1, &l->VAO);
    glGenBuffers(1, &l->VBO);
    glBindVertexArray(l->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, l->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4,
                 &(l->vertices[0]),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLint shaderProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);

    GLint colorloc = glGetUniformLocation(shaderProgram, "color");
    glUniform3fv(colorloc, 1, &(l->lineColor->x));
    glBindVertexArray(l->VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

#endif //GOOM_STRUCTS_H
