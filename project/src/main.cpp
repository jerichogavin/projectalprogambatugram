#include "../include/AttendanceSystem.hpp"
#include <iostream>
#include <sstream>

int main() {
    AttendanceSystem system;
    std::string command;

    std::cout << "RFID Lab Attendance System" << std::endl;
    std::cout << "Commands: scan <rfid> <reader>, status, exit" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        } else if (command == "status") {
            system.displayStatus();
        } else if (command.substr(0, 4) == "scan") {
            std::stringstream ss(command);
            std::string cmd, rfid, reader;
            ss >> cmd >> rfid >> reader;
            if (!rfid.empty() && !reader.empty()) {
                system.simulateRFIDScan(rfid, reader);
            } else {
                std::cout << "Usage: scan <rfid> <reader>" << std::endl;
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }

    return 0;
}