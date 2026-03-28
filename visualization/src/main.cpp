#include <zmq.hpp>
#include <google/protobuf/message.h>
#include <iostream>
#include "star_data.pb.h"

int main() {
    // Initialize ZeroMQ context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ); // Request pattern
    socket.connect("tcp://localhost:5656");

//     try {
//     socket.connect("tcp://localhost:5555");
// } catch (const zmq::error_t& e) {
//     std::cerr << "Connect failed: " << e.what() << std::endl;
//     return 1;
// }

    // Example: Send DataRequest
    mwm_msgs::DataRequest req;
    req.set_timestamp(1234567890);
    req.set_node_name("my_client");

    std::string serialized;
    req.SerializeToString(&serialized);

//     if (!req.SerializeToString(&serialized)) {
//     std::cerr << "Failed to serialize request!" << std::endl;
//     return 1;
// }

    // Send message
    std::cout << "Sending request..." << std::endl;
    zmq::message_t request(serialized.data(), serialized.size());
    socket.send(request, zmq::send_flags::none);

    // Receive reply
    zmq::message_t reply;
    socket.recv(reply, zmq::recv_flags::none);

    // Parse Stars message
    mwm_msgs::Stars stars;
    if (!stars.ParseFromArray(reply.data(), reply.size())) {
        std::cerr << "Failed to parse stars message!" << std::endl;
        return 1;
    }

    // Process reply
    std::cout << "Received " << stars.stars().size() << " stars at timestamp: "
              << stars.timestamp() << std::endl;

    return 0;
}
