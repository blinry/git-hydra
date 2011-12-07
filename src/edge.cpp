class Edge {
    public:
        Edge(const OID& target, string label) : target(target), label(label) { }
        OID target;
        string label;
};
