#include <string>
using namespace std;

enum NodeType {
    BLOB,
    TREE,
    COMMIT,
    TAG
};

typedef string OID;
