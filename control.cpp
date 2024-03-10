#include "soket.h"
#include "AVL-tree.hpp"

pid_t create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("fork error!\n");
    }
    return pid;
}

int main(){
    Socket socket;
    AVL_tree tree;
    std::string s;
    int id;
    
    while(std::cin >> s){
        if (s == "create"){
            std::cin >> id;
            Node *node = tree.find(id); 

            if (node != nullptr) {
                std::cout << "Error: Already exists" << "\n";
                continue;
            }
            if (tree.get_root() == nullptr){
                tree.insert(id);
                socket.bind(id);
                pid_t pid = create_process();
                if (pid == 0){
                    execl(CLIENT_NODE, CLIENT_NODE, std::to_string(id).c_str(), nullptr);     
                }
                
            } else {
                zmqmessage * mes = new zmqmessage({create, 0, id});
                socket.send_message(mes);
                tree.insert(id);
                delete mes;
            }

        }
         else if (s == "remove"){
            std::cin >> id;
            if (tree.find(id) == nullptr){
                std::cout << "Error: "<< id <<": Not found" << "\n";
                continue;
            }
            if (id == tree.get_root()->id){
                tree.remove_t(id);
                zmqmessage * mes = new zmqmessage({destroy, 0, 0});
                socket.send_message(mes);
                socket.new_socket();
                delete mes;
                continue;
            } 
            tree.remove_t(id);
            zmqmessage * mes = new zmqmessage({remove_t, 0, id});
            socket.send_message(mes);
            delete mes;
        }
        else if (s == "ping"){
            if (tree.get_root() == nullptr){
                continue;
            }
            zmqmessage * mes = new zmqmessage({ping, 0, 0});
            socket.send_message(mes);
            delete mes;
        } 
        else if (s == "pingall"){
            zmqmessage * mes = new zmqmessage({pingall, 0, 0});
            socket.send_message(mes);
            socket.recive_message(mes);
            std::set<int> fork_ids;
            while(mes->perant != 1){
                fork_ids.insert(mes->id);
                socket.recive_message(mes);
            }
            fork_ids.insert(mes->id);
            bool flag = false;
            std::cout << "Ok:";
            for(int i = 0; i < tree.all_elem.size(); i++){
                if(fork_ids.count(tree.all_elem[i]) == 0){
                    flag = true;
                    std::cout << tree.all_elem[i] << ";";
                }
            }
            if (flag == false){
                std::cout << "-1";
            }
            std::cout << "\n";
            delete mes;
        }
        else if (s == "exec"){
            int n;
            std::cin >> n;
            if(tree.find(n) == nullptr){
                std::cout << "Error: "<< n <<": Not found" << "\n";
                continue;
            }
            zmqmessage * mes = new zmqmessage({exec, 0, n});
            socket.send_message(mes);
            socket.recive_message(mes);
            std::cout << "Ok:" << n <<":" << mes->id << "\n";
            delete mes;
        }
        
    }
    tree.~AVL_tree();
    socket.close();
    zmq_ctx_destroy(context);

}