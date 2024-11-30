#include "GlobalDS.hpp"

std::string LB_algo(std::string&& client_id){
    // Check if the client is already assigned to a server
    mtx_ServerCliMap.lock();
    for (const auto& [server, clients] : serverClientMap) {
        if (clients.find(client_id) != clients.end()) {
            mtx_ServerCliMap.unlock();
            return server; // Return the server to which the client is already assigned
        }
    }
    mtx_ServerCliMap.unlock();

    // Find the active server with the least number of clients
    std::string selected_server;
    size_t min_clients = std::numeric_limits<size_t>::max();
    mtx_ActiveSL.lock();
    for (const auto& server : active_servers) {
        // Check how many clients are assigned to this server
        mtx_ServerCliMap.lock();
        size_t client_count = serverClientMap[server].size();
        mtx_ServerCliMap.unlock();
        if (client_count < min_clients) {
            min_clients = client_count;
            selected_server = server;
        }
    }
    mtx_ActiveSL.unlock();

    // Assign the client to the selected server
    if (!selected_server.empty()) {
        mtx_ServerCliMap.lock();
        serverClientMap[selected_server].insert(client_id);
        mtx_ServerCliMap.unlock();
    }

    return selected_server;
}

void reader_thread(int sockfd){
    //setup
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];
    char * buffer;
    int client_port;
    std::string sender_key;
    

    while (true) {
        // read from socket()
        buffer = new char [BUFFER_SIZE];
        bzero(buffer,BUFFER_SIZE);
        ssize_t received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        check_err(received_bytes,"recvfrom failed");
        buffer[received_bytes] = '\0';  // Null-terminate the received data
        

        // Convert senders address to a readable IP and port
        bzero(client_ip,INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        client_port = ntohs(client_addr.sin_port);

        // Create a unique key associated with every sender
        sender_key = std::string(client_ip) + "_" + std::to_string(client_port);
        
        //debug::
        std::cout<<" Just received a packet from "<<sender_key<<" and start decoding his packet\n";

        // create job and fill job details as per sender
        job thejob;
        
        if(sender_key == sw_emulator){ // sender is switch emulator

            thejob.destn_type= 'T'; // tftp_server
            thejob.packet = buffer;
            thejob.packet_size = received_bytes;
            thejob.destn = LB_algo(std::string(buffer));// passing the client_id(extract from prefix of buffer)

            std::cout<<"The sender of this packet is sw_eml\n; Actual Sender = "<<std::string(buffer)<<" Assigned to tftp server = "<<thejob.destn<<"\n";
        }
        else{ // sender is any tftp server
            
            thejob.destn_type= 'S'; // swtch_emulator
            thejob.packet = buffer;
            thejob.packet_size = received_bytes;
            thejob.destn = sw_emulator;

            mtx_WD.lock();
            watchDogCnt[sender_key]=0;
            mtx_WD.unlock();

            std::cout<<"The sender of this packet is tftfp_server\n";

        }

        std::cout<<"Pushing job to WorkQ\n";
        //push job to WorkQ
        {
            std::lock_guard<std::mutex> lock(mtx_WorkQ);
            WorkQ.push_back(thejob);
        }

        cv_work.notify_one(); // notifies the frwd thread that WorQ has data

    }// inf looping (while(true))
}