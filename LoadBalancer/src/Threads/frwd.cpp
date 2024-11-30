#include "GlobalDS.hpp"

void frwd_thread(int sockfd){
    //setup
    job thejob;
    sockaddr_in client_addr;

    while(true){
        
        // Remove front job from the Work Q
        {
            std::unique_lock<std::mutex> lock2(mtx_WorkQ);
            cv_work.wait(lock2, [] { return !WorkQ.empty(); });

            thejob = WorkQ.front();
            WorkQ.pop_front();
        }

        // if the destn is tftp_Server & not in serverClientMap ==> destn is down[dont serve this job]
        if(thejob.destn != sw_emulator){
            mtx_ServerCliMap.lock();
            if(serverClientMap.find(thejob.destn)==serverClientMap.end() or serverClientMap.at(thejob.destn).size()==0){
                mtx_ServerCliMap.unlock();
                delete[] thejob.packet;
                continue;
            }
            mtx_ServerCliMap.unlock();  
        }
      

        //debug lines
        std::cout<<"Inside Frwd Thread After Extract Job\n";
        thejob.display_job();

        // Parse the IP and port from the client key
        size_t pos = thejob.destn.find('_');
        std::string client_ip = thejob.destn.substr(0, pos);
        int client_port = std::stoi(thejob.destn.substr(pos + 1));

        // Set up the client address structure     
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        inet_pton(AF_INET, client_ip.c_str(), &client_addr.sin_addr);
        client_addr.sin_port = htons(client_port);

        //sending Packets to Respective Destination
        ssize_t sent_bytes = sendto(sockfd, thejob.packet, thejob.packet_size, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
        check_err(sent_bytes,"sendto failed");
        delete[] thejob.packet;
        std::cout<<"Packet Sent to "<<thejob.destn<<"\n"; 

        //WatchDog - HeartBeat Check of Tttp server
        bool switchFlag = false;
        if(thejob.destn_type == 'T'){
            mtx_ServerCliMap.lock();
            int num_cli = serverClientMap[thejob.destn].size();
            mtx_ServerCliMap.unlock();

            mtx_WD.lock();
            watchDogCnt[thejob.destn]++;
            if(watchDogCnt[thejob.destn]/num_cli > threshold){
                switchFlag = true;
                watchDogCnt[thejob.destn]=0;
                std::cout<<thejob.destn<<" is down \n";
            }
            mtx_WD.unlock();
        }

        if(switchFlag){// Mark thejob.destn server down
         //Step 1: Remove it from Active Server List 
         mtx_ActiveSL.lock();
         active_servers.erase(thejob.destn);
         mtx_ActiveSL.unlock();
         //Step 2: Add to deactive server list
         mtx_DeactiveSL.lock();
         deactive_servers.insert(thejob.destn);
         mtx_DeactiveSL.unlock();
         //Step 3: Remove from ServerClientMap
         mtx_ServerCliMap.lock();
         serverClientMap.erase(thejob.destn);
         mtx_ServerCliMap.unlock();
        }

    }// inf looping(while(true))
}