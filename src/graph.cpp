#include <map>
#include <set>
class Graph {
    public:
        Graph(NodeFactory& factory) : factory(factory) {
            reseed();
        }
        void seed(const OID& oid, int depth=999) {
            map<OID,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
            }
        }
        Node& lookup(const OID& oid) {
            size_t found = oid.find("/");
            if (found != string::npos || oid == "HEAD" || oid == "index") { // it's a ref!
                map<OID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    nodes[oid] = factory.buildNode(oid);
                } else {
                    // it's there, but maybe it needs an update.
                    Vec2f old_pos = nodes[oid].pos();
                    bool old_selected = nodes[oid].selected();
                    nodes[oid] = factory.buildNode(oid);
                    nodes[oid].pos() = old_pos;
                    if (old_selected)
                        nodes[oid].select();
                }
            } else {
                map<OID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    seed(oid,0);
                }
            }
            return nodes[oid];
        }
        Node &nearest_node(float x, float y) {
            if (nodes.size() == 0)
                exit(0); //TODO
            Node *best = 0;
            float best_distance = 99999999; //TODO
            Vec2f pos(x,y);
            for(map<OID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                if (!it->second.visible()) continue;
                float distance = it->second.pos().distance(pos);
                if (distance < best_distance) {
                    best_distance = distance;
                    best = &(it->second);
                }
            }
            return (*best);
        }
        void reseed() {
            roots = factory.getRoots();
        }
        void visibility_analysis() {
            for(map<OID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                it->second.hide();
            }
            for(set<string>::iterator it = roots.begin(); it != roots.end(); it++) {
                OID ref = *it;
                recursive_set_visible(ref);
            }
        }
        void unfold_levels(int depth) {
            for(set<string>::iterator it = roots.begin(); it != roots.end(); it++) {
                OID ref = *it;
                recursive_unfold_levels(ref, depth-1);
            }
        }
        map<OID,Node>::iterator nodes_begin() {
            return nodes.begin();
        }
        map<OID,Node>::iterator nodes_end() {
            return nodes.end();
        }
    private:
        NodeFactory factory;
        set<string> roots;
        map<OID,Node> nodes;
        void recursive_unfold_levels(OID oid, int depth) {
            if (depth<0) return;
            Node &n = lookup(oid);
            for(int i=0; i<n.degree(); i++) {
                //cout << n.edge(i).label() << " " << flush;
                if (n.edge(i).label() != "tree") {
                cout << "set!";
                    n.edge(i).unfold();
                    recursive_unfold_levels(n.edge(i).target(), depth-1);
                }
            }
            for(int i=0; i<n.degree(); i++) {
                //cout << n.edge(i).label() << " " << flush;
                    cout << n.edge(i).folded();
                if (n.edge(i).label() != "tree") {
                }
            }
        }
        void recursive_set_visible(OID oid) {
            Node &n = lookup(oid);
            n.show();
            cout << n.label() << " " << n.degree() << flush;
            for(int j=0; j<n.degree(); j++) {
                cout << "?" << flush;
                Edge &edge = n.edge(j);
                if (!edge.folded()) {
                cout << "!" << flush;
                    recursive_set_visible(edge.target());
                }
            }
        }
};
