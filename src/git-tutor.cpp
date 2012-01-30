#include <iostream>
using namespace std;

#include "types.cpp"
#include "vec2f.cpp"
#include "edge.cpp"
#include "node.cpp"
#include "node_factory.cpp"
#include "graph.cpp"
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
    ForceDirectedLayout layout(graph);
    SFMLDisplay display(graph);

    while(display.open()) {
        graph.reseed();
        graph.unfold_levels(5);
        graph.visibility_analysis();
        layout.apply();
        display.draw();
        display.process_events();
    }

    return 0;
}
