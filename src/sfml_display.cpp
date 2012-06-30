#include <SFML/Graphics.hpp>
using namespace sf;

/*
 * Displays the Graph and the Index. Reacts to input events.
 */

class SFMLDisplay {
    public:

        SFMLDisplay(Graph& graph) : graph(graph), window(VideoMode(500,500), "git-hydra", Style::Default, ContextSettings(0,0,4,2,0)), view(FloatRect(0,0,1000,1000)) {
            window.setView(view);

            focused_node = NULL;

            font.loadFromFile(assets_dir()+"/res/DroidSans-Regular.ttf");
            text.setFont(font);
            text.setCharacterSize(15);

            hole.setRadius(25);
            hole.setFillColor(Color::Black);

            snake.setRadius(25);
            snake.setFillColor(color(SNAKE));
            snake.setOutlineColor(Color::White);

            head.setRadius(15);
            head.setFillColor(color(HEAD));

            tongue.setPointCount(7);
            tongue.setFillColor(Color(200,20,20));
            float fact = 2;
            tongue.setPoint(0, Vector2f(-1*fact, 0));
            tongue.setPoint(1, Vector2f(-1*fact, -15*fact));
            tongue.setPoint(2, Vector2f(-7*fact, -25*fact));
            tongue.setPoint(3, Vector2f(0, -17*fact));
            tongue.setPoint(4, Vector2f(7*fact, -25*fact));
            tongue.setPoint(5, Vector2f(1*fact, -15*fact));
            tongue.setPoint(6, Vector2f(1*fact, 0));

            triangle.setPointCount(3);
            triangle.setFillColor(color(BAG));
            snake.setOutlineColor(Color::White);
            triangle.setPoint(0, Vector2f(0, -1.3*20));
            triangle.setPoint(1, Vector2f(+20, 0.5*20));
            triangle.setPoint(2, Vector2f(-20, 0.5*20));

            apple.setRadius(10);
            apple.setFillColor(color(APPLE));

            eye.setRadius(4);
            eye.setFillColor(Color::White);

            pupil.setRadius(2);
            pupil.setFillColor(Color::Black);
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
            if (n.selected()) {
            hole.setPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
            window.draw(hole);
            head.setPosition(Vector2f(n.pos().x-n.width()/2/5*3,n.pos().y-n.height()/2/5*3));
            window.draw(head);
            }
        }

        void draw_bag(Node n) {
            triangle.setPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y));
            if (n.selected()) {
                triangle.setOutlineThickness(1);
            } else {
                triangle.setOutlineThickness(0);
            }
            window.draw(triangle);
        }

        void draw_apple(Node n) {
            apple.setPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
            window.draw(apple);
        }

        void draw_snake(Node n, bool is_head = false) {

            if (is_head) {
                tongue.setPosition(Vector2f(n.pos().x, n.pos().y));
                window.draw(tongue);
                head.setPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                window.draw(head);
            } else {
                if (n.selected()) {
                    snake.setOutlineThickness(1);
                } else {
                    snake.setOutlineThickness(0);
                }
                snake.setPosition(Vector2f(n.pos().x-n.width()/2,n.pos().y-n.height()/2));
                window.draw(snake);
            }


            if (is_head) {
                double eye_radius = n.width()/8.0;
                Vector2f leye(n.pos().x-3.5*eye_radius,n.pos().y-eye_radius-7);
                Vector2f reye(n.pos().x+1.5*eye_radius,n.pos().y-eye_radius-7);

                Vector2f mouse_position = window.convertCoords(Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
                float dir = atan2(n.pos().x-mouse_position.x,n.pos().y-mouse_position.y);

                eye.setPosition(leye);
                pupil.setPosition(leye+Vector2f(1.8,1.8)-2.0f*Vector2f(sin(dir), cos(dir)));
                window.draw(eye);
                window.draw(pupil);

                eye.setPosition(reye);
                pupil.setPosition(reye+Vector2f(1.8,1.8)-2.0f*Vector2f(sin(dir), cos(dir)));
                window.draw(eye);
                window.draw(pupil);

                text.setString(utf8(n.label()));
                text.setPosition(n.pos().x+20, n.pos().y-15);
                window.draw(text);
            }

        }

        void draw_menu_entry(Node n) {
            text.setString(utf8(n.label()));
            text.setPosition(n.pos().x+5, n.pos().y-10);
            text.setColor(Color(20,20,20));
            window.draw(text);
            text.setColor(Color(255,255,255));
        }

        void draw_sign(Node n) {
            text.setString(utf8(n.label()));
            text.setPosition(n.pos().x-text.getGlobalBounds().width, n.pos().y-15);

            float border = 5;
            RectangleShape rect(Vector2f(text.getGlobalBounds().width+2*border, text.getGlobalBounds().height+2.7*border));
            rect.setPosition(text.getPosition()-Vector2f(border,border));
            rect.setFillColor(color(n.display_type()));
            window.draw(rect);

            window.draw(text);
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
                    line.setFillColor(edge_color);
                    line.setPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.setPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset);

                    line.setPoint(2, line.getPoint(1) - offset - offset);
                    line.setPoint(3, line.getPoint(0) - offset - offset);
                    window.draw(line);

                    ConvexShape arrow(3);
                    arrow.setFillColor(Color::White);
                    arrow.setPoint(0, Vector2f(n.pos().x, n.pos().y) - 0.8f*norm2);
                    arrow.setPoint(1, arrow.getPoint(0) + 0.8f*norm + 0.3f*offset);
                    arrow.setPoint(2, arrow.getPoint(0) + 0.8f*norm - 0.3f*offset);
                    window.draw(arrow);
                } else if (n2.display_type() == SNAKE_TAIL) {
                    ConvexShape tail(3);
                    tail.setFillColor(edge_color);
                    tail.setPoint(0, Vector2f(n.pos().x, n.pos().y)+1.0f*offset);
                    tail.setPoint(1, Vector2f(n2.pos().x, n2.pos().y));

                    tail.setPoint(2, tail.getPoint(0) - 2.0f*offset);
                    window.draw(tail);
                } else {
                    ConvexShape line(4);
                    line.setFillColor(edge_color);
                    line.setPoint(0, Vector2f(n.pos().x, n.pos().y)+offset);
                    line.setPoint(1, Vector2f(n2.pos().x, n2.pos().y)+offset+norm+norm2);

                    line.setPoint(2, line.getPoint(1) - offset - offset);
                    line.setPoint(3, line.getPoint(0) - offset - offset);
                    window.draw(line);

                    ConvexShape arrow(3);
                    arrow.setFillColor(edge_color);
                    arrow.setPoint(0, Vector2f(n2.pos().x, n2.pos().y) + norm);
                    arrow.setPoint(1, arrow.getPoint(0) + norm2 + offset + offset + offset);
                    arrow.setPoint(2, arrow.getPoint(0) + norm2 - offset - offset - offset);
                    window.draw(arrow);
                }

            }
        }

        void draw_background() {
            RectangleShape rect(Vector2f(graph.left_border,view.getSize().y));

            rect.setFillColor(Color(30,60,30));
            rect.setPosition(Vector2f(0,0));
            window.draw(rect);

            if (graph.factory.show_index) {
                rect.setFillColor(color(MENU_ENTRY));
                rect.setSize(Vector2f(1000-graph.right_border,view.getSize().y));
                rect.setPosition(Vector2f(graph.right_border,0));
                window.draw(rect);

                text.setPosition(graph.right_border+20,50);
                text.setString("Index");
                text.setCharacterSize(30);
                text.setColor(Color(20,20,20));
                window.draw(text);
                text.setCharacterSize(15);
                text.setColor(Color(255,255,255));
            }

        }

        void draw_menu() {
            RectangleShape rect;
            rect.setSize(Vector2f(1000,30));
            rect.setPosition(Vector2f(0,0));
            rect.setFillColor(Color(0,0,0,200));
            window.draw(rect);

            text.setPosition(5,5);
            string desc;
            desc += "o: ";
            if (graph.factory.all_objects)
                desc += "Only show reachable objects";
            else
                desc += "Show all objects";
            desc += "  -  r: ";
            if (graph.factory.all_refs)
                desc += "Show only HEAD";
            else
                desc += "Show all refs";
            desc += "  -  i: ";
            if (graph.factory.show_index)
                desc += "Hide index";
            else
                desc += "Show index";
            text.setString(utf8(desc));
            window.draw(text);
        }

        void draw() {
            window.clear();

            graph.left_border = 1000/3.0;
            if (graph.factory.show_index)
                graph.right_border = 2000/3.0;
            else
                graph.right_border = 1000;
            graph.height = view.getSize().y;
            graph.factory.height = view.getSize().y;

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
                    text.setString(utf8(focused_node->edge(j).label()));
                    text.setPosition((focused_node->pos().x+n2.pos().x)/2-text.getGlobalBounds().width/2,(focused_node->pos().y+n2.pos().y)/2-text.getGlobalBounds().height/2);
                    window.draw(text);
                }
            }

            draw_menu();

            // Draw mouse-over description
            if (!graph.empty()) {
                Vector2f mouse_position = window.convertCoords(Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
                Node& n = graph.nearest_node(mouse_position.x, mouse_position.y);

                if (!Mouse::isButtonPressed(Mouse::Right))
                    if (sqrt(pow(n.pos().x-mouse_position.x,2) + pow(n.pos().y-mouse_position.y,2))<100)
                        focused_node = &n;
                    else
                        focused_node = NULL;

                if (focused_node) {
                    if (focused_node->display_type() != HEAD && focused_node->oid().type != INDEX_ENTRY && focused_node->oid().type != REF && focused_node->display_type() != SNAKE_TAIL) {

                        if (focused_node->type() != TREE) {
                            text.setString(utf8(focused_node->text()));
                            text.setPosition(focused_node->pos().x+15, focused_node->pos().y+15);

                            float border = 5;

                            RectangleShape bg(Vector2f(text.getGlobalBounds().width+2*border, text.getGlobalBounds().height+2*border));
                            bg.setPosition(text.getPosition()-Vector2f(border,border));
                            bg.setFillColor(Color(10,10,10,200));
                            window.draw(bg);

                            window.draw(text);
                        }

                        text.setString(utf8(focused_node->label()));
                        text.setPosition(focused_node->pos().x+15, focused_node->pos().y-10);
                        text.setColor(Color::Yellow);

                        RectangleShape ellipse(Vector2f(text.getGlobalBounds().width+10, text.getGlobalBounds().height+5));
                        ellipse.setPosition(focused_node->pos().x+15-5, focused_node->pos().y-5-5);
                        ellipse.setFillColor(Color(10,10,10,200));

                        window.draw(ellipse);
                        window.draw(text);
                        text.setColor(Color::White);
                    }
                }
            }

            window.display();
        }

        void process_events() {
            Event event;
            while(window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Escape)
                        window.close();
                    if (event.key.code == Keyboard::O)
                        graph.factory.all_objects = !graph.factory.all_objects;
                    if (event.key.code == Keyboard::R)
                        graph.factory.all_refs = !graph.factory.all_refs;
                    if (event.key.code == Keyboard::I)
                        graph.factory.show_index = !graph.factory.show_index;
                }
                if (event.type == Event::MouseWheelMoved) {
                    Vector2f click_position = window.convertCoords(Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
                    if (click_position.x < graph.left_border) {
                        graph.history_pos += event.mouseWheel.delta*20;
                        graph.scroll_history(event.mouseWheel.delta*20);
                    }
                    if (click_position.x > graph.right_border) {
                        graph.index_pos += event.mouseWheel.delta*20;
                        graph.factory.index_pos += event.mouseWheel.delta*20;
                    }
                }
                if (event.type == Event::MouseButtonPressed) {
                    if (!graph.empty()) {
                        Vector2f click_position = window.convertCoords(Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
                        if (event.mouseButton.button == 0) {
                            Node &n = graph.nearest_node(click_position.x, click_position.y);

                            if (n.type() == COMMIT || n.type() == TREE) {
                                if (n.hole) {
                                    n.continue_unfolding = true;
                                } else {
                                    n.toggle_select();
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
                }
                if (event.type == Event::Resized) {
                    float aspect_ratio = 1.0*event.size.width/event.size.height;
                    view.reset(FloatRect(0, 0, 1000, 1000/aspect_ratio));
                    window.setView(view);
                }
            }
            if (Mouse::isButtonPressed(Mouse::Right)) {
                if (focused_node) {
                    Vector2f click_position = window.convertCoords(Vector2i(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
                    focused_node->pos(click_position.x, click_position.y);
                }
            }
        }

        bool open() {
            return window.isOpen();
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
            sf::Utf8::toUtf32(utf8.c_str(), utf8.c_str() + utf8.size(), std::back_inserter(utf32));
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
