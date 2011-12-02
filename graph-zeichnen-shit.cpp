#include <SFML/Graphics.hpp>
using namespace sf;

#include <boost/graph/adjacency_list.hpp>
using namespace boost;

#include "git.h"

#include "cmath"
#include "vector"
#include "iostream"
#include "sigc++-2.0/sigc++/reference_wrapper.h"
using namespace std;

class Vec2f {
    public:
        Vec2f(float x, float y) : x(x), y(y) { }
        Vec2f() : x(0), y(0) { }
        float distance(Vec2f other) {
            return (other-(*this)).length();
        }
        float length() {
            sqrt(x*x+y*y);
        }
        Vec2f normal() {
            return (*this)*(1/length());
        }
        Vec2f operator+(const Vec2f &other) {
            return Vec2f(x+other.x, y+other.y);
        }
        void operator+=(const Vec2f &other) {
            x += other.x;
            y += other.y;
        }
        Vec2f operator-(const Vec2f &other) {
            return Vec2f(x-other.x, y-other.y);
        }
        void operator-=(const Vec2f &other) {
            x -= other.x;
            y -= other.y;
        }
        Vec2f operator *(float factor) {
            return Vec2f(x*factor, y*factor);
        }
        void operator *=(float factor) {
            x *= factor;
            y *= factor;
        }
        float x, y;
};

class DirectedEdge;

class Node {
    public:
        Node() {
            static int last_id = 0;
            id = last_id;
            last_id++;
        }
        float width() {
            return text.size()*10;
        }
        float height() {
            return 10;
        }
        float mass() {
            return text.size();
        }
        int id;
        vector<DirectedEdge> edges;
        Vec2f pos;
        Vec2f velocity;
        string text;
};

class DirectedEdge {
    public:
        DirectedEdge(Node &t) : target(t) {
        }
        DirectedEdge& operator=(const DirectedEdge& edge) {
            target = edge.target;
            return *this;
        }
        Node &target;
};

class Graph {
    private:
        float spring, charge, damping;
    public:
        Graph() {
            spring=40;
            charge=20;
            damping=0.5;
        }

        vector<Node> nodes;

        void simulate() {
            for(int i=0; i<nodes.size(); i++) {
                Node &n1 = nodes.at(i);
                for(int j=0; j<nodes.size(); j++) {
                    Node &n2 = nodes.at(j);
                    // Equality?
                    float distance = n1.pos.distance(n2.pos);
                    if (distance == 0) continue;

                    float force = 0;
                    bool connected = false;

                    for(int k=0; k<n1.edges.size(); k++) {
                        if (n1.edges.at(k).target.id == n2.id)
                            connected = true;
                    }
                    for(int k=0; k<n2.edges.size(); k++) {
                        if (n2.edges.at(k).target.id == n1.id)
                            connected = true;
                    }

                    if (connected) {
                        force += (distance-spring)/2.0;
                    } else {
                        force += -((n1.mass()*n2.mass())/(distance*distance))*charge;
                    }
                    Vec2f connection(n2.pos.x-n1.pos.x, n2.pos.y-n1.pos.y);
                    n1.velocity += connection.normal()*force;
                }
                if (n1.velocity.length()>10) //this is an ugly hardcoded value. TODO.
                    n1.velocity = n1.velocity.normal()*10;
                n1.pos += n1.velocity;
                n1.velocity *= damping;
            }
        }
};

void add(Commit &c, Graph &g, Node &child) {
    Node n = Node();
    n.text = "Foo";
    n.pos.x = rand()%40-20+400;
    n.pos.y = rand()%40-20+300;

    Node &nn = n;
    Node &cc = child;

    DirectedEdge e(cc);
    nn.edges.push_back(e);

    g.nodes.push_back(nn);

    for(int i=0; i<c.parents().size(); i++) {
        cout << "!" << flush;
        Commit p = c.parents().at(i);
        //add(p, g, n);
    }
}

int main(int argc, const char *argv[])
{
    srand(time(NULL));

    RenderWindow window(VideoMode::GetDesktopMode(), "Gittut");

    Repository repo("/home/seb/projects/informaticup/.git/");
    Commit c = repo.headCommit();

    Graph graph;

    Node n = Node();
    n.text = "master";
    n.pos.x = rand()%40-20+400;
    n.pos.y = rand()%40-20+300;
    graph.nodes.push_back(n);
    add(c, graph, n);

    /*
    int numberOfNodes = 200;
    for(int i=0; i<numberOfNodes; i++) {
        Node n = Node();
        n.text = "Foo";
        for(int j=0; j<rand()%10; j++) {
            n.text.append("o");
        }
        n.pos.x = rand()%40-20+window.GetWidth()/2;
        n.pos.y = rand()%40-20+window.GetHeight()/2;
        graph.nodes.push_back(n);
    }
    for(int i=1; i<numberOfNodes; i++) {
        Node &n1 = graph.nodes.at(i);
        int j = rand()%i;
        Node &n2 = graph.nodes.at(j);
        DirectedEdge e(n1);
        n2.edges.push_back(e);
    }
    */

    Font font;
    if (!font.LoadFromFile("arial.ttf"))
    {
        return 123123;
    }
    Text text;
    text.SetFont(font);
    text.SetCharacterSize(10);

    float z = 2;

    View view(FloatRect(0,0,window.GetWidth(),window.GetHeight()));
    window.SetView(view);

    while(window.IsOpened()) {
        graph.simulate();
        window.Clear();
        //draw
        for(int i=0; i<graph.nodes.size(); i++) {
            Node &n = graph.nodes.at(i);
            text.SetString(n.text);
            text.SetPosition(n.pos.x-n.width()/2,n.pos.y-n.height()/2);
            Shape rect = Shape::Rectangle(n.pos.x-n.width()/2,n.pos.y-n.height()/2,n.width(),n.height(),Color::Black,1,Color::White);
            window.Draw(rect);
            //window.Draw(text);
            for(int j=0; j<n.edges.size(); j++) {
                Node &n2 = n.edges.at(j).target;
                Shape line = Shape::Line(n.pos.x, n.pos.y, n2.pos.x, n2.pos.y, 1, Color::White);
                window.Draw(line);
            }
        }
        //enddraw
        window.Display();

        Event event;
        while(window.PollEvent(event)) {
            if (event.Type == Event::Closed)
                window.Close();
            if (event.Type == Event::MouseWheelMoved) {
                view.Zoom(1-event.MouseWheel.Delta*0.1);
                window.SetView(view);
            }
        }
    }

    return 0;
}
