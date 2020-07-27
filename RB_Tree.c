#include <stdio.h>
#include <stdlib.h>


struct tree {
    int element;
    struct tree* left;
    struct tree* right;
    struct tree* parent;
    enum {RED, BLACK} color;
};


struct tree* sibling_node(struct tree* node) {
    if (node->parent == NULL) {
        return NULL;
    }

    return node == node->parent->left ? node->parent->right : node->parent->left;
}

struct tree* replacement_node(struct tree* node) {
    if (node->left == NULL && node->right == NULL) {
        return node;
    } else if (node->left == NULL) {
        return node->right;
    } else if (node->right == NULL) {
        return node->left;
    } else {
        node = node->right;
        while (node->left != NULL) {
            node = node->left;
        }
        return node;
    }
}

void R_rotation(struct tree** node) {
    struct tree* temp = (*node)->left;

    (*node)->left->parent = (*node)->parent;
    (*node)->parent = (*node)->left;
    if ((*node)->left->right != NULL) {
        (*node)->left->right->parent = *node;
    }

    (*node)->left = temp->right;
    temp->right = *node;
    *node = temp;
}

void L_rotation(struct tree** node) {
    struct tree* temp = (*node)->right;

    (*node)->right->parent =(*node)->parent;
    (*node)->parent = (*node)->right;
    if ((*node)->right->left != NULL) {
        (*node)->right->left->parent = *node;
    }

    (*node)->right = temp->left;
    temp->left = *node;
    *node = temp;
}

void RL_rotation(struct tree** node) {
    R_rotation(&((*node)->right));
    L_rotation(node);
}

void LR_rotation(struct tree** node) {
    L_rotation(&((*node)->left));
    R_rotation(node);
}

void rebalance_insertion(struct tree** node) {
    if ((*node)->left != NULL && (*node)->left->color == RED) {
        if ((*node)->left->left != NULL && (*node)->left->left->color == RED) {
            R_rotation(node);
            (*node)->color = BLACK;
            (*node)->right->color = RED;
            return;
        } else if ((*node)->left->right != NULL && (*node)->left->right->color == RED) {
            LR_rotation(node);
            (*node)->color = BLACK;
            (*node)->right->color = RED;
            return;
        }
    }

    if ((*node)->right != NULL && (*node)->right->color == RED) {
        if ((*node)->right->right != NULL && (*node)->right->right->color == RED) {
            L_rotation(node);
            (*node)->color = BLACK;
            (*node)->left->color = RED;
            return;
        } else if ((*node)->right->left != NULL && (*node)->right->left->color == RED) {
            RL_rotation(node);
            (*node)->color = BLACK;
            (*node)->left->color = RED;
            return;
        }
    }
}

void recolor_insertion(struct tree** root, struct tree** node) {
    struct tree *temp = *node, *sibling = sibling_node(temp);

    if (sibling == NULL || sibling->color == BLACK) {
        if (temp->parent->parent == NULL) {
            rebalance_insertion(root);
        } else {
            if (temp->parent == temp->parent->parent->left) {
                rebalance_insertion(&(temp->parent->parent->left));
            } else if (temp->parent == temp->parent->parent->right) {
                rebalance_insertion(&(temp->parent->parent->right));
            }
        }
    } else if (sibling->color == RED) {
        temp->color = BLACK;
        sibling->color = BLACK;

        if (temp->parent->parent != NULL) {
            temp->parent->color = RED;

            if (temp->parent->parent->color == RED) {
                recolor_insertion(root, &(temp->parent->parent));
            }
        }
    }
}

void insert(struct tree** root, int el) {
    struct tree* temp = *root;

    struct tree* node = (struct tree*) malloc(sizeof(struct tree));
    node->element = el;
    node->left = NULL;
    node->right = NULL;
    node->color = RED;

    if (*root == NULL) {
        node->color = BLACK;
        node->parent = NULL;
        *root = node;
        return;
    }

    while (temp) {
        if (el < temp->element) {
            if (temp->left == NULL) {
                node->parent = temp;
                temp->left = node;
                if (temp->color == RED) {
                    recolor_insertion(root, &temp);
                }
                return;
            }
            temp = temp->left;
        } else if (el > temp->element) {
            if (temp->right == NULL) {
                node->parent = temp;
                temp->right = node;
                if (temp->color == RED) {
                    recolor_insertion(root, &temp);
                }
                return;
            }
            temp = temp->right;
        } else {
            return;
        }
    }
}

void swap_colors(struct tree* a, struct tree* b) {
    int temp = a->color;

    a->color = b->color;
    b->color = temp;
}

void rebalance_deletion(struct tree** root, struct tree** replacement) {
    struct tree* sibling = sibling_node(*replacement);

    switch ((*replacement)->color) {
        case RED:
            return;

        case BLACK:
            switch (sibling->color) {
                case RED:
                    swap_colors((*replacement)->parent, sibling);

                    if ((*replacement)->parent->parent == NULL) {
                        if (*replacement == (*replacement)->parent->left) {
                            L_rotation(root);
                        } else if (*replacement == (*replacement)->parent->right) {
                            R_rotation(root);
                        }
                    } else {
                        if (*replacement == (*replacement)->parent->left) {
                            if ((*replacement)->parent == (*replacement)->parent->parent->left) {
                                L_rotation(&((*replacement)->parent->parent->left));
                            } else if ((*replacement)->parent == (*replacement)->parent->parent->right) {
                                L_rotation(&((*replacement)->parent->parent->right));
                            }
                        } else if (*replacement == (*replacement)->parent->right) {
                            if ((*replacement)->parent == (*replacement)->parent->parent->left) {
                                R_rotation(&((*replacement)->parent->parent->left));
                            } else if ((*replacement)->parent == (*replacement)->parent->parent->right) {
                                R_rotation(&((*replacement)->parent->parent->right));
                            }
                        }
                    }

                    rebalance_deletion(root, replacement);
                    break;

                case BLACK:
                    if (sibling->left != NULL && sibling->left->color == RED) {
                        if (*replacement == (*replacement)->parent->left) {
                            swap_colors(sibling, sibling->left);
                            R_rotation(&((*replacement)->parent->right));

                            rebalance_deletion(root, replacement);
                        } else if (*replacement == (*replacement)->parent->right) {
                            swap_colors(sibling, (*replacement)->parent);
                            sibling->left->color = BLACK;

                            if ((*replacement)->parent->parent == NULL) {
                                R_rotation(root);
                            } else {
                                if ((*replacement)->parent == (*replacement)->parent->parent->left) {
                                    R_rotation(&((*replacement)->parent->parent->left));
                                } else if ((*replacement)->parent == (*replacement)->parent->parent->right) {
                                    R_rotation(&((*replacement)->parent->parent->right));
                                }
                            }
                        }
                    } else if (sibling->right != NULL && sibling->right->color == RED) {
                        if (*replacement == (*replacement)->parent->left) {
                            swap_colors(sibling, (*replacement)->parent);
                            sibling->right->color = BLACK;

                            if ((*replacement)->parent->parent == NULL) {
                                L_rotation(root);
                            } else {
                                if ((*replacement)->parent == (*replacement)->parent->parent->left) {
                                    L_rotation(&((*replacement)->parent->parent->left));
                                } else if ((*replacement)->parent == (*replacement)->parent->parent->right) {
                                    L_rotation(&((*replacement)->parent->parent->right));
                                }
                            }
                        } else if (*replacement == (*replacement)->parent->right) {
                            swap_colors(sibling, sibling->right);
                            L_rotation(&((*replacement)->parent->left));

                            rebalance_deletion(root, replacement);
                        }
                    } else {
                        sibling->color = RED;

                        if ((*replacement)->parent->color == RED) {
                            (*replacement)->parent->color = BLACK;
                        } else if ((*replacement)->parent->color == BLACK) {
                            if ((*replacement)->parent->parent == NULL) {
                                return;
                            }

                            if ((*replacement)->parent == (*replacement)->parent->parent->left) {
                                rebalance_deletion(root, &((*replacement)->parent->parent->left));
                            } else if ((*replacement)->parent == (*replacement)->parent->parent->right) {
                                rebalance_deletion(root, &((*replacement)->parent->parent->right));
                            }
                        }
                    }
                    break;
            }
            break;
    }
}

void recolor_deletion(struct tree** root, struct tree** replacement) {
    if ((*replacement)->left == NULL && (*replacement)->right == NULL) {
        rebalance_deletion(root, replacement);
    } else {
        swap_colors(*replacement, (*replacement)->right);
    }
}

void deletion(struct tree** root, struct tree** node, int el) {
    struct tree* replacement = replacement_node(*node), *temp = *node;

    (*node)->element = replacement->element;
    recolor_deletion(root, &replacement);

    if ((*node)->left == NULL && (*node)->right == NULL) {
        *node = NULL;
    } else if ((*node)->left == NULL) {
        replacement = *node;
        (*node)->right->parent = (*node)->parent;
        *node = (*node)->right;
    } else if ((*node)->right == NULL) {
        replacement = *node;
        (*node)->left->parent = (*node)->parent;
        *node = (*node)->left;
    } else {
        if ((*node)->right->left == NULL) {
            if (replacement->right != NULL) {
                replacement->right->parent = *node;
            }
            temp->right = replacement->right;
        } else {
            if (replacement->right != NULL) {
                replacement->right->parent = replacement->parent;
            }
            replacement->parent->left = replacement->right;
        }
    }
    free(replacement);
}

void delete(struct tree** root, int el) {
    struct tree* temp = *root;

    if ((*root)->element == el) {
        deletion(root, root, el);
        return;
    } else {
        while (temp) {
            if (el < temp->element) {
                if (temp->left->element == el) {
                    deletion(root, &(temp->left), el);
                    return;
                }
                temp = temp->left;
            } else if (el > temp->element) {
                if (temp->right->element == el) {
                    deletion(root, &(temp->right), el);
                    return;
                }
                temp = temp->right;
            }
        }
    }
}

void show_tree_helper(struct tree* root, int level) {
    if (!root) {
        return;
    }

    for (int i = 0; i < level+1; i++) {
        printf("   ");
    }

    if (root->color == RED) {
        printf("\x1b[31m%d\x1b[0m\n", root->element);
    } else if (root->color == BLACK) {
        printf("%d\n", root->element);
    }

    show_tree_helper(root->left, level+1);
    show_tree_helper(root->right, level+1);
}

void show_tree(struct tree* root) {
    show_tree_helper(root, 0);
}
