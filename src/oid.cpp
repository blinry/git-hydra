#include "oid.h"

OId::OId(const git_oid *id) : oid(id) { }

string OId::format() {
    char ret[40];
    git_oid_fmt(ret, oid);
    string r(ret,40);
    return r;
}
