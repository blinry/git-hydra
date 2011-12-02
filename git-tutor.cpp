#include <git2.h>
#include <string>
#include <iostream>
using namespace std;

class DirectedEdge {
    public:
        DirectedEdge(string target_oid, string label) : target_oid(target_oid), label(label) { }
        string target_oid;
        string label;
};

#include <vector>
class Node {
    public:
        string label;
        vector<DirectedEdge> children;
};

class NodeFactory {
    public:
        NodeFactory(string repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            //TODO: check.
        }
        Node buildNode(string oid) {
            Node node;

            git_oid id;
            git_oid_fromstr(&id, oid.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            git_otype type = git_object_type(object);

            node.label = git_object_type2string(type);

            switch(type) {
                case 1: //commit
                    git_commit *commit;
                    git_commit_lookup(&commit, repo, &id);

                    int parentcount = git_commit_parentcount(commit);
                    for(int i = 0; i<parentcount; i++) {
                        git_commit *parent;
                        git_commit_parent(&parent, commit, i);
                        const git_oid *target_id = git_commit_id(parent);
                        char oid_str[40];
                        git_oid_fmt(oid_str, target_id);
                        string oid_string(oid_str,40);
                        node.children.push_back(DirectedEdge(oid_string, "parent"));
                    }
                    break;
            }

            return node;
        }
        string get_head_commit_oid() {
            git_reference *ref = 0;
            git_repository_head(&ref, repo);
            const git_oid *oid = git_reference_oid(ref);
            char oid_str[40];
            git_oid_fmt(oid_str, oid);
            string oid_string(oid_str,40);
            return oid_string;
        }
    private:
        git_repository *repo; 
};

#include <map>
class Graph {
    public:
        Graph(NodeFactory factory) : factory(factory) {
            seed(factory.get_head_commit_oid());
        }
        void seed(string oid) {
            map<string,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
                for(vector<DirectedEdge>::iterator iter = nodes[oid].children.begin(); iter != nodes[oid].children.end(); iter++) {
                    seed(iter->target_oid);
                }
            }
        }
        bool contains(string oid) {
        }
    private:
        map<string,Node> nodes;
        NodeFactory factory;
};

int main(int argc, const char *argv[])
{
    NodeFactory node_factory("/home/seb/projects/informaticup/.git/");
    Graph graph(node_factory);
    /*
    ForceDirectedLayout layout();
    SFMLDisplay display();
    while(display.open()) {
        layout.apply(graph);
        display.draw(graph);
        display.process_events();
    }
    */
    return 0;
}
