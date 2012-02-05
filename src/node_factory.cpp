#include <git2.h>
#include <cstring>

/**
 * This class communicates with the Git repository and creates Nodes.
 */

class NodeFactory {

    public:

        NodeFactory(const string& repository_path) : repository_path(repository_path) {
            int ret = git_repository_open(&repo, repository_path.c_str());
            if (ret != 0) {
                cerr << "You don't seem to be in a Git repository.\n";
                exit(1);
            }
        }

        Node buildNode(const NodeID& oid) {
            Node node(oid);

            node.pos().x = 500+(rand()%1000000)/5000.0;
            node.pos().y = 500+(rand()%1000000)/5000.0;

            if (oid.type == REF) {
                git_reference *ref = NULL;
                git_reference_lookup(&ref, repo, oid.name.c_str());
                if (ref == NULL) {
                    node.add_edge(Edge(NodeID(TAIL,oid.name), "has"));
                } else {
                    switch(git_reference_type(ref)) {
                        case GIT_REF_OID:
                            {
                                const git_oid* target_id = git_reference_oid(ref);

                                char oid_str[40];
                                git_oid_fmt(oid_str, target_id);
                                string oid_string(oid_str,40);
                                node.add_edge(Edge(NodeID(OBJECT,oid_string), "points to"));
                                break;
                            }
                        case GIT_REF_SYMBOLIC:
                            {
                                const char *oid_str;
                                oid_str = git_reference_target(ref);
                                string oid_string(oid_str,strlen(oid_str));
                                node.add_edge(Edge(NodeID(REF,oid_string), "points to"));
                                break;
                            }
                    }
                }
                node.label(oid.name);
                node.type(TAG);
            } else if (oid.type == INDEX) {
                git_repository_free(repo);
                int ret = git_repository_open(&repo, repository_path.c_str());
                node.label(oid.name);

                git_index *index;
                git_repository_index(&index, repo);
                for(int i=0; i<git_index_entrycount(index); i++) {
                    char num[10];
                    sprintf(num, "%d", i);
                    node.add_edge(Edge(NodeID(INDEX_ENTRY,num), "contains"));
                }
            } else if (oid.type == INDEX_ENTRY) {
                git_repository_free(repo);
                int ret = git_repository_open(&repo, repository_path.c_str());
                git_index *index;
                git_repository_index(&index, repo);

                git_index_entry *entry = NULL;
                //cout << "prae\n" << flush;
                if (git_index_entrycount(index) >= atoi(oid.name.c_str()))
                    entry = git_index_get(index, atoi(oid.name.c_str()));
                //cout << "post\n" << flush;

                if (!entry) {
                    node.label("invalid");
                } else {
                    char oid_str[40];
                    git_oid_fmt(oid_str, &entry->oid);
                    string oid_string(oid_str,40);

                    char label[3000];
                    int stage = git_index_entry_stage(entry);
                    if (stage == 0)
                        sprintf(label, "%s", entry->path);
                    else
                        sprintf(label, "%s (%d)", entry->path, stage);

                    node.label(label);
                    node.add_edge(Edge(NodeID(OBJECT,oid_string), "refers"));
                }
            } else if (oid.type == OBJECT) {
                git_oid id;
                git_oid_fromstr(&id, oid.name.c_str());
                git_object *object;
                git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
                git_otype type = git_object_type(object);

                node.label(oid.name.substr(0,6));

                git_odb* odb;

                git_odb_open(&odb, (repository_path+string("objects")).c_str());
                git_odb_object* obj;
                git_odb_read(&obj, odb, &id);
                node.text(string(((const char *)git_odb_object_data(obj)),git_odb_object_size(obj)));

                switch(type) {
                    case 1: //commit
                        {
                            node.type(COMMIT);
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
                                string oid_string(oid_str,40);
                                node.add_edge(Edge(NodeID(OBJECT,oid_string), "parent"));
                            }

                            if (parentcount == 0) {
                                node.add_edge(Edge(NodeID(TAIL,oid.name), "has"));
                            }

                            // tree
                            git_tree *tree;
                            git_commit_tree(&tree, commit);
                            const git_oid *target_id = git_tree_id(tree);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            string oid_string(oid_str,40);
                            node.add_edge(Edge(NodeID(OBJECT,oid_string), "tree"));
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
                                string oid_string(oid_str,40);
                                node.add_edge(Edge(NodeID(OBJECT,oid_string), git_tree_entry_name(entry)));
                            }
                            break;
                        }
                    case 4: //tag
                        {
                            node.type(TAG);
                            git_tag *tag;
                            git_tag_lookup(&tag, repo, &id);
                            git_object *target;
                            const git_oid *target_id;
                            target_id = git_tag_target_oid(tag);
                            char oid_str[40];
                            git_oid_fmt(oid_str, target_id);
                            string oid_string(oid_str,40);
                            node.add_edge(Edge(NodeID(OBJECT,oid_string), "target"));
                            break;
                        }
                    default:
                        node.type(BLOB);
                }
            }

            return node;
        }

        set<NodeID> getRoots() {
            set<NodeID> roots;

            git_strarray ref_nms;
            git_reference_listall(&ref_nms, repo, GIT_REF_LISTALL);

            for(int i=0; i<ref_nms.count; i++) {
                roots.insert(NodeID(REF,ref_nms.strings[i]));
            }

            if (false) {
                FILE *fp;
                int status;
                char path[1035];

                string prog = assets_dir();
                prog += "/list-all-object-ids";
                fp = popen(prog.c_str(), "r");
                if (fp == NULL) {
                    printf("Failed to run command\n" );
                    exit;
                }

                while (fgets(path, sizeof(path)-1, fp) != NULL) {
                    path[strcspn ( path, "\n" )] = '\0';
                    roots.insert(NodeID(OBJECT,path));
                }

                pclose(fp);
            }

            roots.insert(NodeID(REF,"HEAD"));
            roots.insert(NodeID(INDEX,"index"));

            return roots;
        }

    private:

        git_repository *repo; // TODO
        string repository_path;
        string assets_dir() {
            char path_to_program[200];
            int length = readlink("/proc/self/exe", path_to_program, 200);
            path_to_program[length] = '\0';
            string assets_dir = path_to_program;
            return string(assets_dir, 0, assets_dir.rfind("/"));
        }

};
