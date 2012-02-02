/**
 * Directed Edge that points to another node. Has a label and contains
 * information about whether it is folded, which means that it's parents are
 * hidden.
 */

class Edge {

    public:

        Edge(const Edge &other) : m_target(other.m_target) {
            m_label = other.m_label;
            m_folded = other.m_folded;
        }

        Edge(const NodeID &target, string label, bool folded=true) : m_target(target), m_label(label), m_folded(folded) { }

        const NodeID &target() {
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

        void fold() {
            m_folded = true;
        }

        void toggle_fold() {
            m_folded = !m_folded;
        }

    private:

        NodeID m_target;
        string m_label;
        bool m_folded;
};
