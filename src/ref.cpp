#include "ref.h"

Ref::Ref(git_reference *r): ref(r) {
}

string Ref::name() {
    return git_reference_name(ref);
}

OId Ref::oid() {
    return OId(git_reference_oid(ref));
}
