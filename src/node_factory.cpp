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

            git_odb_open(&odb, (repository_path+string("objects")).c_str());

            all_objects = false;
            all_refs = false;
            show_index = false;
            unfold_new_commits = false;
            unfold_all = false;

            index_pos = 130;
        }

        ~NodeFactory() {
            git_repository_free(repo);
            git_odb_free(odb);
        }

        Node buildNode(const NodeID& oid) {
            Node node(oid);
            node.pos(500,500);

            switch(oid.type) {
                case REF:
                    build_ref(node);
                    break;
                case INDEX:
                    build_index(node);
                    break;
                case INDEX_ENTRY:
                    build_index_entry(node);
                    break;
                case OBJECT:
                    build_object(node);
                    break;
            }

            return node;
        }

        void build_ref(Node& node) {
            node.type(TAG);
            node.pos(1000/6, 500);
            node.needsPosition = false;

            git_reference *ref = NULL;
            git_reference_lookup(&ref, repo, node.oid().name.c_str());

            if (ref == NULL) {
                node.add_edge(Edge(NodeID(TAIL,node.oid().name)));
            } else {
                if (git_reference_type(ref) == GIT_REF_OID) {
                    node.add_edge(Edge(NodeID(OBJECT,oidstr(git_reference_oid(ref)))));
                } else {
                    const char *oid_str;
                    oid_str = git_reference_target(ref);
                    string oid_string(oid_str,strlen(oid_str));
                    node.add_edge(Edge(NodeID(REF,oid_string)));
                }
            }
            git_reference_free(ref);
            node.label(node.oid().name);
        }

        void build_index(Node& node) {
            node.label(node.oid().name);

            if (show_index) {
                git_index *index;
                git_repository_index(&index, repo);
                git_index_read(index);

                if (git_index_entrycount(index) == 0) {
                    // happens mostly in new repositories. we need a dirty hack here:
                    git_repository_free(repo);
                    git_repository_open(&repo, repository_path.c_str());
                }

                int start_index = (150-index_pos)/30;
                int end_index = start_index+(height-150)/30;
                if (start_index<0)
                    start_index = 0;
                for(int i=start_index; i<git_index_entrycount(index) && i<=end_index; i++) {
                    char num[10];
                    sprintf(num, "%d", i);
                    node.add_edge(Edge(NodeID(INDEX_ENTRY,num)));
                }
                git_index_free(index);
            }
        }

        void build_index_entry(Node& node) {
            git_index *index;
            git_repository_index(&index, repo);

            git_index_entry *entry = NULL;
            if (git_index_entrycount(index) >= atoi(node.oid().name.c_str()))
                entry = git_index_get(index, atoi(node.oid().name.c_str()));

            if (!entry) {
                node.label("invalid");
            } else {
                char label[3000];
                int stage = git_index_entry_stage(entry);
                if (stage == 0)
                    sprintf(label, "%s", entry->path);
                else
                    sprintf(label, "%s (%d)", entry->path, stage);

                node.label(label);
                if (show_index)
                    node.add_edge(Edge(NodeID(OBJECT,oidstr(&entry->oid))));
            }
            git_index_free(index);
        }

        void build_object(Node& node) {
            git_oid id;
            git_oid_fromstr(&id, node.oid().name.c_str());
            git_object *object;
            git_object_lookup(&object, repo, &id, GIT_OBJ_ANY);
            if (object == NULL)
                return;
            git_otype type = git_object_type(object);
            git_object_free(object);

            node.label(node.oid().name.substr(0,6)+string("..."));

            git_odb_object* obj;
            git_odb_read(&obj, odb, &id);
            if (obj == NULL)
                return;
            int content_size = git_odb_object_size(obj);
            if (content_size > 2000)
                node.text(string(((const char *)git_odb_object_data(obj)), 2000)+"...");
            else
                node.text(string(((const char *)git_odb_object_data(obj)), content_size));

            git_odb_object_free(obj);

            switch(type) {
                case 1: //commit
                    build_commit(node, id);
                    break;
                case 2: //tree
                    build_tree(node, id);
                    break;
                case 4: //tag
                    build_tag(node, id);
                    break;
                default:
                    node.type(BLOB);
            }
        }

        void build_tag(Node& node, git_oid id) {
            node.type(TAG);

            git_tag *tag;
            git_tag_lookup(&tag, repo, &id);

            node.add_edge(Edge(NodeID(OBJECT,oidstr(git_tag_target_oid(tag)))));
            git_tag_free(tag);
        }

        void build_tree(Node& node, git_oid id) {
            node.type(TREE);

            git_tree *tree;
            git_tree_lookup(&tree, repo, &id);

            for(int i = 0; i<git_tree_entrycount(tree); i++) {
                const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
                node.add_edge(Edge(NodeID(OBJECT,oidstr(git_tree_entry_id(entry))), git_tree_entry_name(entry)));
            }
            git_tree_free(tree);
        }

        void build_commit(Node& node, git_oid id) {
            node.type(COMMIT);

            git_commit *commit;
            git_commit_lookup(&commit, repo, &id);

            // parents
            int parentcount = git_commit_parentcount(commit);
            for(int i = 0; i<parentcount; i++) {
                git_commit *parent;
                git_commit_parent(&parent, commit, i);
                node.add_edge(Edge(NodeID(OBJECT,oidstr(git_commit_id(parent)))));
                git_commit_free(parent);
            }

            if (parentcount == 0)
                node.add_edge(Edge(NodeID(TAIL,node.oid().name)));

            // tree
            git_tree *tree;
            git_commit_tree(&tree, commit);
            node.add_edge(Edge(NodeID(OBJECT,oidstr(git_tree_id(tree))), ""));
            git_tree_free(tree);
            git_commit_free(commit);
            if (unfold_new_commits)
                node.toggle_select();
        }

        set<NodeID> getRoots() {
            set<NodeID> roots;

            if (all_refs) {
                git_strarray ref_nms;
                git_reference_listall(&ref_nms, repo, GIT_REF_LISTALL);

                for(int i=0; i<ref_nms.count; i++) {
                    roots.insert(NodeID(REF,ref_nms.strings[i]));
                }
                git_strarray_free(&ref_nms);
            }

            if (all_objects) {
                FILE *fp;
                int status;
                char path[1035];

                string prog = assets_dir();
                prog += "/res/list-all-object-ids";
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

        bool all_objects;
        bool all_refs;
        bool show_index;
        bool unfold_new_commits;
        bool unfold_all;

        float index_pos;
        float height;

    private:

        string oidstr(const git_oid* oid) {
            char oid_str[40];
            git_oid_fmt(oid_str, oid);
            return string(oid_str,40);
        }

        git_repository *repo;
        git_odb *odb;
        string repository_path;
        string assets_dir() {
            char path_to_program[200];
            int length = readlink("/proc/self/exe", path_to_program, 200);
            path_to_program[length] = '\0';
            string assets_dir = path_to_program;
            return string(assets_dir, 0, assets_dir.rfind("/"));
        }

};
