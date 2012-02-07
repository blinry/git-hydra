enum GitType {
    BLOB,
    TREE,
    COMMIT,
    TAG
};

enum NodeType {
    INDEX,
    INDEX_ENTRY,
    REF,
    TAIL,
    OBJECT
};

enum NodeDisplayType {
    RECT,
    SIGN,
    SNAKE,
    HEAD,
    HALO,
    BAG,
    APPLE,
    SNAKE_TAIL
};

class NodeID {

    public:

        NodeID() { }

        NodeID(NodeType type, string name) : type(type), name(name) { }

        bool operator==(const NodeID& other) const {
            return other.type == type && other.name == name;
        }

        bool operator<(const NodeID& other) const {
            if (type < other.type)
                return true;
            else if (type == other.type)
                return name < other.name;
            return false;
        }

        NodeType type;
        string name;

};
