#include <iostream>
using namespace std;
#include "git.h"

int main(int argc, const char *argv[]) {
    Repository repo("/home/seb/projects/informaticup/.git/");
    Commit c = repo.headCommit();
    cout << c.message() << "\n";
    Tree t = c.tree();
    return 0;
}
