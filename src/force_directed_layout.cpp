/**
 * This class layouts a graph by simulating it's nodes as charged, interconnected particles.
 */

class ForceDirectedLayout {

    public:

        ForceDirectedLayout(Graph& graph) : graph(graph) {
            spring = 0;
            charge = 1000;
            damping = 0.1;
            clock.Restart();
        }

        void apply(bool simulate_time = false) {
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
                    }

                    Vec2f connection(n2.pos().x-n1.pos().x, n2.pos().y-n1.pos().y);

                    if (n1.parent_of_visible(n2.oid())) {
                        turn(n1, n2);
                        n1.velocity() += connection.normal()*attract(n1, n2);
                        n2.velocity() -= connection.normal()*attract(n1, n2);
                    }

                    n1.velocity() += connection.normal()*repulse(n1, n2);
                }
            }

            int elapsed_ms = clock.GetElapsedTime().AsMilliseconds();
            clock.Restart();
            if (simulate_time)
                elapsed_ms = 100;

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n1 = it->second;

                float max = 500; //this is an ugly hardcoded value. TODO.
                if (n1.velocity().length()>max)
                    n1.velocity() = n1.velocity().normal()*max;

                n1.velocity() *= damping;
                n1.pos() += n1.velocity()*((float)(elapsed_ms/50.0));

                if (n1.label() == "index") {
                    n1.pos().x = 1000;
                    n1.pos().y = graph.index_pos;
                    continue;
                } else if (n1.oid().type == INDEX_ENTRY) {
                    n1.pos().x = graph.right_border+10;
                    n1.pos().y = graph.index_pos+atoi(n1.oid().name.c_str())*30;
                }

            }
        }

        float attract(Node& n1, Node& n2) {
            float distance = n1.pos().distance(n2.pos());
            return (distance-spring)/2.0;
        }

        float repulse(Node& n1, Node& n2) {
            float distance = n1.pos().distance(n2.pos());
            return -((n1.mass()*n2.mass())/(distance*distance))*charge;
        }

        void constrain_to_field(Node& n1) {

            // keep snake in the middle of the left area
            if (n1.type() == COMMIT || n1.display_type() == SNAKE_TAIL || n1.display_type() == HEAD) {
                n1.velocity().x -= 0.0001*pow(n1.pos().x-graph.left_border/2,3);
            }
            //if (n1.display_type() == SNAKE_TAIL || n1.hole)
            if (n1.oid() == NodeID(REF, "HEAD"))
                n1.velocity().y -= 0.0001*pow(n1.pos().y-graph.history_pos,3);

            float border = 200;
            float strength = 0.00002;

            // keep blobs and trees in the middle area
            if (n1.type() == BLOB || n1.type() == TREE) {
                if (n1.pos().x < graph.left_border + border)
                    n1.velocity().x += strength*pow(graph.left_border + border - n1.pos().x,3);
                if (n1.pos().x > graph.right_border - border)
                    n1.velocity().x += strength*pow(graph.right_border - border - n1.pos().x,3);
                if (n1.pos().y < border)
                    n1.velocity().y += strength*pow(border - n1.pos().y,3);
                if (n1.pos().y > graph.height-border)
                    n1.velocity().y += strength*pow(graph.height - border - n1.pos().y,3);
                //n1.velocity().x -= 0.000005*pow((n1.pos().x-(graph.left_border+(-graph.left_border+graph.right_border)/2)),3);
                //n1.velocity().y -= 0.000002*pow(n1.pos().y-(graph.left_border+(-graph.left_border+graph.right_border))/2,3);
            }
        }

        void turn(Node& n1, Node& n2) {
            float direction = 0;
            if (n1.display_type() == SNAKE || n1.display_type() == HEAD || n1.display_type() == SNAKE_TAIL) {
                if (n2.display_type() == SNAKE || n2.display_type() == SNAKE_TAIL)
                    direction = M_PI;
                else
                    direction = -M_PI*1/2;
            } else if (n1.display_type() == SIGN)
                direction = -M_PI*0.5;
            else if (n1.oid().type == INDEX_ENTRY)
                direction = M_PI*0.5;
            else if (n1.type() == TREE)
                direction = -M_PI*0.5;
            else
                return;
                //direction = -M_PI*1/2;

            float correction = small_angle(n1.dir_to(n2),direction);
            //float strength = 100/(1+exp(-5*correction))-50;
            //float strength = 300*correction;
            float strength = 50*correction;
            n1.velocity().x += strength*sin(n1.dir_to(n2)+M_PI/2);
            n1.velocity().y += strength*cos(n1.dir_to(n2)+M_PI/2);

            n2.velocity().x -= strength*sin(n1.dir_to(n2)+M_PI/2);
            n2.velocity().y -= strength*cos(n1.dir_to(n2)+M_PI/2);
        }

        float small_angle(float a, float t) {
            if (t-a < -M_PI)
                return 2*M_PI+t-a;
            else if (t-a > M_PI)
                return t-a-2*M_PI;
            else
                return t-a;
        }

    private:

        float spring, charge, damping;
        Graph &graph;
        sf::Clock clock; // sorry, future custom-display-writer. Deadline is approaching.

};
