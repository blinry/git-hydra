#ifndef COMMIT_H
#define COMMIT_H

#include "common.h"
#include "object.h"
#include "tree.h"

class Commit : Object {
    public:
        Commit(git_commit *c);
        string message();
        vector<Commit> parents();
        Tree tree();
    private:
        git_commit *commit;
};

#endif
