#include <string>
#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;

#include <cmath>
#include <stdio.h>
#include <stdlib.h>

// Sorry, but I don't think we need ugly header-file-duplication in such a
// small project.

#include "types.cpp"
#include "vec2f.cpp"
#include "edge.cpp"
#include "node.cpp"
#include "node_factory.cpp"
#include "graph.cpp"
#include "force_directed_layout.cpp"
#include "sfml_display.cpp"

int main(int argc, const char *argv[]) {
    srand(time(NULL));

    string git_directory;
    if (argc>1) {
        git_directory = argv[1];
        git_directory += "/.git/";
    } else {
        git_directory = ".git/";
    }

    NodeFactory node_factory(git_directory);
    Graph graph(node_factory);

    ForceDirectedLayout layout(graph);
    graph.update(true);

    //display.draw();
    for(int i=0; i<500; i++) {
        layout.apply();
    }

    SFMLDisplay display(graph);

    while(display.open()) {
        display.draw();
        graph.update();

        layout.apply();

        display.process_events();
    }

    return 0;
}
