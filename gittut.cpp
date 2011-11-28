#include <iostream>
using namespace std;
#include "git.h"

int main(int argc, const char *argv[]) {
    Repository repo("/home/seb/projects/informaticup/.git/");
    Commit c = repo.headCommit();
    cout << c.message();
    cout << "\n";
    cout << c.parents().at(0).message();
    return 0;
}
