class Index {
    public:
        Index(NodeFactory& factory) : factory(factory) {
        }
        void update() {
            m_entries = factory.getIndexEntries();
        }
        vector<IndexEntry> entries() {
            return m_entries;
        }
    private:
        NodeFactory factory;
        vector<IndexEntry> m_entries;
};
