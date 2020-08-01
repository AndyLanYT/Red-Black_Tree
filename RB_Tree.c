#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int element;
    struct node *left;
    struct node *right;
    struct node *parent;
    enum {RED, BLACK} color;
} node_t;


_Bool isLeftChild(const node_t *const node) {
    return node == node->parent->left;
}

_Bool isRightChild(const node_t *const node) {
    return node == node->parent->right;
}

void R_rotation(node_t **node) {
    node_t *temp = (*node)->left;

    (*node)->left->parent = (*node)->parent;
    (*node)->parent = (*node)->left;
    if (temp->right)
        temp->right->parent = *node;
    
    (*node)->left = temp->right;
    temp->right = *node;
    *node = temp;
}

void L_rotation(node_t **node) {
    node_t *temp = (*node)->right;

    (*node)->right->parent = (*node)->parent;
    (*node)->parent = (*node)->right;
    if (temp->left)
        temp->left->parent = *node;

    (*node)->right = temp->left;
    temp->left = *node;
    *node = temp;
}

void RL_rotation(node_t **node) {
    R_rotation(&(*node)->right);
    L_rotation(node);
}

void LR_rotation(node_t **node) {
    L_rotation(&(*node)->left);
    R_rotation(node);
}

node_t *replacement_node(node_t *node) {
    if (!node->left && !node->right)
        return node;
    else if (!node->left)
        return node->right;
    else if (!node->right)
        return node->left;
    else {
        node = node->right;
        while (node->left)
            node = node->left;
        return node;
    }
}

node_t *sibling_node(const node_t *const node) {
    if (!node)
        return NULL;
    
    return node == node->parent->left ? node->parent->right : node->parent->left;
}

void rebalance_insertion(node_t **node) {
    if ((*node)->left && (*node)->left->color == RED) {
        if ((*node)->left->left && (*node)->left->left->color == RED) {
            R_rotation(node);
            (*node)->color = BLACK;
            (*node)->right->color = RED;
            return;
        } else if ((*node)->left->right && (*node)->left->right->color == RED) {
            LR_rotation(node);
            (*node)->color = BLACK;
            (*node)->right->color = RED;
            return;
        }
    }

    if ((*node)->right && (*node)->right->color == RED) {
        if ((*node)->right->right && (*node)->right->right->color == RED) {
            L_rotation(node);
            (*node)->color = BLACK;
            (*node)->left->color = RED;
            return;
        } else if ((*node)->right->left && (*node)->right->left->color == RED) {
            RL_rotation(node);
            (*node)->color = BLACK;
            (*node)->left->color = RED;
            return;
        }
    }
}

void recolor_insertion(node_t **root, node_t **node) {
    node_t *sibling = sibling_node(*node);

    if (!sibling || sibling->color == BLACK) {
        if (!(*node)->parent->parent)
            rebalance_insertion(root);
        else {
            if (isLeftChild((*node)->parent))
                rebalance_insertion(&(*node)->parent->parent->left);
            else if (isRightChild((*node)->parent))
                rebalance_insertion(&(*node)->parent->parent->right);
        }
    } else if (sibling->color == RED) {
        (*node)->color = BLACK;
        sibling->color = BLACK;

        if ((*node)->parent->parent) {
            (*node)->parent->color = RED;

            if ((*node)->parent->parent->color == RED)
                recolor_insertion(root, &(*node)->parent->parent);
        }
    }
}

void insert(node_t **root, int el) {
    node_t *temp = *root;

    node_t *node = (node_t*) malloc(sizeof(node_t));
    node->element = el;
    node->left = NULL;
    node->right = NULL;
    node->color = RED;

    if (!*root) {
        node->parent = NULL;
        node->color = BLACK;
        *root = node;
        return;
    }

    while (temp) {
        if (el < temp->element) {
            if (!temp->left) {
                node->parent = temp;
                temp->left = node;
                if (temp->color == RED)
                    recolor_insertion(root, &temp);
                return;
            }
            temp = temp->left;
        } else if (el > temp->element) {
            if (!temp->right) {
                node->parent = temp;
                temp->right = node;
                if (temp->color == RED)
                    recolor_insertion(root, &temp);
                return;
            }
            temp = temp->right;
        } else {
            fprintf(stderr, "This element already exists!\n");
            return;
        }
    }
}

void swap_colors(node_t *const a, node_t *const b) {
    int temp = a->color;

    a->color = b->color;
    b->color = temp;
}

void rebalance_deletion(node_t **root, node_t **replacement) {
    node_t *sibling = sibling_node(*replacement);

    switch ((*replacement)->color) {
        case RED:
            return;
        
        case BLACK:
            switch (sibling->color) {
                case RED:
                    swap_colors(sibling, (*replacement)->parent);

                    if (!(*replacement)->parent->parent) {
                        if (isLeftChild(*replacement))
                            L_rotation(root);
                        else if (isRightChild(*replacement))
                            R_rotation(root);
                    } else {
                        if (isLeftChild(*replacement)) {
                            if (isLeftChild((*replacement)->parent))
                                L_rotation(&(*replacement)->parent->parent->left);
                            else if (isRightChild((*replacement)->parent))
                                L_rotation(&(*replacement)->parent->parent->right);
                        } else if (isRightChild(*replacement)) {
                            if (isLeftChild((*replacement)->parent))
                                R_rotation(&(*replacement)->parent->parent->left);
                            else if (isRightChild((*replacement)->parent))
                                R_rotation(&(*replacement)->parent->parent->right);
                        }
                    }

                    rebalance_deletion(root, replacement);
                    break;
                
                case BLACK:
                    if (sibling->left && sibling->left->color == RED) {
                        if (isLeftChild(*replacement)) {
                            swap_colors(sibling, sibling->left);
                            R_rotation(&(*replacement)->parent->right);

                            rebalance_deletion(root, replacement);
                        } else if (isRightChild(*replacement)) {
                            sibling->left->color = BLACK;

                            if (!(*replacement)->parent->parent)
                                R_rotation(root);
                            else {
                                if (isLeftChild((*replacement)->parent))
                                    R_rotation(&(*replacement)->parent->parent->left);
                                else if (isRightChild((*replacement)->parent))
                                    R_rotation(&(*replacement)->parent->parent->right);
                            }
                        }
                    } else if (sibling->right && sibling->right->color == RED) {
                        if (isLeftChild(*replacement)) {
                            sibling->right->color = BLACK;

                            if (!(*replacement)->parent->parent)
                                L_rotation(root);
                            else {
                                if (isLeftChild((*replacement)->parent))
                                    L_rotation(&(*replacement)->parent->parent->left);
                                else if (isRightChild((*replacement)->parent))
                                    L_rotation(&(*replacement)->parent->parent->right);
                            }
                        } else if (isRightChild(*replacement)) {
                            swap_colors(sibling, sibling->right);
                            L_rotation(&(*replacement)->parent->right);

                            rebalance_deletion(root, replacement);
                        }
                    } else {
                        sibling->color = RED;

                        switch ((*replacement)->parent->color) {
                            case RED:
                                (*replacement)->parent->color = BLACK;
                                break;
                            
                            case BLACK:
                                if (!(*replacement)->parent->parent)
                                    return;
                                
                                if (isLeftChild((*replacement)->parent))
                                    rebalance_deletion(root, &(*replacement)->parent->parent->left);
                                else if (isRightChild((*replacement)->parent))
                                    rebalance_deletion(root, &(*replacement)->parent->parent->right);
                                break;
                        }
                    }
                    break;
            }
            break;
    }
}

void recolor_deletion(node_t **root, node_t **replacement) {
    if (!(*replacement)->left && !(*replacement)->right)
        rebalance_deletion(root, replacement);
    else
        swap_colors(*replacement, (*replacement)->right);
}

void deletion(node_t **root, node_t **node) {
    node_t *temp = *node, *replacement = replacement_node(*node);

    (*node)->element = replacement->element;
    recolor_deletion(root, &replacement);

    if (!(*node)->left && !(*node)->right) {
        *node = NULL;
    } else if (!(*node)->left) {
        replacement = *node;
        (*node)->right->parent = (*node)->parent;
        *node = (*node)->right;
        if (!(*node)->parent)
            (*node)->color = BLACK;
    } else if (!(*node)->right) {
        replacement = *node;
        (*node)->left->parent = (*node)->parent;
        *node = (*node)->left;
        if (!(*node)->parent)
            (*node)->color = BLACK;
    } else {
        if (!(*node)->right->left) {
            if (replacement->right)
                replacement->right->parent = (*node)->parent;
            temp->right = replacement->right;
        } else {
            if (replacement->right)
                replacement->right->parent = replacement->parent;
            replacement->parent->left = replacement->right;
        }
    }

    free(replacement);
}

void delete(node_t **root, int el) {
    node_t *temp = *root;

    if (el == (*root)->element) {
        deletion(root, root);
        return;
    }

    while (temp) {
        if (el < temp->element) {
            if (temp->left && el == temp->left->element) {
                deletion(root, &temp->left);
                return;
            }
            temp = temp->left;
        } else if (el > temp->element) {
            if (temp->right && el == temp->right->element) {
                deletion(root, &temp->right);
                return;
            }
            temp = temp->right;
        }
    }

    fprintf(stderr, "There is no such element!\n");
}

void show_tree_helper(const node_t *const root, unsigned int level) {
    if (!root)
        return;

    for (int i = 0; i < level+1; i++)
        printf("   ");

    switch (root->color) {
        case RED:
            printf("\x1b[31m%d\x1b[0m\n", root->element);
            break;

        case BLACK:
            printf("%d\n", root->element);
        break;
    }

    show_tree_helper(root->left, level+1);
    show_tree_helper(root->right, level+1);
}

void show_tree(const node_t *const root) {
    show_tree_helper(root, 0);
}
