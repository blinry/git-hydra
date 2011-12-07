class Edge {
    public:
        Edge(const OID& target, string label, bool folded=false) : target(target), label(label), folded(folded) { }
        OID target;
        string label;
        bool folded;
};
