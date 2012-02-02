/**
 * A node in a directed graph. Points to some children and contains information
 * about physical simulation.
 */

class Node {

    public:

        Node() { }

        Node(const OID& oid) : m_oid(oid), m_visible(false), m_selected(false) { }

        const OID &oid() {
            return m_oid;
        }

        const string &label() {
            return m_label;
        }

        void label(const string &label) {
            m_label = label;
        }

        const string &text() {
            return m_text;
        }

        void text(const string &text) {
            m_text = text;
        }

        const bool &visible() {
            return m_visible;
        }

        void show() {
            m_visible = true;
        }

        void hide() {
            m_visible = false;
        }

        const bool selected() {
            return m_selected;
        }

        void select() {
            m_selected = true;
        }

        void toggle_select() {
            m_selected = !m_selected;
        }

        OID toggle_tree() {
            for(int i=0; i<m_children.size(); i++) {
                if (edge(i).label() == "tree") {
                    edge(i).toggle_fold();
                    return edge(i).target();
                }
            }
        }

        Vec2f &pos() {
            return m_pos;
        }

        void pos(float x, float y) {
            m_pos.x = x;
            m_pos.y = y;
        }

        Vec2f &velocity() {
            return m_velocity;
        }

        const NodeType &type() {
            return m_type;
        }

        void type(NodeType type) {
            m_type = type;
        }

        void add_edge(Edge edge) {
            m_children.push_back(edge);
        }

        int degree() {
            return m_children.size();
        }

        Edge &edge(int i) {
            return m_children.at(i);
        }

        float mass() {
            return width();
        }

        float width() {
            switch(type()) {
                case COMMIT:
                    return 30;
                default:
                    return 10;
            }
        }

        float height() {
            return width();
        }

    private:

        OID m_oid;
        string m_label;
        string m_text;
        bool m_visible;
        bool m_selected;
        Vec2f m_pos;
        Vec2f m_velocity;
        NodeType m_type;

        vector<Edge> m_children;
};
