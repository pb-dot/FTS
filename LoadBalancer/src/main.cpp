
// how to use -->prithi@HP$ ./build/lb_exe <Lb_Port>
// Sits between the Server Layer and Switch Emulator Layer
#include "GlobalDS.hpp"

void reader_thread(int sockfd);
void frwd_thread(int sockfd);
void timer_thread();


void load_configuration(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    // Read threshold
    if (!(file >> threshold)) {
        throw std::runtime_error("Failed to read threshold from file.");
    }

    // Consume the remaining newline character
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read sw_emulator
    if (!std::getline(file, sw_emulator)) {
        throw std::runtime_error("Failed to read sw_emulator from file.");
    }

    // Read the size of active_servers
    size_t server_count;
    if (!(file >> server_count)) {
        throw std::runtime_error("Failed to read active_servers size.");
    }

    // Consume the remaining newline character
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read active_servers
    active_servers.clear();
    std::string server;
    for (size_t i = 0; i < server_count; ++i) {
        if (!std::getline(file, server)) {
            throw std::runtime_error("Failed to read an active server entry from file.");
        }
        active_servers.insert(server);
    }

    file.close();
}



int main(int argc, char **argv){

	// checking command line args
	if (argc <2){
		printf(" Enter format :- ./build/lb_exe <Lb_Port> <configFilePath>\n");
        std::cout<<"Suggestion:- Give 9000s port to Layer-3(Tftp servers) \n Give 8000s port to Layer-2(LoadBalencers) \nGive 7000s port to Layer-1(sw_emulator)\n";
        std::cout<<"Config File Format \n The first line contains threshold (an integer).\nThe second line contains sw_emulator[IP_Port] (a string).\nThe third line contains the size of active_servers (an integer).\nThe subsequent lines contain the entries[IP_Port] of active_servers.\n";
		exit(1); 
	}
	
    //init global data-structures
    load_configuration(argv[2]);

	//creating the socket
	int sockfd= socket(AF_INET,SOCK_DGRAM,0);
	check_err(sockfd,"Error in opening UDP socket");
	
	// setting server details
	struct sockaddr_in sa;
	bzero(&sa,sizeof(sa));
	sa.sin_family = AF_INET;//setting address family to IPv4
	sa.sin_port = htons(atoi(argv[1]));
	sa.sin_addr.s_addr=INADDR_ANY;
	
	// binding
	int status= bind(sockfd,(struct sockaddr *) &sa, sizeof(sa));
	check_err(status,"Error in binding");
	
	//optional messages
	printf("\nSetup Finished Sarting 3 threads ...\n\n");

			
	std::thread reader(reader_thread,sockfd);
	std::thread frwd(frwd_thread,sockfd);
	std::thread timer(timer_thread);

	reader.join();
	frwd.join();
	timer.join();

	
	return 0;
}
