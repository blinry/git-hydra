/**
 * Directed Edge that points to another OID. Has a label and contains
 * information about whether it is folded, which means that it's parents are
 * hidden.
 */

class Edge {
    public:
        Edge(const OID &target, string label, bool folded=false) : m_target(target), m_label(label), m_folded(folded) { }
        const OID &target() {
            return m_target;
        }
        const string &label() {
            return m_label;
        }
        const bool &folded() {
            return m_folded;
        }
        void unfold() {
            m_folded = false;
        }
    private:
        OID m_target;
        string m_label;
        bool m_folded;
};
