#include <string>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// I don't think we need ugly header-duplication in such a small project.

#include "types.cpp"
#include "vec2f.cpp"
#include "edge.cpp"
#include "node.cpp"
#include "node_factory.cpp"
#include "graph.cpp"
#include "sfml_display.cpp"
#include "force_directed_layout.cpp"

int main(int argc, const char *argv[]) {
    srand(time(NULL));

    string start_path;
    if (argc>1) {
        start_path = argv[1];
    } else {
        start_path = ".";
    }

    NodeFactory node_factory(start_path);
    Graph graph(node_factory);

    ForceDirectedLayout layout(graph);
    graph.update(true);

    SFMLDisplay display(graph);
    display.draw();
    for(int i=0; i<500; i++) {
        layout.apply(true);
    }

    graph.scroll_history(-graph.lookup(NodeID(REF, "HEAD")).pos().y+100);

    while(display.open()) {
        display.draw();
        graph.update();

        layout.apply();

        display.process_events();
    }

    return 0;
}
