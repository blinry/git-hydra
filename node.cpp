#include <vector>
class Node {
    public:
        Node() { }
        Node(const OID& oid) : oid(oid) { }
        OID oid;
        string label;
        vector<Edge> children;
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
