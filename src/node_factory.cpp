#include <git2.h>
#include <cstring>

class NodeFactory {
    public:
        NodeFactory(const string& repository_path) : repository_path(repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            //TODO: check.
        }
        Node buildNode(const OID& oid) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            if (is_ref(oid)) {
                Node node(oid);
                git_reference *ref;
                git_reference_lookup(&ref, repo, oid.c_str());
                switch(git_reference_type(ref)) {
                    case GIT_REF_OID:
                        {
                            const git_oid* target_id = git_reference_oid(ref);

                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            OID oid_string(oid_str,40);
                            node.children.push_back(Edge(oid_string, "points to"));
                            break;
                        }
                    case GIT_REF_SYMBOLIC:
                        {
                            const char *oid_str;
                            oid_str = git_reference_target(ref);
                            OID oid_string(oid_str,strlen(oid_str));
                            node.children.push_back(Edge(oid_string, "points to"));
                            break;
                        }
                    default:
                        exit(0);
                }

                node.visible = true;
                //node.expanded = true;
                node.pos.x = rand()%100+250;
                node.pos.y = rand()%100+250;
                node.label = oid;
                node.type = TAG;
                return node;
            }

            git_oid id;
            git_oid_fromstr(&id, oid.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            git_otype type = git_object_type(object);

            Node node(oid);

            node.label = oid.substr(0,6);
            node.visible = true;
            //node.expanded = true;

            switch(type) {
                case 1: //commit
                    {
                        node.type = COMMIT;
                        git_commit *commit;
                        git_commit_lookup(&commit, repo, &id);

                        // parents
                        int parentcount = git_commit_parentcount(commit);
                        for(int i = 0; i<parentcount; i++) {
                            git_commit *parent;
                            git_commit_parent(&parent, commit, i);
                            const git_oid *target_id = git_commit_id(parent);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            OID oid_string(oid_str,40);
                            node.children.push_back(Edge(oid_string, "parent"));
                        }

                        // tree
                        git_tree *tree;
                        git_commit_tree(&tree, commit);
                        const git_oid *target_id = git_tree_id(tree);
                        char oid_str[40];
                        git_oid_fmt(oid_str, target_id);
                        OID oid_string(oid_str,40);
                        node.children.push_back(Edge(oid_string, "tree", true));
                        //node.visible = true;
                        //node.expanded = false;
                        break;
                    }
                case 2: //tree
                    {
                        node.type = TREE;
                        git_tree *tree;
                        git_tree_lookup(&tree, repo, &id);

                        int entrycount = git_tree_entrycount(tree);
                        for(int i = 0; i<entrycount; i++) {
                            const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
                            const git_oid *target_id = git_tree_entry_id(entry);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            OID oid_string(oid_str,40);
                            node.children.push_back(Edge(oid_string, "entry"));
                        }
                        break;
                    }
                case 4: //tag
                    node.type = TAG;
                    git_tag *tag;
                    git_tag_lookup(&tag, repo, &id);
                    git_object *target;
                    git_tag_target(&target, tag);
                    const git_oid *target_id;
                    target_id = git_object_id(target);
                    char oid_str[40];
                    git_oid_fmt(oid_str, target_id);
                    OID oid_string(oid_str,40);
                    node.children.push_back(Edge(oid_string, "target"));
            }

            node.pos.x = rand()%100+250;
            node.pos.y = rand()%100+250;
            return node;
        }
        git_repository *repo; // TODO
    private:
        string repository_path;
        bool is_ref(const OID& oid) {
            return oid == "HEAD" || oid.find("/") != string::npos;
        }
};
