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

void print_tree(struct node *node) {
    if (node != NULL) {
        print_tree(node->left);
        printf("(%.0f,%.0f) -> (%.0f,%.0f)\n", node->start_x, node->start_y, node->end_x, node->end_y);
        print_tree(node->right);
    }
}

#endif //GOOM_STRUCTS_H
