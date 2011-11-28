#ifndef TREE_H
#define TREE_H

#include "common.h"
#include "object.h"

class Tree : Object {
    public:
        Tree(git_tree *t);
    private:
        git_tree *tree;
};

#endif
