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
            text.SetCharacterSize(20);
        }

        void draw(Node n) {
            if (!n.visible()) return;
            Color color;
            switch(n.type()) {
                case COMMIT:
                    color = Color(20,155,20);
                    break;
                case TREE:
                    color = Color::Green;
                    break;
                case TAG:
                    color = Color::Blue;
                    break;
                default:
                    color = Color::White;
            }
            Color border_color;
            border_color = color;
            for(int j=0; j<n.degree(); j++) {
                if (n.edge(j).folded()) continue;
                Node n2 = graph.lookup(n.edge(j).target());
                if (n2.visible()) {
                    Color edge_color;
                    if (n2.selected() || n.selected()) {
                        edge_color = Color::White;
                        text.SetString(n.edge(j).label());
                        text.SetPosition((n.pos().x+n2.pos().x)/2,(n.pos().y+n2.pos().y)/2);
                        window.Draw(text);
                    } else {
                        edge_color = Color(50,50,50);
                    }

                    float dir = n.dir_to(n2);

                    float width = 1;
                    if (n.type() == COMMIT && n2.type() == COMMIT)
                        width = 5;

                    Vector2f offset(sin(dir+M_PI/2)*width, cos(dir+M_PI/2)*width);

                    ConvexShape line(4);
                    line.SetFillColor(color);
                    line.SetPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.SetPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset);

                    line.SetPoint(2, line.GetPoint(1) - offset - offset);
                    line.SetPoint(3, line.GetPoint(0) - offset - offset);
                    window.Draw(line);

                    /*

                    line[0].Position = Vector2f(n2.pos().x+sin(dir)*5, n2.pos().y+cos(dir)*5);
                    line[1].Position = Vector2f(n2.pos().x+sin(dir)*5+sin(dir+0.5)*5, n2.pos().y+cos(dir)*5+cos(dir+0.5)*5);
                    window.Draw(line);

                    line[0].Position = Vector2f(n2.pos().x+sin(dir)*5, n2.pos().y+cos(dir)*5);
                    line[1].Position = Vector2f(n2.pos().x+sin(dir)*5+sin(dir-0.5)*5, n2.pos().y+cos(dir)*5+cos(dir-0.5)*5);
                    window.Draw(line);
                    */
                }
            }

            CircleShape circ;
            RectangleShape rect;

            switch (n.type()) {
                case COMMIT:
                    circ = CircleShape(n.width()/2, 64);
                    circ.SetFillColor(color);
                    circ.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                    window.Draw(circ);
                    break;
                default:
                    rect = RectangleShape(Vector2f(n.width(),n.height()));
                    rect.SetFillColor(color);
                    rect.SetOutlineColor(border_color);
                    rect.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                    window.Draw(rect);
                    break;
            }

            text.SetString(n.label());
            text.SetPosition(n.pos().x+5, n.pos().y);
            window.Draw(text);
        }

        void draw() {
            window.Clear();

            // Draw Nodes

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n = it->second;
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
