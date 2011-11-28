#include <git2.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Object {
    public:
        Object() {
        }
        Object(git_object *o) : object(o) {
        }
        string typeString() {
            return git_object_type2string(git_object_type(object));
        }
    private:
            git_object *object;
};

class OId {
    public:
        OId(const git_oid *id) : oid(id) {
        }
        string format() {
            char ret[40];
            git_oid_fmt(ret, oid);
            string r(ret,40);
            return r;
        }
    private:
        const git_oid *oid;
};

class Ref {
    public:
        Ref(git_reference *r): ref(r) {
        }
        string name() {
            return git_reference_name(ref);
        }
        OId oid() {
            return OId(git_reference_oid(ref));
        }
    private:
        git_reference *ref;
};

class Blob : Object {
    public:
    private:
};

class Tree : Object {
    public:
    private:
};

class Commit : Object {
    public:
        Commit(git_commit *c) : commit(c) {
        }
        string message() {
            return git_commit_message(commit);
        }
        vector<Commit> parents() {
            vector<Commit> p;
            int numberOfParents = git_commit_parentcount(commit);
            if (numberOfParents == 0) return p;
            for(int i = 0; i<numberOfParents; i++) {
                git_commit *parent;
                git_commit_parent(&parent, commit, i);
                p.push_back(Commit(parent));
            }
            return p;
        }
    private:
        git_commit *commit;
};

class Tag : Object {
    public:
    private:
};

class Repository {
    public:
        Repository(const string &path) {
            repo = 0;
            int ret = git_repository_open(&repo, path.c_str());
            if (!ret)
                ;//TODO
        }
        Commit headCommit() {
            git_reference *ref = 0;
            git_repository_head(&ref, repo);
            return lookupCommit(Ref(ref).oid());
        }
        Object lookup(OId oid) {
            git_object *object = 0;
            git_oid o;
            const char* oi = oid.format().c_str();
            git_oid_fromstr(&o, oi);
            git_object_lookup(&object, repo, &o, GIT_OBJ_ANY);
            return Object(object);
        }
        Commit lookupCommit(OId oid) {
            git_commit *commit = 0;
            git_oid o;
            const char* oi = oid.format().c_str();
            git_oid_fromstr(&o, oi);
            git_commit_lookup(&commit, repo, &o);
            return Commit(commit);
        }
    private:
        git_repository *repo;
};

int main(int argc, const char *argv[]) {
    Repository repo("/home/seb/projects/informaticup/.git/");
    Commit c = repo.headCommit();
    cout << c.message();
    cout << "\n";
    cout << c.parents().at(0).message();
    return 0;
}
