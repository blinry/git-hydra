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
                    nodes[oid] = factory.buildNode(oid);
                    nodes[oid].pos() = old_pos;
                }
            } else {
                map<OID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    seed(oid,0);
                }
            }
            return nodes[oid];
        }
        Node& nearest_node(float x, float y) {
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
            return *best;
        }
        void reseed() {
            git_strarray ref_nms;
            git_reference_listall(&ref_nms, factory.repo, GIT_REF_LISTALL);
            ref_names.clear();
            for(int i=0; i<ref_nms.count; i++) {
                ref_names.insert(ref_nms.strings[i]);
            }
            ref_names.insert("HEAD");
            ref_names.insert("index");
        }
        void visibility_analysis() {
            for(map<OID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                it->second.hide();
            }
            for(set<string>::iterator it = ref_names.begin(); it != ref_names.end(); it++) {
                OID ref = *it;
                recursive_set_visible(ref);
            }
        }
        void unfold_levels(int depth) {
            for(set<string>::iterator it = ref_names.begin(); it != ref_names.end(); it++) {
                OID ref = *it;
                recursive_unfold_levels(ref, depth-1);
            }
        }
        map<OID,Node> nodes; // TODO: soll nicht public sein!
        set<string> ref_names;
    private:
        void recursive_unfold_levels(OID oid, int depth) {
            if (depth<0) return;
            Node &n = lookup(oid);
            for(int i=0; i<n.degree(); i++) {
                n.edge(i).unfold();
                recursive_unfold_levels(n.edge(i).target(), depth-1);
            }
        }
        void recursive_set_visible(OID oid) {
            Node &n = lookup(oid);
            n.show();
            for(int j=0; j<n.degree(); j++) {
                Edge &edge = n.edge(j);
                if (!edge.folded())
                    recursive_set_visible(edge.target());
            }
        }
        NodeFactory factory;
};
