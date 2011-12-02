#include <git2.h>
#include <string>
#include <iostream>
#include <cmath>
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

class DirectedEdge {
    public:
        DirectedEdge(string target_oid, string label) : target_oid(target_oid), label(label) { }
        string target_oid;
        string label;
};

#include <vector>
class Node {
    public:
        Node() { }
        Node(string oid) : oid(oid) { }
        string oid;
        string label;
        vector<DirectedEdge> children;
        float mass() {
            return 1;
        }
        float width() {
            return 1;
        }
        float height() {
            return 1;
        }
        Vec2f pos;
        Vec2f velocity;
};

class NodeFactory {
    public:
        NodeFactory(string repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            //TODO: check.
        }
        Node buildNode(string oid) {
            Node node(oid);

            git_oid id;
            git_oid_fromstr(&id, oid.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            git_otype type = git_object_type(object);

            node.label = git_object_type2string(type);

            switch(type) {
                case 1: //commit
                    git_commit *commit;
                    git_commit_lookup(&commit, repo, &id);

                    int parentcount = git_commit_parentcount(commit);
                    for(int i = 0; i<parentcount; i++) {
                        git_commit *parent;
                        git_commit_parent(&parent, commit, i);
                        const git_oid *target_id = git_commit_id(parent);
                        char oid_str[40];
                        git_oid_fmt(oid_str, target_id);
                        string oid_string(oid_str,40);
                        node.children.push_back(DirectedEdge(oid_string, "parent"));
                    }
                    break;
            }

            node.pos.x = rand()%600;
            node.pos.y = rand()%400;
            return node;
        }
        string get_head_commit_oid() {
            git_reference *ref = 0;
            git_repository_head(&ref, repo);
            const git_oid *oid = git_reference_oid(ref);
            char oid_str[40];
            git_oid_fmt(oid_str, oid);
            string oid_string(oid_str,40);
            return oid_string;
        }
    private:
        git_repository *repo; 
};

#include <map>
class Graph {
    public:
        Graph(NodeFactory factory) : factory(factory) {
            seed(factory.get_head_commit_oid());
        }
        void seed(string oid) {
            map<string,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
                for(vector<DirectedEdge>::iterator iter = nodes[oid].children.begin(); iter != nodes[oid].children.end(); iter++) {
                    seed(iter->target_oid);
                }
            }
        }
        Node lookup(string oid) {
            return nodes[oid];
        }
        map<string,Node> nodes; // TODO: soll nicht public sein!
    private:
        NodeFactory factory;
};

class ForceDirectedLayout {
    public:
        ForceDirectedLayout() {
            spring=10;
            charge=10;
            damping=0.6;
        }
        void apply(Graph& graph) {
            for(map<string,Node>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
                Node& n1 = it->second;
                for(map<string,Node>::iterator it2 = graph.nodes.begin(); it2 != graph.nodes.end(); it2++) {
                    Node& n2 = it2->second;
                    float distance = n1.pos.distance(n2.pos);
                    if (n1.oid == n2.oid) continue;

                    float force = 0;
                    bool connected = false;

                    for(int k=0; k<n1.children.size(); k++) {
                        if (n1.children.at(k).target_oid == n2.oid)
                            connected = true;
                    }
                    for(int k=0; k<n2.children.size(); k++) {
                        if (n2.children.at(k).target_oid == n1.oid)
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
    private:
        float spring, charge, damping;
};

#include <SFML/Graphics.hpp>
using namespace sf;
class SFMLDisplay {
    public:
        SFMLDisplay() : window(VideoMode::GetDesktopMode(), "Gittut") {
            //View view(FloatRect(0,0,window.GetWidth(),window.GetHeight()));
            //window.SetView(view);
        }
        void draw(Graph graph) {
            window.Clear();
            for(map<string,Node>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
                Node n = it->second;
                Shape rect = Shape::Rectangle(n.pos.x-n.width()/2,n.pos.y-n.height()/2,n.width(),n.height(),Color::Black,1,Color::White);
                window.Draw(rect);
                for(int j=0; j<n.children.size(); j++) {
                    Node n2 = graph.lookup(n.children.at(j).target_oid);
                    Shape line = Shape::Line(n.pos.x, n.pos.y, n2.pos.x, n2.pos.y, 1, Color::White);
                    window.Draw(line);
                }
            }
            window.Display();
        }
        void process_events() {
            Event event;
            while(window.PollEvent(event)) {
                if (event.Type == Event::Closed)
                    window.Close();
                if (event.Type == Event::MouseWheelMoved) {
                    //view.Zoom(1-event.MouseWheel.Delta*0.1);
                    //window.SetView(view);
                }
            }
        }
        bool open() {
            return window.IsOpened();
        }
    private:
        RenderWindow window;
};

int main(int argc, const char *argv[])
{
    NodeFactory node_factory("/home/seb/projects/advent/.git/");
    Graph graph(node_factory);
    ForceDirectedLayout layout;
    SFMLDisplay display;
    while(display.open()) {
        layout.apply(graph);
        display.draw(graph);
        display.process_events();
    }
    return 0;
}
