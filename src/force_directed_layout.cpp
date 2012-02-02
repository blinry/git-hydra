/**
 * This class layouts a graph by simulating it's nodes as charged, interconnected particles.
 */

class ForceDirectedLayout {

    public:

        ForceDirectedLayout(Graph& graph) : graph(graph) {
            spring = 20;
            charge = 1000;
            damping = 0.1;
        }

        void apply() {
            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n1 = it->second;
                if (!n1.visible()) continue;

                constrain_to_field(n1);

                for(map<NodeID,Node>::iterator it2 = graph.nodes_begin(); it2 != graph.nodes_end(); it2++) {
                    Node& n2 = it2->second;

                    if (n1.oid() == n2.oid()) continue;
                    if (!n2.visible()) continue;

                    float distance = n1.pos().distance(n2.pos());
                    if (distance == 0) {
                        n2.pos().x += 0.1;
                        continue;
                    }

                    Vec2f connection(n2.pos().x-n1.pos().x, n2.pos().y-n1.pos().y);
                    n1.velocity() += connection.normal()*(attract(n1, n2)+repulse(n1, n2));
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

        float attract(Node& n1, Node& n2) {
            bool connected = false;
            float force = 0;

            float distance = n1.pos().distance(n2.pos());

            for(int k=0; k<n1.degree(); k++) {
                if (n1.edge(k).target() == n2.oid() && !n1.edge(k).folded()) {
                    connected = true;
                    if (n1.type() == COMMIT && n2.type() == COMMIT) {
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
            return force;
        }

        float repulse(Node& n1, Node& n2) {
            float distance = n1.pos().distance(n2.pos());
            return -((n1.mass()*n2.mass())/(distance*distance))*charge;
        }

        void constrain_to_field(Node& n1) {
            if (n1.type() == COMMIT)
                n1.velocity().x -= 0.0001*pow(n1.pos().x,3);
            else if (n1.type() == TAG) {
                ;
            } else {
                n1.velocity().x -= 0.000001*pow((n1.pos().x-500),3);
            }
        }

    private:

        float spring, charge, damping;
        Graph &graph;

};
