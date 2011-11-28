#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "common.h"
#include "commit.h"
#include "object.h"
#include "oid.h"
#include "ref.h"

class Repository {
    public:
        Repository(const string &path);
        Commit headCommit();
        Object lookup(OId oid);
        Commit lookupCommit(OId oid);
    private:
        git_repository *repo;
};

#endif
