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
    string working_directory;
    if (argc>1) {
        working_directory = argv[1];
        working_directory += "/.git/";
    } else {
        working_directory = ".git/";
    }

    NodeFactory node_factory(working_directory);
    Graph graph(node_factory);
    ForceDirectedLayout layout;
    SFMLDisplay display;

    while(display.open()) {
        graph.reseed();
        layout.apply(graph);
        display.draw(graph);
        display.process_events(graph);
    }

    return 0;
}
