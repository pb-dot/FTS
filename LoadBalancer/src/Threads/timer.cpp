#include "GlobalDS.hpp"

void timer_thread() {
    while (true) {
        // Wait for 15 seconds
        std::this_thread::sleep_for(std::chrono::seconds(15));

        // Temporary list for transferring servers
        std::list<std::string> temp;

        // Transfer deactive_servers to temp
        {
            std::lock_guard<std::mutex> lock_deactive(mtx_DeactiveSL);
            temp.insert(temp.end(), deactive_servers.begin(), deactive_servers.end());
            deactive_servers.clear();
        }

        // Transfer temp to active_servers
        {
            std::lock_guard<std::mutex> lock_active(mtx_ActiveSL);
            active_servers.insert(temp.begin(), temp.end());
        }
    }
}