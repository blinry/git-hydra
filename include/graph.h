#ifndef GRAPH_H
#define GRAPH_H

class Graph {
    public:
        void insert(Object o) {
        }
        void remove(Object o) {
        }
        void connect(Object o1, Object o2, string label) {
        }
        void disconnect(Object o1, Object o2) {
        }
    private:
        map<git_oid, Object> objects;
};

#endif
