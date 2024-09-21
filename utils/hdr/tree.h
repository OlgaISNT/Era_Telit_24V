//
// Created by klokov on 29.03.2022.
//
#ifndef FLASHCONST_TREE_H
#define FLASHCONST_TREE_H

#include <stdint.h>

typedef enum {
    STRING_ELEM,
} vtype_tree_t;

typedef union {
    char *string;
} value_tree_t;

// Структура узел
typedef struct tree_node {
    struct {
        value_tree_t key;
        value_tree_t value;
    } data;                      // Вложенная структура - данные узла. Содержит ключ и значение
    struct tree_node *left;      // Левый потомок
    struct tree_node *right;     // Правый потомок
    struct tree_node *parent;    // Родитель
} tree_node;

// Структура дерево
typedef struct Tree {
    struct {
        vtype_tree_t key;
        vtype_tree_t value;
    } type;                 // Вложенная структура - тип. Содержит тип хранимого ключа и тип хранимого значения
    struct tree_node *root; // Корень
    uint8_t *numberNodes;
} Tree;

extern Tree *newTree(vtype_tree_t key, vtype_tree_t value);

extern void freeTree(Tree *tree);

extern value_tree_t getElementTree(Tree *tree, char *key);

extern void addElementTree(Tree *tree, char *key, char *value);

extern void deleteByKeyTree(Tree *tree, char *key);

extern int containsElementTree(Tree *tree, char *key);

extern void printTree(Tree *tree);

extern void printTreeAsList(Tree *tree);

extern char *toString(tree_node *node);

extern char *toStringSortByAlph(tree_node *node);

#endif //FLASHCONST_TREE_H
