class ForceDirectedLayout {
    public:
        ForceDirectedLayout(Graph& graph) : graph(graph) {
            spring=20;
            charge=1000;
            damping=0.1;
        }
        void apply() {
            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n1 = it->second;

                if (!n1.visible()) continue;

                if (n1.type() == COMMIT)
                    n1.velocity().x -= 0.0001*pow(n1.pos().x,3);
                else if (n1.type() == TAG) {
                    ;
                    //if (n1.degree() > 0)
                        //n1.velocity().y -= n1.pos().y-(graph.lookup(n1.edge(0).target()).pos().y-100);
                } else {
                    n1.velocity().x -= 0.000001*pow((n1.pos().x-500),3);
                }

                for(map<NodeID,Node>::iterator it2 = graph.nodes_begin(); it2 != graph.nodes_end(); it2++) {
                    Node& n2 = it2->second;
                    if (!n2.visible()) continue;
                    float distance = n1.pos().distance(n2.pos());
                    if (n1.oid() == n2.oid()) continue;
                    if (distance == 0) continue;

                    float force = 0;
                    bool connected = false;

                    for(int k=0; k<n1.degree(); k++) {
                        if (n1.edge(k).target() == n2.oid() && !n1.edge(k).folded()) {
                            connected = true;
                            if (n1.type() == COMMIT && n2.type() == COMMIT) {
                                /*
                                float distance = n1.pos().y - n2.pos().y;
                                n1.velocity().y -= 0.00000001*exp(distance);
                                */
                                float distance = n1.pos().y+50-(n2.pos().y);
                                float force = 0.02*exp(distance);
                                if (force < 1000)
                                    n1.velocity().y -= force;
                            }
                        }
                    }
                    for(int k=0; k<n2.degree(); k++) {
                        if (n2.edge(k).target() == n1.oid() && !n2.edge(k).folded()) {
                            connected = true;
                            if (n1.type() == COMMIT && n2.type() == COMMIT) {
                                /*
                                float distance = n2.pos().y - n1.pos().y;
                                n1.velocity().y += 0.00000001*exp(distance);
                                */
                                float distance = n2.pos().y+50-(n1.pos().y);
                                float force = 0.02*exp(distance);
                                if (force < 1000)
                                    n1.velocity().y += force;
                            }
                        }
                    }

                    if (connected) {
                        force += (distance-spring)/2.0;
                    }
                    force -= ((n1.mass()*n2.mass())/(distance*distance))*charge;
                    Vec2f connection(n2.pos().x-n1.pos().x, n2.pos().y-n1.pos().y);
                    n1.velocity() += connection.normal()*force;
                }
                float max = 100; //this is an ugly hardcoded value. TODO.
                if (n1.velocity().length()>max)
                    n1.velocity() = n1.velocity().normal()*max;

                n1.velocity() *= damping;
                n1.pos() += n1.velocity();

                if (n1.label() == "index") {
                    n1.pos().x = 500;
                    n1.pos().y = 0;
                }
            }
        }
    private:
        float spring, charge, damping;
        Graph &graph;
};
