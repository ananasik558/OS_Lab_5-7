#include <iostream>
#include <set>


class Node{

    public:
        int id;
        int height;
        Node* left;
        Node* right;
        Node(int new_id){
            id = new_id; 
            left = right = nullptr;
            height = 1;
        }
};


class AVL_tree {
    private:
        Node *root;
        void destroy_node(Node * node){
            if(node != nullptr){
                destroy_node(node->left);
                destroy_node(node->right);
                delete node;
            }
        }
        void print_tree(Node *node) {
            if (node != nullptr) {
                print_tree(node->left);
                for(int i = 0; i < node->height; i++){
                    std::cout << "  ";
                }
                std::cout << node->id << "\n";
                print_tree(node->right);
            } 
        }

        void fix_height(Node* node){
            if(node->left != nullptr && node->right != nullptr){
	            node->height = std::max(node->left->height, node->right->height) + 1;
            } else if (node->left != nullptr){
                node->height = node->left->height + 1;
            } else if (node->right != nullptr){
                node->height = node->right->height + 1;
            } else{
                node->height = 1;
            }
        }

        int bfactor(Node * node){
           if(node->left != nullptr && node->right != nullptr){
	            return node->right->height - node->left->height;
            } else if (node->left != nullptr){
                return -node->left->height;
            } else if (node->right != nullptr){
                return node->right->height;
            } else{
                return 0;
            }
        }

        Node* rotateright(Node * node){
            Node* new_node = node->left;
            node->left = new_node->right;
            new_node->right = node;
            fix_height(node);
            fix_height(new_node);
            return new_node;
        }

        Node* rotateleft(Node * node){
            Node* q = node->right;
            node->right = q->left;
            q->left = node;
            fix_height(node);
            fix_height(q);
            return q;
        }

        Node* balance(Node* node){
            fix_height(node);
            if(bfactor(node) == 2){
                if(bfactor(node->right) < 0){

                    node->right = rotateright(node->right);
                }
                return rotateleft(node);
            }
            if(bfactor(node) == -2){
                if(bfactor(node->left) > 0){
                    node->left = rotateleft(node->left);
                }
                return rotateright(node);
            }
            return node;
        }
        Node* insert_(Node* p, int id){
            if(p == nullptr){
                return new Node(id);
            }
            if(id < p->id){
                p->left = insert_(p->left, id);
            } else {
                p->right = insert_(p->right, id);
            }
            return balance(p);
        }

        Node * remove(Node *node, int id){
            if (node == nullptr){
                return node;
            }
            if (id == node->id){
                free(node);
                return nullptr;
            }
            if (id < node->id){
                node->left = remove(node->left, id);
            } else {
                node->right = remove(node->right, id);
            }
            return balance(node);
    }

    public:
        std::vector<int> all_elem;
        AVL_tree(){
            root = nullptr;
        }
        ~AVL_tree(){
            destroy_node(root);
        }
        void insert(int id){
            all_elem.push_back(id);
            root = insert_(root, id);
        }
        void print(){
            this->print_tree(root);
        }
        Node * get_root(){
            return root;
        }
        Node *find(int id) {
            Node *node = root;
            while (node != nullptr && node->id != id) {
                if (node->id > id){
                    node = node->left;
                } else{
                    node = node->right;
                }
            }
            return node;
        }
        void remove_t(int id){
            root = remove(root, id);
        }

};