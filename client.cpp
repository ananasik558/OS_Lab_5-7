
#include "soket.h"

pid_t create_process() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("fork error!\n");
    }
    return pid;
}

void make_node(Socket &child, int id){
    child.bind(id);
    pid_t fork_id = create_process();
    if (fork_id == 0) {
        execl(CLIENT_NODE, CLIENT_NODE, std::to_string(id).c_str(), nullptr);
    }
}


int main(int argc, char ** argv){
    if (argc != 2){
        throw std::logic_error("./NAME_PROGRAMM id");
    } 

    int node_id = std::atoi(argv[1]);
    Socket socket;
    socket.connect(node_id);
    Socket left_child, right_child;
    zmqmessage * command;
    zmqmessage * ans_left;
    zmqmessage * ans_right;

    std::cout << "Ok " << getpid() << "\n";

    while(true){
        socket.recive_message(command);
        if (command->act == create){
            if(command->id < node_id && left_child.id == -1){
                make_node(left_child, command->id);
            }
            else if(command->id > node_id && right_child.id == -1){
                make_node(right_child, command->id);
            } 
            else if (command->id < node_id && left_child.id != -1){
                left_child.send_message(command);
            } else {
                right_child.send_message(command);
            }
        } else if(command->act == exec) {
            if (node_id == command->id){
                int n;
                std::cin >> n;
                long long res = 0;
                int elem;
                for(int i = 0; i < n; i++){
                    std::cin >> elem;
                    res += elem;
                }
                command->id = res;
            } else if (node_id < command->id){
                right_child.send_message(command);
                right_child.recive_message(command);
            } else {
                left_child.send_message(command);
                left_child.recive_message(command);
            }
            socket.send_message(command);
        }

        else if(command->act == ping){
            std::cout << "\n-------------\n";
            std::cout << "IM " << node_id << "\n";
            std::cout << "LEFT CHILD " << left_child.id << "\n";
            std::cout << "RIGHT CHILD " << right_child.id << "\n";
            std::cout << "-------------\n";
            if(left_child.id != -1){
                left_child.send_message(command);
            }
            if(right_child.id != -1){
                right_child.send_message(command);
            }
        }
        else if (command->act == pingall){
            command->perant += 1;
            zmqmessage * ans = new zmqmessage({fail, command->perant, node_id});
            if(left_child.id != -1){
                ans->act = pingall;
                left_child.send_message(command);
                left_child.recive_message(ans_left);
                while(ans_left->perant - command->perant > 1){
                    socket.send_message(ans_left);
                    left_child.recive_message(ans_left);
                }
                socket.send_message(ans_left);
                
            }
            if(right_child.id != -1){
                ans->act = pingall;
                right_child.send_message(command);
                right_child.recive_message(ans_right);
                while(ans_right->perant - command->perant > 1){
                    socket.send_message(ans_right);
                    right_child.recive_message(ans_right);
                }
                socket.send_message(ans_right);
            }
            socket.send_message(ans);
            delete ans;
        } else if (command->act == destroy){
            if(left_child.id != -1){
                left_child.send_message(command);
            }
            if(right_child.id != -1){
                right_child.send_message(command);
            }
            break;
        } else if (command->act == remove_t){
            if (command->id == left_child.id){               
                command->act = destroy;
                left_child.send_message(command);
                left_child.close();
                left_child.new_socket();
            } else if (command->id == right_child.id) {
                command->act = destroy;
                right_child.send_message(command);
                right_child.new_socket();
            } else if (command->id > node_id){
                right_child.send_message(command);
            } else{
                left_child.send_message(command);
            }
            command->act = fail;
        }
    }
    left_child.close();
    right_child.close();
}