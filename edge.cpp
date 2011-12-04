class Edge {
    public:
        Edge(const OID& target_oid, string label) : target_oid(target_oid), label(label) { }
        OID target_oid;
        string label;
};
