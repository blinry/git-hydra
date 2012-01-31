#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;

#include <stdio.h>
#include <stdlib.h>

#include "types.cpp"
#include "vec2f.cpp"
#include "edge.cpp"
#include "node.cpp"
#include "index_entry.cpp"
#include "node_factory.cpp"
#include "graph.cpp"
#include "index.cpp"
#include "force_directed_layout.cpp"
#include "sfml_display.cpp"

int main(int argc, const char *argv[])
{
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
    Index index(node_factory);

    ForceDirectedLayout layout(graph);
    SFMLDisplay display(graph, index);

    int i = 0;
    while(display.open()) {
        if (i%10 == 0) {
            graph.update();
            index.update();
        }
        layout.apply();
        display.draw();
        display.process_events();
        i++;
    }

    return 0;
}
