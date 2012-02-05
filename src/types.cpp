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
    OBJECT
};

enum NodeDisplayType {
    RECT,
    SIGN,
    SNAKE,
    HEAD,
    HALO
};

class NodeID {

    public:

        NodeID() { }

        NodeID(NodeType type, string name) : type(type), name(name) { }

        bool operator==(const NodeID& other) const {
            return other.type == type && other.name == name;
        }

        bool operator<(const NodeID& other) const {
            if (name < other.name)
                return true;
            else if (name == other.name)
                return type < other.type;
            return false;
        }

        NodeType type;
        string name;

};
