#include <iostream>
using namespace std;
#include "git.h"

int main(int argc, const char *argv[]) {
    Repository repo("/home/seb/projects/informaticup/.git/");
    Commit c = repo.headCommit();
    cout << c.message() << "\n";
    Commit cp = c.parents().at(0);
    cout << cp.message() << "\n";
    Tree t = c.tree();

    Node c_node(c.oid());
    Node cp_node(cp.oid());
    Graph g();
    g.insert(c_node);
    g.insert(cp_node);
    g.connect(c_node, cp_node, "parent");

    return 0;
}
