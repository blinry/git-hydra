#include <vector>
class Node {
    public:
        OID oid;
        string label;
        string text;
        vector<Edge> children;
        bool visible;
        Vec2f pos;
        Vec2f velocity;
        NodeType type;

        Node() { }
        Node(const OID& oid) : oid(oid) {
        }
        float mass() {
            return 10;
        }
        float width() {
            return 5;
        }
        float height() {
            return 5;
        }
};
