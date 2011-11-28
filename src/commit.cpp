#include "commit.h"

Commit::Commit(git_commit *c) : commit(c) { }

string Commit::message() {
    return git_commit_message(commit);
}

vector<Commit> Commit::parents() {
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

Tree Commit::tree() {
    git_tree *tree;
    git_commit_tree(&tree, commit);
    return Tree(tree);
}
