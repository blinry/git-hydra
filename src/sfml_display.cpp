#include <SFML/Graphics.hpp>
using namespace sf;

/**
 * Displays the Graph and the Index. Reacts to input events.
 */

class SFMLDisplay {

    public:

        SFMLDisplay(Graph& graph) : graph(graph), window(VideoMode(500,500), "git-hydra", Style::Default, ContextSettings(0,0,4,3,0)), view(FloatRect(0,0,1000,1000)) {
            window.SetView(view);
            //cout << window.GetSettings().AntialiasingLevel << flush;
            font.LoadFromFile(assets_dir()+"/res/DroidSans-Regular.ttf");
            text.SetFont(font);
            text.SetCharacterSize(15);

            hole.SetRadius(25);
            hole.SetFillColor(Color::Black);

            snake.SetRadius(25);
            snake.SetFillColor(color(SNAKE));
            snake.SetOutlineColor(Color::White);

            head.SetRadius(15);
            head.SetFillColor(color(HEAD));

            tongue.SetPointCount(7);
            tongue.SetFillColor(Color(200,20,20));
            float fact = 2;
            tongue.SetPoint(0, Vector2f(-1*fact, 0));
            tongue.SetPoint(1, Vector2f(-1*fact, -15*fact));
            tongue.SetPoint(2, Vector2f(-7*fact, -25*fact));
            tongue.SetPoint(3, Vector2f(0, -17*fact));
            tongue.SetPoint(4, Vector2f(7*fact, -25*fact));
            tongue.SetPoint(5, Vector2f(1*fact, -15*fact));
            tongue.SetPoint(6, Vector2f(1*fact, 0));

            triangle.SetPointCount(3);
            triangle.SetFillColor(color(BAG));
            snake.SetOutlineColor(Color::White);
            triangle.SetPoint(0, Vector2f(0, -1.3*20));
            triangle.SetPoint(1, Vector2f(+20, 0.5*20));
            triangle.SetPoint(2, Vector2f(-20, 0.5*20));

            apple.SetRadius(10);
            apple.SetFillColor(color(APPLE));

            eye.SetRadius(4);
            eye.SetFillColor(Color::White);

            pupil.SetRadius(2);
            pupil.SetFillColor(Color::Black);
        }

        void draw(Node n) {
            if (n.label() == "index")
                return;
            if (n.hole) {
                draw_hole(n);
                return;
            }
            switch (n.display_type()) {
                case SNAKE:
                    draw_snake(n);
                    break;
                case HEAD:
                    draw_snake(n, true);
                    break;
                case BAG:
                    draw_bag(n);
                    break;
                case SNAKE_TAIL:
                    break;
                case MENU_ENTRY:
                    draw_menu_entry(n);
                    break;
                case SIGN:
                    draw_sign(n);
                    break;
                case APPLE:
                default:
                    draw_apple(n);
                    break;
            }
        }

        void draw_hole(Node n) {
            hole.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
            window.Draw(hole);
            head.SetPosition(Vector2f(n.pos().x-n.width()/2/5*3,n.pos().y-n.height()/2/5*3));
            window.Draw(head);
        }

        void draw_bag(Node n) {
            triangle.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y));
            if (n.selected()) {
                triangle.SetOutlineThickness(1);
            } else {
                triangle.SetOutlineThickness(0);
            }
            window.Draw(triangle);
        }

        void draw_apple(Node n) {
            apple.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
            window.Draw(apple);
        }

        void draw_snake(Node n, bool is_head = false) {

            if (is_head) {
                tongue.SetPosition(Vector2f(n.pos().x, n.pos().y));
                window.Draw(tongue);
                head.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                window.Draw(head);
            } else {
                if (n.selected()) {
                    snake.SetOutlineThickness(1);
                } else {
                    snake.SetOutlineThickness(0);
                }
                snake.SetPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                window.Draw(snake);
            }


            if (is_head) {
                double eye_radius = n.width()/8.0;
                Vector2f leye(n.pos().x-3.5*eye_radius,n.pos().y-eye_radius-7);
                Vector2f reye(n.pos().x+1.5*eye_radius,n.pos().y-eye_radius-7);

                Vector2f mouse_position = window.ConvertCoords(Mouse::GetPosition(window).x, Mouse::GetPosition(window).y);
                float dir = atan2(n.pos().x-mouse_position.x,n.pos().y-mouse_position.y);

                eye.SetPosition(leye);
                pupil.SetPosition(leye+Vector2f(1.8,1.8)-2.0f*Vector2f(sin(dir), cos(dir)));
                window.Draw(eye);
                window.Draw(pupil);

                eye.SetPosition(reye);
                pupil.SetPosition(reye+Vector2f(1.8,1.8)-2.0f*Vector2f(sin(dir), cos(dir)));
                window.Draw(eye);
                window.Draw(pupil);

                text.SetString(utf8(n.label()));
                text.SetPosition(n.pos().x+20, n.pos().y-15);
                window.Draw(text);
            }

        }

        void draw_menu_entry(Node n) {
            text.SetString(utf8(n.label()));
            text.SetPosition(n.pos().x+5, n.pos().y-10);
            text.SetColor(Color(20,20,20));
            window.Draw(text);
            text.SetColor(Color(255,255,255));
        }

        void draw_sign(Node n) {
            text.SetString(utf8(n.label()));
            text.SetPosition(n.pos().x-text.GetGlobalBounds().Width, n.pos().y-15);

            float border = 5;
            RectangleShape rect(Vector2f(text.GetGlobalBounds().Width+2*border, text.GetGlobalBounds().Height+2.7*border));
            rect.SetPosition(text.GetPosition()-Vector2f(border,border));
            rect.SetFillColor(color(n.display_type()));
            window.Draw(rect);

            window.Draw(text);
        }

        void draw_edges(Node n) {
            for(int j=0; j<n.degree(); j++) {
                if (n.edge(j).folded()) continue;
                Node n2 = graph.lookup(n.edge(j).target());
                if (!n2.visible())
                    continue;

                if (n.oid().type == INDEX)
                    continue;

                Color edge_color = color(n.display_type());

                float dir = n.dir_to(n2);
                float width = (n2.width()+n.width())/6;
                if ((n.display_type() == SNAKE || n.display_type() == HEAD) && (n2.display_type() == SNAKE || n2.display_type() == SNAKE_TAIL )) {
                    width = 30;
                }

                Vector2f offset(sin(dir+M_PI/2)*width/2, cos(dir+M_PI/2)*width/2);
                Vector2f norm(sin(dir)*n2.width()/2, cos(dir)*n2.width()/2);
                Vector2f norm2(sin(dir)*width*2, cos(dir)*width*2);

                if ((n.display_type() == SNAKE || n.display_type() == HEAD) && n2.display_type() == SNAKE) {
                    ConvexShape line(4);
                    line.SetFillColor(edge_color);
                    line.SetPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.SetPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset);

                    line.SetPoint(2, line.GetPoint(1) - offset - offset);
                    line.SetPoint(3, line.GetPoint(0) - offset - offset);
                    window.Draw(line);

                    ConvexShape arrow(3);
                    arrow.SetFillColor(Color::White);
                    arrow.SetPoint(0, Vector2f(n.pos().x, n.pos().y) - 0.8f*norm2);
                    arrow.SetPoint(1, arrow.GetPoint(0) + 0.8f*norm + 0.3f*offset);
                    arrow.SetPoint(2, arrow.GetPoint(0) + 0.8f*norm - 0.3f*offset);
                    window.Draw(arrow);
                } else if (n2.display_type() == SNAKE_TAIL) {
                    ConvexShape tail(3);
                    tail.SetFillColor(edge_color);
                    tail.SetPoint(0, Vector2f(n.pos().x, n.pos().y)+1.0f*offset);
                    tail.SetPoint(1, Vector2f(n2.pos().x, n2.pos().y));

                    tail.SetPoint(2, tail.GetPoint(0) - 2.0f*offset);
                    window.Draw(tail);
                } else {
                    ConvexShape line(4);
                    line.SetFillColor(edge_color);
                    line.SetPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.SetPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset+norm+norm2);

                    line.SetPoint(2, line.GetPoint(1) - offset - offset);
                    line.SetPoint(3, line.GetPoint(0) - offset - offset);
                    window.Draw(line);

                    ConvexShape arrow(3);
                    arrow.SetFillColor(edge_color);
                    arrow.SetPoint(0, Vector2f(n2.pos().x, n2.pos().y) + norm);
                    arrow.SetPoint(1, arrow.GetPoint(0) + norm2 + offset + offset + offset);
                    arrow.SetPoint(2, arrow.GetPoint(0) + norm2 - offset - offset - offset);
                    window.Draw(arrow);
                }

            }
        }

        void draw_background() {
            RectangleShape rect(Vector2f(graph.left_border,view.GetSize().y));

            rect.SetFillColor(Color(30,60,30));
            rect.SetPosition(Vector2f(0,0));
            window.Draw(rect);

            if (graph.factory.show_index) {
                rect.SetFillColor(color(MENU_ENTRY));
                rect.SetSize(Vector2f(1000-graph.right_border,view.GetSize().y));
                rect.SetPosition(Vector2f(graph.right_border,0));
                window.Draw(rect);

                text.SetPosition(graph.right_border+20,50);
                text.SetString("Index");
                text.SetCharacterSize(30);
                text.SetColor(Color(20,20,20));
                window.Draw(text);
                text.SetCharacterSize(15);
                text.SetColor(Color(255,255,255));
            }

        }

        void draw_menu() {
            RectangleShape rect;
            rect.SetSize(Vector2f(1000,30));
            rect.SetPosition(Vector2f(0,0));
            rect.SetFillColor(Color::Black);
            window.Draw(rect);

            text.SetPosition(5,5);
            string desc;
            desc += "O: ";
            if (graph.factory.all_objects)
                desc += "Nur erreichbare Objekte anzeigen";
            else
                desc += "Alle Objekte anzeigen";
            desc += " - R: ";
            if (graph.factory.all_refs)
                desc += "Nur HEAD anzeigen";
            else
                desc += "Alle Refs anzeigen";
            desc += " - I: ";
            if (graph.factory.show_index)
                desc += "Index ausblenden";
            else
                desc += "Index anzeigen";
            text.SetString(desc);
            window.Draw(text);
        }

        void draw() {
            window.Clear();

            graph.left_border = 1000/3.0;
            if (graph.factory.show_index)
                graph.right_border = 2000/3.0;
            else
                graph.right_border = 1000;
            graph.height = view.GetSize().y;
            graph.factory.height = view.GetSize().y;

            draw_background();

            // Draw edges

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n = it->second;
                if (n.visible())
                    draw_edges(n);
            }


            // Draw nodes

            for(map<NodeID,Node>::iterator it = graph.nodes_begin(); it != graph.nodes_end(); it++) {
                Node& n = it->second;
                if (n.visible())
                    draw(n);
            }

            // Draw edge labels

            if (focused_node) {
                for(int j=0; j<focused_node->degree(); j++) {
                    if (focused_node->edge(j).folded()) continue;
                    Node n2 = graph.lookup(focused_node->edge(j).target());
                    text.SetString(focused_node->edge(j).label());
                    text.SetPosition((focused_node->pos().x+n2.pos().x)/2-text.GetGlobalBounds().Width/2,(focused_node->pos().y+n2.pos().y)/2-text.GetGlobalBounds().Height/2);
                    window.Draw(text);
                }
            }

            draw_menu();

            // Draw mouse-over description

            if (!graph.empty()) {
                Vector2f mouse_position = window.ConvertCoords(Mouse::GetPosition(window).x, Mouse::GetPosition(window).y);
                Node& n = graph.nearest_node(mouse_position.x, mouse_position.y);

                if (!Mouse::IsButtonPressed(Mouse::Right))
                    if (sqrt(pow(n.pos().x-mouse_position.x,2) + pow(n.pos().y-mouse_position.y,2))<100)
                        focused_node = &n;
                    else
                        focused_node = NULL;

                if (focused_node) {
                    if (focused_node->display_type() != HEAD && focused_node->oid().type != INDEX_ENTRY && focused_node->oid().type != REF && focused_node->display_type() != SNAKE_TAIL) {
                        text.SetString(utf8(focused_node->label()));
                        text.SetPosition(focused_node->pos().x+15, focused_node->pos().y-10);
                        text.SetColor(Color::Red);
                        window.Draw(text);

                        if (focused_node->type() != TREE) {
                            text.SetString(utf8(focused_node->text()));
                            text.SetPosition(focused_node->pos().x+15, focused_node->pos().y+15);
                            text.SetColor(Color::White);

                            float border = 5;

                            RectangleShape bg(Vector2f(text.GetGlobalBounds().Width+2*border, text.GetGlobalBounds().Height+2*border));
                            bg.SetPosition(text.GetPosition()-Vector2f(border,border));
                            bg.SetFillColor(Color(10,10,10,200));
                            window.Draw(bg);

                            window.Draw(text);
                        }
                    }
                }
            }

            window.Display();
        }

        void process_events() {
            Event event;
            while(window.PollEvent(event)) {
                if (event.Type == Event::Closed)
                    window.Close();
                if (event.Type == Event::KeyPressed) {
                    if (event.Key.Code == Keyboard::Escape)
                        window.Close();
                    if (event.Key.Code == Keyboard::O)
                        graph.factory.all_objects = !graph.factory.all_objects;
                    if (event.Key.Code == Keyboard::R)
                        graph.factory.all_refs = !graph.factory.all_refs;
                    if (event.Key.Code == Keyboard::I)
                        graph.factory.show_index = !graph.factory.show_index;
                }
                if (event.Type == Event::MouseWheelMoved) {
                    Vector2f click_position = window.ConvertCoords(Mouse::GetPosition(window).x, Mouse::GetPosition(window).y);
                    if (click_position.x < graph.left_border) {
                        graph.history_pos += event.MouseWheel.Delta*20;
                        graph.scroll_history(event.MouseWheel.Delta*20);
                    }
                    if (click_position.x > graph.right_border) {
                        graph.index_pos += event.MouseWheel.Delta*20;
                        graph.factory.index_pos += event.MouseWheel.Delta*20;
                    }
                }
                if (event.Type == Event::MouseButtonPressed) {
                    if (!graph.empty()) {
                        Vector2f click_position = window.ConvertCoords(Mouse::GetPosition(window).x, Mouse::GetPosition(window).y);
                        if (event.MouseButton.Button == 0) {
                            Node &n = graph.nearest_node(click_position.x, click_position.y);

                            n.toggle_select();
                            if (n.type() == COMMIT || n.type() == TREE) {
                                // toggle tree
                                for(int i=0; i<n.degree(); i++) {
                                    if (graph.lookup(n.edge(i).target()).type() == TREE || graph.lookup(n.edge(i).target()).type() == BLOB) {
                                        n.edge(i).toggle_fold();
                                    }
                                }
                            }
                        }
                    }
                }
                if (event.Type == Event::Resized) {
                    float aspect_ratio = 1.0*event.Size.Width/event.Size.Height;
                    view.Reset(FloatRect(0, 0, 1000, 1000/aspect_ratio));
                    window.SetView(view);
                }
            }
            if (Mouse::IsButtonPressed(Mouse::Right)) {
                if (focused_node) {
                    Vector2f click_position = window.ConvertCoords(Mouse::GetPosition(window).x, Mouse::GetPosition(window).y);
                    focused_node->pos(click_position.x, click_position.y);
                }
            }
        }

        bool open() {
            return window.IsOpen();
        }

    private:

        Color color(NodeDisplayType t) {
            switch (t) {
                case SNAKE:
                case SNAKE_TAIL:
                case HEAD:
                    return Color(20,155,20);
                case BAG:
                    return Color(0,55,0);
                case APPLE:
                    return Color(100,100,100);
                case SIGN:
                    return Color(50,50,200);
                case MENU_ENTRY:
                    return Color(240,230,190);
                default:
                    return Color(255,0,255);
            }
        }

        string assets_dir() {
            char path_to_program[200];
            int length = readlink("/proc/self/exe", path_to_program, 200);
            path_to_program[length] = '\0';
            string assets_dir = path_to_program;
            return string(assets_dir, 0, assets_dir.rfind("/"));
        }

        sf::String utf8(string utf8) {
            std::basic_string<Uint32> utf32;
            sf::Utf8::ToUtf32(utf8.c_str(), utf8.c_str() + utf8.size(), std::back_inserter(utf32));
            return sf::String(utf32);
        }

        RenderWindow window;
        View view;
        Font font;
        Text text;
        Graph &graph;

        CircleShape hole;
        CircleShape snake;
        CircleShape head;
        ConvexShape tongue;
        ConvexShape triangle;
        CircleShape apple;
        CircleShape eye;
        CircleShape pupil;
        RectangleShape rect;

        Node* focused_node;

};
