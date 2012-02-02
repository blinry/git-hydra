#include <SFML/Graphics.hpp>
using namespace sf;

/**
 * Displays the Graph and the Index. Reacts to input events.
 */

class SFMLDisplay {

    public:

        SFMLDisplay(Graph& graph, Index& index) : graph(graph), index(index), window(VideoMode(500,500), "Git-Tutor", Style::Default, ContextSettings(0,0,4,3,0)), view(FloatRect(0,0,window.GetWidth(),window.GetHeight())) {
            window.SetView(view);
            //cout << window.GetSettings().AntialiasingLevel << flush;
            font.LoadFromFile(assets_dir()+"/arial.ttf");
            text.SetFont(font);
            text.SetCharacterSize(10);
        }

        void draw(Node n) {
            switch (n.display_type()) {
                case SNAKE:
                    draw_snake(n);
                    break;
                case HEAD:
                    draw_snake(n, true);
                    break;
                    /*
                case SIGN:
                    draw_sign(n);
                    break;
                    */
                default:
                    draw_rect(n);
                    break;
            }
        }

        void draw_snake(Node n, bool head = false) {
            Color color(20,155,20);
            CircleShape circ(n.width()/2.0, 64);
            circ.SetFillColor(color);
            circ.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));

            if (head) {
                ConvexShape tongue(7);
                tongue.SetFillColor(Color::Red);

                tongue.SetPoint(0, Vector2f(n.pos().x-1, n.pos().y));
                tongue.SetPoint(1, Vector2f(n.pos().x-1, n.pos().y-15));
                tongue.SetPoint(2, Vector2f(n.pos().x-7, n.pos().y-25));
                tongue.SetPoint(3, Vector2f(n.pos().x, n.pos().y-17));
                tongue.SetPoint(4, Vector2f(n.pos().x+7, n.pos().y-25));
                tongue.SetPoint(5, Vector2f(n.pos().x+1, n.pos().y-15));
                tongue.SetPoint(6, Vector2f(n.pos().x+1, n.pos().y));
                window.Draw(tongue);
            }

            window.Draw(circ);

            if (head) {
                double eye_radius = n.width()/8.0;
                CircleShape eye(eye_radius, 32);
                eye.SetFillColor(Color::White);

                CircleShape pupil(eye_radius/2, 16);
                pupil.SetFillColor(Color::Black);

                Vector2f leye(n.pos().x-3.5*eye_radius,n.pos().y-eye_radius);
                Vector2f reye(n.pos().x+1.5*eye_radius,n.pos().y-eye_radius);

                eye.SetPosition(leye);
                pupil.SetPosition(leye);
                window.Draw(eye);
                window.Draw(pupil);

                eye.SetPosition(reye);
                pupil.SetPosition(reye);
                window.Draw(eye);
                window.Draw(pupil);

            }

            text.SetString(n.label());
            text.SetPosition(n.pos().x+10, n.pos().y);
            window.Draw(text);
        }

        void draw_rect(Node n) {
            Color color = Color::Blue;
            RectangleShape rect(Vector2f(n.width(),n.height()));
            rect.SetFillColor(color);
            rect.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
            window.Draw(rect);

            text.SetString(n.label());
            text.SetPosition(n.pos().x+10, n.pos().y);
            window.Draw(text);
        }

        void draw_edges(Node n) {
            for(int j=0; j<n.degree(); j++) {
                if (n.edge(j).folded()) continue;
                Node n2 = graph.lookup(n.edge(j).target());
                if (!n2.visible())
                    continue;
                    Color edge_color = Color::White;

                    float dir = n.dir_to(n2);
                    float width = 3;
                    if ((n.display_type() == SNAKE || n.display_type() == HEAD) && n2.display_type() == SNAKE) {
                        width = 10;
                        edge_color = Color(20,155,20);
                    }

                    Vector2f offset(sin(dir+M_PI/2)*width/2, cos(dir+M_PI/2)*width/2);

                    ConvexShape line(4);
                    line.SetFillColor(edge_color);
                    line.SetPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.SetPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset);

                    line.SetPoint(2, line.GetPoint(1) - offset - offset);
                    line.SetPoint(3, line.GetPoint(0) - offset - offset);
                    window.Draw(line);
            }
        }

        void draw() {
            window.Clear();

            // Draw Edges

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n = it->second;
                if (n.visible())
                    draw_edges(n);
            }

            // Draw Nodes

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n = it->second;
                if (n.visible())
                    draw(n);
            }

            // Draw Node descriptions

            if (!graph.empty()) {
                Vector2f mouse_position = window.ConvertCoords(Mouse::GetPosition().x, Mouse::GetPosition().y);
                Node& n = graph.nearest_node(mouse_position.x, mouse_position.y);

                text.SetString(n.text());
                text.SetPosition(n.pos().x+5, n.pos().y+10);
            }
            window.Draw(text);

            // Draw Index

            int line = 0;
            for(int i=0; i<index.entries().size(); i++) {
                IndexEntry e = index.entries().at(i);
                char s[3000];
                sprintf(s, "%s (%d)", e.path().c_str(), e.stage());
                text.SetString(s);
                text.SetPosition(500, i*20);
                window.Draw(text);
            }

            window.Display();
        }

        void process_events() {
            Event event;
            while(window.PollEvent(event)) {
                if (event.Type == Event::Closed)
                    window.Close();
                if (event.Type == Event::MouseWheelMoved) {
                    view.Zoom(1-event.MouseWheel.Delta*0.1);
                    window.SetView(view);
                }
                if (event.Type == Event::MouseButtonPressed) {
                    if (!graph.empty()) {
                        if (event.MouseButton.Button == 0) {
                            Vector2f click_position = window.ConvertCoords(event.MouseButton.X, event.MouseButton.Y);
                            Node &n = graph.nearest_node(click_position.x, click_position.y);

                            n.toggle_select();
                            if (n.type() == COMMIT) {
                                NodeID tree = n.toggle_tree();
                                graph.recursive_unfold_levels(tree,99999);
                            }
                        } else if (event.MouseButton.Button == 2) {
                            Vector2f click_position = window.ConvertCoords(Mouse::GetPosition().x, Mouse::GetPosition().y);
                            view.SetCenter(click_position);
                            window.SetView(view);
                        }
                    }
                }
                if (event.Type == Event::Resized) {
                    float aspect_ratio = 1.0*event.Size.Width/event.Size.Height;
                    float width = view.GetSize().x*(1.0*event.Size.Width/window.GetWidth());
                    view.SetSize(width, width/aspect_ratio);
                    window.SetView(view);
                }
            }
            if (Mouse::IsButtonPressed(Mouse::Right)) {
                if (!graph.empty()) {
                    Vector2f click_position = window.ConvertCoords(Mouse::GetPosition().x, Mouse::GetPosition().y);
                    Node& n = graph.nearest_node(click_position.x, click_position.y);
                    n.pos(click_position.x, click_position.y);
                }
            }
        }

        bool open() {
            return window.IsOpen();
        }

    private:

        string assets_dir() {
            char path_to_program[200];
            int length = readlink("/proc/self/exe", path_to_program, 200);
            path_to_program[length] = '\0';
            string assets_dir = path_to_program;
            return string(assets_dir, 0, assets_dir.rfind("/"));
        }

        RenderWindow window;
        View view;
        Font font;
        Text text;
        Graph &graph;
        Index &index;

};
