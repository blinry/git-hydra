#include <git2.h>
#include <cstring>
#include <set>

class NodeFactory {
    public:
        NodeFactory(const string& repository_path) : repository_path(repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            if (ret != 0) {
                cerr << repository_path << " doesn't appear to be a Git repository.\n";
                exit(1);
            }
        }
        Node buildNode(const OID& oid) {
            Node node(oid);
            if (is_ref(oid)) {
                git_reference *ref = NULL;
                git_reference_lookup(&ref, repo, oid.c_str());
                if (ref != NULL) {
                    switch(git_reference_type(ref)) {
                        case GIT_REF_OID:
                            {
                                const git_oid* target_id = git_reference_oid(ref);

                                char oid_str[40];
                                git_oid_fmt(oid_str, target_id);
                                OID oid_string(oid_str,40);
                                node.add_edge(Edge(oid_string, "points to"));
                                break;
                            }
                        case GIT_REF_SYMBOLIC:
                            {
                                const char *oid_str;
                                oid_str = git_reference_target(ref);
                                OID oid_string(oid_str,strlen(oid_str));
                                node.add_edge(Edge(oid_string, "points to"));
                                break;
                            }
                        default:
                            exit(1);
                    }
                }
                node.label(oid);
                node.type(TAG);
            } else if (oid == "index") {

                git_repository_free(repo);
                int ret = git_repository_open(&repo, repository_path.c_str());
                node.label(oid);
                node.type(TAG);

                git_index *index;
                git_repository_index(&index, repo);
                for(int i=0; i<git_index_entrycount(index); i++) {
                    git_index_entry *entry;
                    entry = git_index_get(index, i);
                    char oid_str[40];
                    git_oid_fmt(oid_str, &entry->oid);
                    OID oid_string(oid_str,40);
                    node.add_edge(Edge(oid_string, entry->path));
                }
            } else {

                git_oid id;
                git_oid_fromstr(&id, oid.c_str());
                git_object *object;
                git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
                git_otype type = git_object_type(object);

                node.label(oid.substr(0,6));

                switch(type) {
                    case 1: //commit
                        {
                            node.type(COMMIT);
                            git_commit *commit;
                            git_commit_lookup(&commit, repo, &id);

                            node.text(git_commit_message(commit));

                            // parents
                            int parentcount = git_commit_parentcount(commit);
                            for(int i = 0; i<parentcount; i++) {
                                git_commit *parent;
                                git_commit_parent(&parent, commit, i);
                                const git_oid *target_id = git_commit_id(parent);
                                char oid_str[40];
                                git_oid_fmt(oid_str, target_id);
                                OID oid_string(oid_str,40);
                                node.add_edge(Edge(oid_string, "parent"));
                            }

                            /*
                            // tree
                            git_tree *tree;
                            git_commit_tree(&tree, commit);
                            const git_oid *target_id = git_tree_id(tree);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            OID oid_string(oid_str,40);
                            node.add_edge(Edge(oid_string, "tree"));
                            */
                            break;
                        }
                    case 2: //tree
                        {
                            node.type(TREE);
                            git_tree *tree;
                            git_tree_lookup(&tree, repo, &id);

                            int entrycount = git_tree_entrycount(tree);
                            for(int i = 0; i<entrycount; i++) {
                                const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
                                const git_oid *target_id = git_tree_entry_id(entry);
                                char oid_str[40];
                                git_oid_fmt(oid_str, target_id);
                                OID oid_string(oid_str,40);
                                node.add_edge(Edge(oid_string, git_tree_entry_name(entry)));
                            }
                            break;
                        }
                    case 4: //tag
                        node.type(TAG);
                        git_tag *tag;
                        git_tag_lookup(&tag, repo, &id);
                        git_object *target;
                        const git_oid *target_id;
                        target_id = git_tag_target_oid(tag);
                        char oid_str[40];
                        git_oid_fmt(oid_str, target_id);
                        OID oid_string(oid_str,40);
                        node.add_edge(Edge(oid_string, "target"));
                }
            }

            node.pos().x = (rand()%1000000)/1000000.0;
            node.pos().y = (rand()%1000000)/1000000.0;
            return node;
        }
        set<string> getRoots() {
            set<string> roots;

            git_strarray ref_nms;
            git_reference_listall(&ref_nms, repo, GIT_REF_LISTALL);

            for(int i=0; i<ref_nms.count; i++) {
                roots.insert(ref_nms.strings[i]);
            }

            roots.insert("HEAD");
            //roots.insert("index");

            return roots;
        }

    private:
        git_repository *repo; // TODO
        string repository_path;
        bool is_ref(const OID& oid) {
            return oid == "HEAD" || oid.find("/") != string::npos;
        }
};
