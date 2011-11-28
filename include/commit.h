#ifndef COMMIT_H
#define COMMIT_H

#include "common.h"
#include "object.h"

class Commit : Object {
    public:
        Commit(git_commit *c);
        string message();
        vector<Commit> parents();
    private:
        git_commit *commit;
};

#endif
