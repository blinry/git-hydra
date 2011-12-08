class Edge {
    public:
        Edge(const OID& target, string label, bool folded=true) : target(target), label(label), folded(folded) { }
        OID target;
        string label;
        bool folded;
};
