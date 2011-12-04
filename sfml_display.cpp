#include <SFML/Graphics.hpp>
using namespace sf;
class SFMLDisplay {
    public:
        SFMLDisplay() : window(VideoMode(500,500), "Git-Tutor"), view(FloatRect(0,0,window.GetWidth(),window.GetHeight())) {
            window.SetView(view);
            font.LoadFromFile("arial.ttf");
            text.SetFont(font);
            text.SetCharacterSize(10);
        }
        void draw(Node n, Graph& graph) {
            if (! n.visible) return;
            Color color;
            switch(n.type) {
                case COMMIT:
                    color = Color::Red;
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
            if (n.expanded)
                border_color = color;
            else
                border_color = Color::White;
            if (n.expanded) {
                bool any_visible = false;
                for(int j=0; j<n.children.size(); j++) {
                    Node n2 = graph.lookup(n.children.at(j).target_oid);
                    if (n2.visible) {
                        any_visible = true;
                        Shape line = Shape::Line(n.pos.x, n.pos.y, n2.pos.x, n2.pos.y, 1, Color::White);
                        window.Draw(line);
                        float dir = atan2(n.pos.x-n2.pos.x,n.pos.y-n2.pos.y);
                        window.Draw(Shape::Line(n2.pos.x+sin(dir)*5, n2.pos.y+cos(dir)*5, n2.pos.x+sin(dir)*5+sin(dir+0.5)*5, n2.pos.y+cos(dir)*5+cos(dir+0.5)*5, 1, Color::White));
                        window.Draw(Shape::Line(n2.pos.x+sin(dir)*5, n2.pos.y+cos(dir)*5, n2.pos.x+sin(dir)*5+sin(dir-0.5)*5, n2.pos.y+cos(dir)*5+cos(dir-0.5)*5, 1, Color::White));
                        draw(n2, graph);
                    }
                }
                /*
                   if (!any_visible && !n.children.size() == 0)
                   graph.reduce(n.oid);
                   else
                   graph.expand(n.oid);
                   */
            }
            Shape rect = Shape::Rectangle(n.pos.x-n.width()/2,n.pos.y-n.height()/2,n.width(),n.height(),color,1,border_color);
            window.Draw(rect);
            text.SetString(n.label);
            text.SetPosition(n.pos.x-n.width()/2+10,n.pos.y-n.height()/2);
            window.Draw(text);
        }
        void draw(Graph& graph) {
            window.Clear();
            //draw(graph.lookup(graph.head_oid), graph);
            for(set<string>::iterator it = graph.ref_names.begin(); it != graph.ref_names.end(); it++) {
                string ref = *it;
                draw(graph.lookup(ref), graph);
            }
            window.Display();
        }
        void process_events(Graph& graph) {
            Event event;
            while(window.PollEvent(event)) {
                if (event.Type == Event::Closed)
                    window.Close();
                if (event.Type == Event::MouseWheelMoved) {
                    view.Zoom(1-event.MouseWheel.Delta*0.1);
                    window.SetView(view);
                }
                if (event.Type == Event::MouseButtonPressed) {
                    if (event.MouseButton.Button == 0) {
                        Vector2f click_position = window.ConvertCoords(event.MouseButton.X, event.MouseButton.Y);
                        Node& n = graph.nearest_node(click_position.x, click_position.y);
                        if (n.expanded)
                            graph.reduce(n.oid);
                        else
                            graph.expand(n.oid);
                    }
                }
            }
            if (Mouse::IsButtonPressed(Mouse::Right)) {
                Vector2f click_position = window.ConvertCoords(Mouse::GetPosition().x, Mouse::GetPosition().y);
                Node& n = graph.nearest_node(click_position.x, click_position.y);
                n.pos.x = click_position.x;
                n.pos.y = click_position.y;
            }
        }
        bool open() {
            return window.IsOpened();
        }
    private:
        RenderWindow window;
        View view;
        Font font;
        Text text;
};
