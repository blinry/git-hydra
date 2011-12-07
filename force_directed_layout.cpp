class ForceDirectedLayout {
    public:
        ForceDirectedLayout() {
            spring=20;
            charge=1000;
            damping=0.3;
        }
        void apply(Graph& graph) {
            for(map<OID,Node>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
                Node& n1 = it->second;
                /*
                   if (n1.type == COMMIT)
                       1.velocity += Vec2f(0,400-n1.pos.y);
                   else
                       n1.velocity += Vec2f(0,500-n1.pos.y);
                   */
                if (!n1.visible) continue;
                for(map<OID,Node>::iterator it2 = graph.nodes.begin(); it2 != graph.nodes.end(); it2++) {
                    Node& n2 = it2->second;
                    if (!n2.visible) continue;
                    float distance = n1.pos.distance(n2.pos);
                    if (n1.oid == n2.oid) continue;
                    if (distance == 0) continue;

                    float force = 0;
                    bool connected = false;

                    for(int k=0; k<n1.children.size(); k++) {
                        if (n1.children.at(k).target == n2.oid && n1.expanded) {
                            connected = true;
                            n1.velocity += Vec2f(0,-10);
                            n2.velocity += Vec2f(0,10);
                        }
                    }
                    for(int k=0; k<n2.children.size(); k++) {
                        if (n2.children.at(k).target == n1.oid && n2.expanded)
                            connected = true;
                    }

                    if (connected) {
                        force += (distance-spring)/2.0;
                    }
                    force -= ((n1.mass()*n2.mass())/(distance*distance))*charge;
                    Vec2f connection(n2.pos.x-n1.pos.x, n2.pos.y-n1.pos.y);
                    n1.velocity += connection.normal()*force;
                }
                float max = 100; //this is an ugly hardcoded value. TODO.
                if (n1.velocity.length()>max)
                    n1.velocity = n1.velocity.normal()*max;

                n1.velocity *= damping;
                n1.pos += n1.velocity;
            }
        }
    private:
        float spring, charge, damping;
};
