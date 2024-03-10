
#include <iostream>
#include "zmq.h"
#include <cstring>
#include <vector>


const int MAIN_id = 4000;
const char * CLIENT_NODE = "cl";
void * context = zmq_ctx_new();

enum actions{
  fail = 0,
  success = 1,
  create = 2,
  pingall = 3,
  ping = 4,
  exec = 5,
  destroy = 6,
  remove_t = 7
};

struct zmqmessage{
    actions act;
    int perant;
    int id;
};





class Socket{
    public:
        void * socket;
        int id;

        Socket(){
            id = -1;
            socket = zmq_socket(context, ZMQ_PAIR);
            
        }
        void bind(int node){
            id = node;
            if(zmq_bind(socket, ("tcp://*:" + std::to_string(MAIN_id + id)).c_str())){
                throw std::runtime_error("ZMQ_bind");
            }
        }
        void unbind(){
            if (id == -1){
                return;
            }
            if(zmq_unbind(socket, ("tcp://*:" + std::to_string(MAIN_id + id)).c_str())){
                throw std::runtime_error("ZMQ_unbind" + std::to_string(id));
            }
            id = -1;
        }

        void connect(int node){
            id = node;
            if(zmq_connect(socket, ("tcp://localhost:" + std::to_string(MAIN_id + id)).c_str())){
                throw std::runtime_error("ZMQ_con");
            }
        }

        void disconnect(){
            if (id == -1){
                return;
            }
            if(zmq_disconnect(socket, ("tcp://*:" + std::to_string(MAIN_id + id)).c_str())){
                throw std::runtime_error("ZMQ_discon");
            }
            id = -1;
        }

        void send_message(const zmqmessage * mes){
            zmq_send(socket, mes, sizeof(zmqmessage), 0);

          
        }
        void recive_message(zmqmessage *&mes){
            zmq_recv(socket, mes, sizeof(zmqmessage), 0);
        }

        void close(){
            zmq_close(socket);
        }
        void new_socket(){
            id = -1;
            socket = zmq_socket(context, ZMQ_PAIR);
        }
};