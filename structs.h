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
    int x1, y1, x2, y2;
    int c[3];
} wall;

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point start;
    Point end;
    int color[3];
} Line;

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

int getSide(Line splitLine, Point point) {
    // Compute the determinant of the 2x2 matrix formed by the vectors (splitLine.end - splitLine.start) and (line.start - splitLine.start)
    double det = (splitLine.end.x - splitLine.start.x) * (point.y - splitLine.start.y) -
                 (point.x - splitLine.start.x) * (splitLine.end.y - splitLine.start.y);

    if (det < 0) {
        return LEFT_SIDE;
    } else if (det > 0) {
        return RIGHT_SIDE;
    } else {
        return ON_SPLIT_LINE;
    }
}

Node *buildBSP(Line *lines, int numLines) {
    if (numLines == 0) {
        return NULL;
    } else {
        // Choose a random line as the splitting line
        int randomIndex = rand() % numLines;
        Line splitLine = lines[randomIndex];

        // Split the lines into left and right sets based on which side of the splitting line they lie
        Line leftLines[100];
        Line rightLines[100];
        int numLeftLines = 0;
        int numRightLines = 0;
        for (int i = 0; i < numLines; i++) {
            if (i == randomIndex) {
                continue; // Skip the splitting line
            }
            int side = getSide(splitLine, lines[i].start);

            if (side == LEFT_SIDE) {
                leftLines[numLeftLines++] = lines[i];
            } else if (side == RIGHT_SIDE) {
                rightLines[numRightLines++] = lines[i];
            } else {
                // If the line is coincident with the splitting line, randomly assign it to one side or the other
                if (rand() % 2 == 0) {
                    leftLines[numLeftLines++] = lines[i];
                } else {
                    rightLines[numRightLines++] = lines[i];
                }
            }
        }

        // Build the left and right subtrees recursively
        Node *leftSubtree = buildBSP(leftLines, numLeftLines);
        Node *rightSubtree = buildBSP(rightLines, numRightLines);

        // Create a new node for the splitting line and return the tree
        Node *newNode = createNode(splitLine);
        newNode->left = leftSubtree;
        newNode->right = rightSubtree;
        return newNode;
    }
}

//TODO: move method
void print_tree(struct Node *node) {
    if (node != NULL) {
        print_tree(node->left);
        printf("(%.0f,%.0f) -> (%.0f,%.0f)\n", node->line.start.x, node->line.start.y, node->line.end.x,
               node->line.end.y);
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


void draw_line(vec2 *start, vec2 *end, vec3 *color) {

    line *l = (line *) malloc(sizeof(line));

    float x1 = start->x;
    float y1 = start->y;
    float x2 = end->x;
    float y2 = end->y;
    float w = 320;
    float h = 240;

    // convert 3d world space position 2d screen space position
    x1 = 2 * x1 / w - 1;
    y1 = 2 * y1 / h - 1;

    x2 = 2 * x2 / w - 1;
    y2 = 2 * y2 / h - 1;

    start->x = x1;
    start->y = y1;
    end->x = x2;
    end->y = y2;

    l->startPoint = start;
    l->endPoint = end;
    l->lineColor = color;

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
