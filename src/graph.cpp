#include <map>
#include <set>
class Graph {
    public:
        Graph(NodeFactory& factory) : factory(factory) {
            reseed();
        }
        void expand(const OID& oid) {
            Node& n = lookup(oid);
            n.expanded = true;
            for(vector<Edge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                show(iter->target);
                if (lookup(iter->target).expanded)
                    expand(iter->target);
            }
        }
        void reduce(const OID& oid) {
            Node& n = lookup(oid);
            n.expanded = false;
            for(vector<Edge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                hide(iter->target);
            }
        }
        void show(const OID& oid) {
            Node& n = lookup(oid);
            n.visible = true;
            for(vector<Edge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                if (lookup(iter->target).expanded)
                    expand(iter->target);
            }
        }
        void hide(const OID& oid) {
            Node& n = lookup(oid);
            n.visible = false;
            for(vector<Edge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                hide(iter->target);
            }
        }
        void seed(const OID& oid, int depth=999) {
            map<OID,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
                /*
                for(vector<Edge>::iterator iter = nodes[oid].children.begin(); iter != nodes[oid].children.end(); iter++) {
                    seed(iter->target,depth-1);
                }
                */
                nodes[oid].expanded = true;
            }
        }
        Node& lookup(const OID& oid) {
            size_t found = oid.find("/");
            if (found != string::npos || oid == "HEAD") { // it's a ref!
                map<OID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    nodes[oid] = factory.buildNode(oid);
                } else {
                    // it's there, but maybe it needs an update.
                    Node new_ref = factory.buildNode(oid);
                    nodes[oid].children = new_ref.children;
                }
                return nodes[oid];
            } else {
                map<OID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    seed(oid,0);
                    if (nodes[oid].type != COMMIT)
                        reduce(oid);
                }
                return nodes[oid];
            }
        }
        Node& nearest_node(float x, float y) {
            if (nodes.size() == 0)
                exit(0); //TODO
            Node *best = 0;
            float best_distance = 99999999; //TODO
            Vec2f pos(x,y);
            for(map<OID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                if (!it->second.visible) continue;
                float distance = it->second.pos.distance(pos);
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
        }
        map<OID,Node> nodes; // TODO: soll nicht public sein!
        set<string> ref_names;
    private:
        NodeFactory factory;
};
