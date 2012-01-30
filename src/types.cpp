#include <string>
using namespace std;

enum NodeType {
    BLOB,
    TREE,
    COMMIT,
    TAG,
    INDEX
};

typedef string OID;
