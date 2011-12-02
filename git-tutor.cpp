#include <git2.h>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

enum NodeType {
    BLOB,
    TREE,
    COMMIT,
    TAG
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
            switch(type) {
                case COMMIT:
                    return 10;
                default:
                    return 1;
            }
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
        NodeFactory(string repository_path) : repository_path(repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            //TODO: check.
        }
        Node buildNode(string oid) {
            size_t found = oid.find("/");
            if (found != string::npos || oid == "HEAD") { // it's a ref!
                Node node(oid);
                git_reference *ref;
                git_reference_lookup(&ref, repo, oid.c_str());
                switch(git_reference_type(ref)) {
                    case GIT_REF_OID:
                        {
                            const git_oid* target_id = git_reference_oid(ref);

                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            string oid_string(oid_str,40);
                            node.children.push_back(DirectedEdge(oid_string, "points to"));
                            break;
                        }
                    case GIT_REF_SYMBOLIC:
                        {
                            string oid_string = git_reference_target(ref);
                            node.children.push_back(DirectedEdge(oid_string, "points to"));
                            break;
                        }
                    default:
                        exit(0);
                }

                node.visible = true;
                node.expanded = true;
                node.pos.x = rand()%100+250;
                node.pos.y = rand()%100+250;
                node.label = oid;
                node.type = TAG;
                return node;
            }

            git_oid id;
            git_oid_fromstr(&id, oid.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            git_otype type = git_object_type(object);

            Node node(oid);

            node.label = oid.substr(0,6);
            node.visible = true;
            node.expanded = true;

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
                        //node.visible = true;
                        //node.expanded = false;
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

            node.pos.x = rand()%100+250;
            node.pos.y = rand()%100+250;
            return node;
        }
        string get_head_commit_oid() {
            int ret = git_repository_open(&repo, repository_path.c_str());
            git_reference *ref = 0;
            git_repository_head(&ref, repo);
            const git_oid *oid = git_reference_oid(ref);
            char oid_str[40];
            git_oid_fmt(oid_str, oid);
            string oid_string(oid_str,40);
            return oid_string;
        }
        git_repository *repo; // TODO
    private:
        string repository_path;
};

#include <map>
#include <set>
class Graph {
    public:
        string head_oid;
        Graph(NodeFactory factory) : factory(factory) {
            reseed();
        }
        void expand(string oid) {
            Node& n = lookup(oid);
            n.expanded = true;
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                show(iter->target_oid);
                if (lookup(iter->target_oid).expanded)
                    expand(iter->target_oid);
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
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                if (lookup(iter->target_oid).expanded)
                    expand(iter->target_oid);
            }
        }
        void hide(string oid) {
            Node& n = lookup(oid);
            n.visible = false;
            for(vector<DirectedEdge>::iterator iter = n.children.begin(); iter != n.children.end(); iter++) {
                hide(iter->target_oid);
            }
        }
        void seed(string oid, int depth=999) {
            map<string,Node>::iterator it = nodes.find(oid);
            if (it == nodes.end()) {
                // map doesn't contain oid yet
                nodes[oid] = factory.buildNode(oid);
                /*
                for(vector<DirectedEdge>::iterator iter = nodes[oid].children.begin(); iter != nodes[oid].children.end(); iter++) {
                    seed(iter->target_oid,depth-1);
                }
                */
                nodes[oid].expanded = true;
            }
        }
        Node& lookup(string oid) {
            size_t found = oid.find("/");
            if (found != string::npos || oid == "HEAD") { // it's a ref!
                map<string,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    nodes[oid] = factory.buildNode(oid);
                } else {
                    // it's there, but maybe it needs an update.
                    Node new_ref = factory.buildNode(oid);
                    nodes[oid].children = new_ref.children;
                }
                return nodes[oid];
            } else {
                map<string,Node>::iterator it = nodes.find(oid);
                if (it == nodes.end()) {
                    seed(oid,0);
                    if (nodes[oid].type != COMMIT)
                        reduce(oid);
                }
                return nodes[oid];
            }
        }
        Node& nearest_node(float x, float y) {
            if (nodes.size() == 0)
                exit(0); //TODO
            Node *best = 0;
            float best_distance = 99999999; //TODO
            Vec2f pos(x,y);
            for(map<string,Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
                if (!it->second.visible) continue;
                float distance = it->second.pos.distance(pos);
                if (distance < best_distance) {
                    best_distance = distance;
                    best = &(it->second);
                }
            }
            return *best;
        }
        void reseed() {
            git_strarray ref_nms;
            git_reference_listall(&ref_nms, factory.repo, GIT_REF_LISTALL);
            ref_names.clear();
            for(int i=0; i<ref_nms.count; i++) {
                ref_names.insert(ref_nms.strings[i]);
            }
            ref_names.insert("HEAD");
            //string the_head_oid = factory.get_head_commit_oid();
            //head_oid = the_head_oid;
            //seed(the_head_oid);
        }
        map<string,Node> nodes; // TODO: soll nicht public sein!
        //map<string,Node> refs; // TODO: soll nicht public sein!
        set<string> ref_names;
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
                /*
                   if (n1.type == COMMIT)
                   1.velocity += Vec2f(0,400-n1.pos.y);
                   else
                   n1.velocity += Vec2f(0,500-n1.pos.y);
                   */
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
                        if (n1.children.at(k).target_oid == n2.oid && n1.expanded) {
                            connected = true;
                            n1.velocity += Vec2f(10,0);
                            n2.velocity += Vec2f(-10,0);
                        }
                    }
                    for(int k=0; k<n2.children.size(); k++) {
                        if (n2.children.at(k).target_oid == n1.oid && n2.expanded)
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
            text.SetPosition(n.pos.x-n.width()/2,n.pos.y-n.height()/2);
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

int main(int argc, const char *argv[])
{
    srand(time(NULL));
    NodeFactory node_factory("/home/seb/.dotfiles/.git/");
    //NodeFactory node_factory("/home/seb/projects/advent/.git/");
    //NodeFactory node_factory("/home/seb/projects/libgit2/.git/");
    //NodeFactory node_factory("/home/seb/projects/linux/.git/");
    //NodeFactory node_factory("/home/seb/projects/git/.git/");
    //NodeFactory node_factory("/home/seb/tmp/test/.git/");
    Graph graph(node_factory);
    ForceDirectedLayout layout;
    SFMLDisplay display;
    while(display.open()) {
        graph.reseed();
        layout.apply(graph);
        display.draw(graph);
        display.process_events(graph);
    }
    return 0;
}
