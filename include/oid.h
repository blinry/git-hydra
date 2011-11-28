#ifndef OID_H
#define OID_H

#include "common.h"

class OId {
    public:
        OId(const git_oid *id);
        string format();
    private:
        const git_oid *oid;
};

#endif
