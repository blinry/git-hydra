enum GitType {
    BLOB,
    TREE,
    COMMIT,
    TAG
};

enum NodeType {
    INDEX,
    REF,
    OBJECT
};

enum NodeDisplayType {
    RECT,
    SIGN,
    SNAKE,
    HEAD
};

class NodeID {

    public:

        NodeID() { }

        NodeID(NodeType type, string name) : type(type), name(name) { }

        bool operator==(const NodeID& other) const {
            return other.type == type && other.name == name;
        }

        bool operator<(const NodeID& other) const {
            return other.name < name;
        }

        NodeType type;
        string name;

};
