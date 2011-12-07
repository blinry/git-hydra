#include <vector>
class Node {
    public:
        OID oid;
        string label;
        vector<Edge> children;
        bool expanded;
        bool visible;
        Vec2f pos;
        Vec2f velocity;
        NodeType type;

        Node() { }
        Node(const OID& oid) : oid(oid) { }
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
};
