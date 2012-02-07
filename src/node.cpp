/**
 * A node in a directed graph. Points to some children and contains information
 * about physical simulation.
 */

class Node {
    public:

        Node() {
            needsPosition = true;
            hole = false;
        }

        Node(const NodeID& oid) : m_oid(oid), m_visible(false), m_selected(false) {
            needsPosition = true;
            hole = false;
        }

        const NodeID &oid() {
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

        const GitType &type() {
            return m_type;
        }

        void type(GitType type) {
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
            if (oid().type == INDEX_ENTRY)
                return 2;
            else
                return 10;
        }

        float width() {
            switch(display_type()) {
                case SNAKE:
                    return 50;
                case HEAD:
                case SNAKE_TAIL:
                    return 30;
                default:
                    return 20;
            }
        }

        float height() {
            return width();
        }

        bool parent_of_visible(NodeID oid) {
            for(int k=0; k<degree(); k++) {
                if (edge(k).target() == oid && !edge(k).folded()) {
                    return true;
                }
            }
            return false;
        }

        float dir_to(Node n2) {
            return atan2(pos().x-n2.pos().x,pos().y-n2.pos().y);
        }

        NodeDisplayType display_type() {
            if (type() == COMMIT)
                return SNAKE;
            else if ((type() == TAG && (label().find("refs/heads/") == 0 || (label().find("refs/remotes/") == 0 && label().find("HEAD") == -1))))
                return HEAD;
            else if (type() == TAG)
                return SIGN;
            else if (type() == TREE)
                return BAG;
            else if (type() == BLOB)
                return APPLE;
            else if (oid().type == TAIL)
                return SNAKE_TAIL;
            else if (oid().type == INDEX_ENTRY)
                return MENU_ENTRY;
            else
                return RECT;
        }

        bool needsPosition;
        bool hole;

    private:

        NodeID m_oid;
        string m_label;
        string m_text;
        bool m_visible;
        bool m_selected;
        Vec2f m_pos;
        Vec2f m_velocity;
        GitType m_type;

        vector<Edge> m_children;

};
