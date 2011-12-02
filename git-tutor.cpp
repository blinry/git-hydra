#include <git2.h>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

enum NodeType {
    COMMIT,
    TREE
};

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
        bool expanded;
        float mass() {
            return 1;
        }
        float width() {
            return 5;
        }
        float height() {
            return 5;
        }
        Vec2f pos;
        Vec2f velocity;
        NodeType type;
        bool visible;
};

class NodeFactory {
    public:
        NodeFactory(string repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            //TODO: check.
        }
        Node buildNode(string oid) {

            git_oid id;
            git_oid_fromstr(&id, oid.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            git_otype type = git_object_type(object);

            Node node(oid);

            node.label = git_object_type2string(type);
            node.visible = true;

            switch(type) {
                case 1: //commit
                    {
                        node.type = COMMIT;
                        git_commit *commit;
                        git_commit_lookup(&commit, repo, &id);

                        // parents
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

                        // tree
                        git_tree *tree;
                        git_commit_tree(&tree, commit);
                        const git_oid *target_id = git_tree_id(tree);
                        char oid_str[40];
                        git_oid_fmt(oid_str, target_id);
                        string oid_string(oid_str,40);
                        node.children.push_back(DirectedEdge(oid_string, "tree"));
                        break;
                    }
                case 2: //tree
                    {
                        node.type = TREE;
                        git_tree *tree;
                        git_tree_lookup(&tree, repo, &id);

                        int entrycount = git_tree_entrycount(tree);
                        for(int i = 0; i<entrycount; i++) {
                            const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
                            const git_oid *target_id = git_tree_entry_id(entry);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            string oid_string(oid_str,40);
                            node.children.push_back(DirectedEdge(oid_string, "entry"));
                        }
                        break;
                    }
            }

            node.pos.x = rand()%100+1440/2;
            node.pos.y = rand()%100+900/2;
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
        string head_oid;
        Graph(NodeFactory factory) : factory(factory) {
            head_oid = factory.get_head_commit_oid();
            seed(head_oid);
        }
        void expand(string oid) {
            Node& n = lookup(oid);
            n.expanded = true;
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                show(iter->target_oid);
            }
        }
        void reduce(string oid) {
            Node& n = lookup(oid);
            n.expanded = false;
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                hide(iter->target_oid);
            }
        }
        void show(string oid) {
            Node& n = lookup(oid);
            n.visible = true;
        }
        void hide(string oid) {
            Node& n = lookup(oid);
            n.visible = false;
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                hide(iter->target_oid);
            }
        }
        void seed(string oid, int depth=7) {
            map<string,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
                if(depth>0) {
                    for(vector<DirectedEdge>::iterator iter = nodes[oid].children.begin(); iter != nodes[oid].children.end(); iter++) {
                        seed(iter->target_oid,depth-1);
                    }
                    nodes[oid].expanded = true;
                } else {
                    nodes[oid].expanded = false;
                }
            }
        }
        Node& lookup(string oid) {
            return nodes[oid];
        }
        Node& nearest_node(float x, float y) {
            if (nodes.size() == 0)
                exit(0); //TODO
            Node *best = 0;
            float best_distance = 99999999; //TODO
            Vec2f pos(x,y);
            for(map<string,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                float distance = it->second.pos.distance(pos);
                if (distance < best_distance) {
                    best_distance = distance;
                    best = &(it->second);
                }
            }
            return *best;
        }
        map<string,Node> nodes; // TODO: soll nicht public sein!
    private:
        NodeFactory factory;
};

class ForceDirectedLayout {
    public:
        ForceDirectedLayout() {
            spring=20;
            charge=1000;
            damping=0.1;
        }
        void apply(Graph& graph) {
            for(map<string,Node>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
                Node& n1 = it->second;
                if (!n1.visible) continue;
                for(map<string,Node>::iterator it2 = graph.nodes.begin(); it2 != graph.nodes.end(); it2++) {
                    Node& n2 = it2->second;
                    if (!n2.visible) continue;
                    float distance = n1.pos.distance(n2.pos);
                    if (n1.oid == n2.oid) continue;
                    if (distance == 0) continue;

                    float force = 0;
                    bool connected = false;

                    for(int k=0; k<n1.children.size(); k++) {
                        if (n1.children.at(k).target_oid == n2.oid) {
                            connected = true;
                            //n1.velocity += Vec2f(10,0);
                            //n2.velocity += Vec2f(-10,0);
                        }
                    }
                    for(int k=0; k<n2.children.size(); k++) {
                        if (n2.children.at(k).target_oid == n1.oid)
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

#include <SFML/Graphics.hpp>
using namespace sf;
class SFMLDisplay {
    public:
        SFMLDisplay() : window(VideoMode::GetDesktopMode(), "Gittut"), view(FloatRect(0,0,window.GetWidth(),window.GetHeight())) {
            window.SetView(view);
        }
        void draw(Node n, Graph graph) {
            if (! n.visible) return;
            Color color;
            switch(n.type) {
                case COMMIT:
                    color = Color::Red;
                    break;
                case TREE:
                    color = Color::Green;
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
                for(int j=0; j<n.children.size(); j++) {
                    Node n2 = graph.lookup(n.children.at(j).target_oid);
                    Shape line = Shape::Line(n.pos.x, n.pos.y, n2.pos.x, n2.pos.y, 1, Color::White);
                    window.Draw(line);
                    float dir = atan2(n.pos.x-n2.pos.x,n.pos.y-n2.pos.y);
                    window.Draw(Shape::Line(n2.pos.x+sin(dir)*5, n2.pos.y+cos(dir)*5, n2.pos.x+sin(dir)*5+sin(dir+0.5)*5, n2.pos.y+cos(dir)*5+cos(dir+0.5)*5, 1, Color::White));
                    window.Draw(Shape::Line(n2.pos.x+sin(dir)*5, n2.pos.y+cos(dir)*5, n2.pos.x+sin(dir)*5+sin(dir-0.5)*5, n2.pos.y+cos(dir)*5+cos(dir-0.5)*5, 1, Color::White));
                    draw(n2, graph);
                }
            }
            Shape rect = Shape::Rectangle(n.pos.x-n.width()/2,n.pos.y-n.height()/2,n.width(),n.height(),color,1,border_color);
            window.Draw(rect);
        }
        void draw(Graph graph) {
            window.Clear();
            draw(graph.lookup(graph.head_oid), graph);
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
        }
        bool open() {
            return window.IsOpened();
        }
    private:
        RenderWindow window;
        View view;
};

int main(int argc, const char *argv[])
{
    srand(time(NULL));
    NodeFactory node_factory("/home/seb/.dotfiles/.git/");
    //NodeFactory node_factory("/home/seb/projects/advent/.git/");
    //NodeFactory node_factory("/home/seb/projects/libgit2/.git/");
    //NodeFactory node_factory("/home/seb/projects/linux/.git/");
    //NodeFactory node_factory("/home/seb/projects/git/.git/");
    Graph graph(node_factory);
    ForceDirectedLayout layout;
    SFMLDisplay display;
    while(display.open()) {
        layout.apply(graph);
        display.draw(graph);
        display.process_events(graph);
    }
    return 0;
}
