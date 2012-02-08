/** 
 * A directed, acyclic graph. Attached to a Factory.
 */

class Graph {

    public:

        Graph(NodeFactory& factory) : factory(factory) {
            reseed();
            index_pos = 130;
            history_pos = 100;

            left_border = 1000/3.0;
            right_border = 2000/3.0;
        }

        void update(bool initial = false) {
            reseed();
            unfold_levels(10);
            visibility_analysis();

            if (initial) {
                factory.unfold_new_commits = true;
                factory.show_index = true;
                /*
                if (nodes.size()<50) {
                    factory.all_objects = true;
                    factory.all_refs = true;
                    factory.show_index = true;
                }
                */
            }
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
                if (it->second.oid().type == INDEX_ENTRY) continue;
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
                    if (lookup(n.edge(i).target()).type() != TREE) {
                        n.edge(i).unfold();
                        lookup(n.edge(i).target()).hole = true;
                    }
                }
                return;
            }
            for(int i=0; i<n.degree(); i++) {
                if (factory.unfold_all || n.selected() || !(n.type() == COMMIT && lookup(n.edge(i).target()).type() == TREE)) {
                    n.edge(i).unfold();
                    recursive_unfold_levels(n.edge(i).target(), depth-1);
                }
            }
        }

        void scroll_history(float delta) {
            for(map<NodeID,Node>::iterator it = nodes_begin(); it != nodes_end(); it++) {
                Node& n1 = it->second;
                if (n1.type() == COMMIT || n1.display_type() == SNAKE_TAIL || n1.display_type() == HEAD)
                    n1.pos().y += delta;
                if (n1.display_type() == SIGN && (lookup(n1.edge(0).target()).type() == COMMIT || lookup(n1.edge(0).target()).oid().type == REF))
                    n1.pos().y += delta;
            }
        }

        float index_pos;
        float history_pos;
        NodeFactory& factory;

        float left_border, right_border;
        float height;

    private:

        void recursive_set_visible(NodeID oid) {
            Node &n = lookup(oid);
            n.show();
            if (n.hole) {
                return;
            }
            for(int j=0; j<n.degree(); j++) {
                Edge &edge = n.edge(j);

                if (!edge.folded()) {
                    Node &n2 = lookup(edge.target());
                    if (n2.oid().type == INDEX_ENTRY) {
                        n2.pos().x = right_border+10;
                        n2.pos().y = index_pos+atoi(n2.oid().name.c_str())*30;
                        n2.needsPosition = false;
                    } else {
                        float dx = 0, dy = 0;
                        if (n.oid().type == INDEX_ENTRY && n2.type() == BLOB) {
                            dx = -100;
                        } else if (n.type() == TREE) {
                            dx = 100;
                        } else if (n.type() == COMMIT || n.display_type() == HEAD) {
                            if (n2.type() == TREE)
                                dx = 100;
                            else
                                dy = 100;
                        } else if (n.type() == TAG || n.oid().type == REF) {
                            dx = 100;
                        }

                        if (n2.needsPosition && !n.needsPosition) {
                            n2.pos(n.pos().x + dx, n.pos().y + dy);
                            n2.needsPosition = false;
                        } else if (n.needsPosition && !n2.needsPosition) {
                            n.pos(n2.pos().x - dx, n2.pos().y - dy);
                            n.needsPosition = false;
                        }
                    }
                    recursive_set_visible(edge.target());
                }
            }
        }

        set<NodeID> roots;
        map<NodeID,Node> nodes;
};
