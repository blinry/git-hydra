/** 
 * A directed, acyclic graph. Attached to a Factory.
 */

class Graph {

    public:

        Graph(NodeFactory& factory) : factory(factory) {
            reseed();
            index_pos = 100;
            history_pos = 100;
        }

        void update() {
            reseed();
            unfold_levels(10);
            visibility_analysis();
        }

        void seed(const NodeID& oid, int depth=999) {
            map<NodeID,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
            }
        }

        Node& lookup(const NodeID& oid) {
            if (oid.type == REF || oid.type == INDEX || oid.type == INDEX_ENTRY) {
                map<NodeID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    nodes[oid] = factory.buildNode(oid);
                } else {
                    // it's there, but maybe it needs an update.
                    Vec2f old_pos = nodes[oid].pos();
                    bool old_selected = nodes[oid].selected();
                    bool visible = nodes[oid].visible();
                    bool needsPosition = nodes[oid].needsPosition;
                    bool hole = nodes[oid].hole;
                    nodes[oid] = factory.buildNode(oid);
                    nodes[oid].pos() = old_pos;
                    if (old_selected)
                        nodes[oid].select();
                    for(int i=0; i<nodes[oid].degree(); i++) {
                        nodes[oid].edge(i).unfold();
                    }
                    if (visible)
                        nodes[oid].show();
                    nodes[oid].needsPosition = needsPosition;
                    nodes[oid].hole = hole;
                }
            } else {
                map<NodeID,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    seed(oid,0);
                }
            }
            return nodes[oid];
        }

        Node &nearest_node(float x, float y) {
            Node *best = 0;
            float best_distance = 99999999; //TODO
            Vec2f pos(x,y);
            for(map<NodeID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                if (!it->second.visible()) continue;
                if (it->second.oid() == NodeID(INDEX,"index")) continue;
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
            for(map<NodeID,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                it->second.hide();
            }
            for(set<NodeID>::iterator it = roots.begin(); it != roots.end(); it++) {
                NodeID ref = *it;
                recursive_set_visible(ref);
            }
        }

        void unfold_levels(int depth) {
            for(set<NodeID>::iterator it = roots.begin(); it != roots.end(); it++) {
                NodeID ref = *it;
                recursive_unfold_levels(ref, depth-1);
            }
        }

        map<NodeID,Node>::iterator nodes_begin() {
            return nodes.begin();
        }

        map<NodeID,Node>::iterator nodes_end() {
            return nodes.end();
        }

        bool empty() {
            return nodes.size() == 0;
        }

        void recursive_unfold_levels(NodeID oid, int depth) {
            Node &n = lookup(oid);
            if (depth<0) {
                for(int i=0; i<n.degree(); i++) {
                    if (n.edge(i).label() != "tree") {
                        lookup(n.edge(i).target()).hole = true;
                    }
                }
                return;
            }
            for(int i=0; i<n.degree(); i++) {
                if (n.edge(i).label() != "tree") {
                    if (n.edge(i).folded()) {
                        n.edge(i).unfold();
                    }
                    recursive_unfold_levels(n.edge(i).target(), depth-1);
                }
            }
        }

        float index_pos;
        float history_pos;
        NodeFactory factory;

    private:

        void recursive_set_visible(NodeID oid) {
            Node &n = lookup(oid);
            n.show();
            for(int j=0; j<n.degree(); j++) {
                Edge &edge = n.edge(j);

                Node &n2 = lookup(edge.target());
                if (!edge.folded()) {
                    if (n2.needsPosition) {
                        float dx = 0.1, dy = 0.1;
                        if (n.oid().type == INDEX_ENTRY && n2.type() == BLOB) {
                            dx = -20;
                        } else if (n.type() == COMMIT) {
                            dy = 20;
                        }
                        n2.pos(n.pos().x + dx, n.pos().y + dy);
                        n2.needsPosition = false;
                    }

                    recursive_set_visible(edge.target());
                } else {
                }
            }
        }

        set<NodeID> roots;
        map<NodeID,Node> nodes;

};
