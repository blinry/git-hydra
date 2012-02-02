/**
 * An entry of the index.
 */

class IndexEntry {

    public:

        IndexEntry(string oid, string path, int stage) : m_oid(oid), m_path(path), m_stage(stage) { }

        string oid() {
            return m_oid;
        }

        string path() {
            return m_path;
        }

        int stage() {
            return m_stage;
        }

    private:

        string m_oid, m_path;
        int m_stage;

};
