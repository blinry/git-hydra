#include "repository.h"

Repository::Repository(const string &path) {
    repo = 0;
    int ret = git_repository_open(&repo, path.c_str());
    if (!ret)
        ;//TODO
}

Commit Repository::headCommit() {
    git_reference *ref = 0;
    git_repository_head(&ref, repo);
    return lookupCommit(Ref(ref).oid());
}

/*
Object Repository::lookup(OId oid) {
    git_object *object = 0;
    git_oid o;
    const char* oi = oid.format().c_str();
    git_oid_fromstr(&o, oi);
    git_object_lookup(&object, repo, &o, GIT_OBJ_ANY);
    return Object(object);
}
*/

Commit Repository::lookupCommit(OId oid) {
    git_commit *commit = 0;
    git_oid o;
    const char* oi = oid.format().c_str();
    git_oid_fromstr(&o, oi);
    git_commit_lookup(&commit, repo, &o);
    return Commit(commit);
}
