#pragma once

#include "std_headers.h"


////////////////////////////////////////// contents of Worker Queue

class job{
public:

    char destn_type; // T for tftp_server | S for switch_emulator
    std::string destn; // Ip+"_"+Port
    char * packet;
    int packet_size;

    void display_job(){
        std::cout<<"dest_type = "<<destn_type<<"\n";
        std::cout<<"destn = "<<destn<<"\n";
        // // Output the resulting string in hex for verification
        // std::cout<<"Packet in hex form \n";
        // for (int i =0;i<packet_size;++i) {
        //     printf("%02X ", packet[i]);
        // }
        // std::cout << "\n";
        // std::cout<<"packet_size = "<<packet_size<<"\n";
    }
    ~job(){;} // to prevent free the dynamic memory by default destructor
}; 


///////////////////////////////////////////// DS and variables //////////////////////

// key/server_id = IP + Port | int = watch Dog count
inline std::unordered_map<std::string , int> watchDogCnt;
inline int threshold;//Max Number of retransmission allowed beyond which server is declare dead

inline std::unordered_map<std::string, std::unordered_set<std::string>> serverClientMap;

inline std::list<job> WorkQ;

inline std::string sw_emulator;// IP_Port of switch Emulator
inline std::unordered_set<std::string> active_servers;//List of TFTP_servers
inline std::unordered_set<std::string> deactive_servers;

inline std::mutex mtx_WD, mtx_ServerCliMap, mtx_WorkQ , mtx_ActiveSL , mtx_DeactiveSL;      
inline std::condition_variable cv_work;// Condition variable to notify frwd thread

/////////////////////////////// Helper Functions ////////////////////

inline void check_err(int fd,std::string mssg){
	if(fd <0){
		perror(mssg.c_str());
		exit(1);
	}	
}

