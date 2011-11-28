#ifndef REF_H
#define REF_H

#include "common.h"
#include "oid.h"

class Ref {
    public:
        Ref(git_reference *r);
        string name();
        OId oid();
    private:
        git_reference *ref;
};

#endif
