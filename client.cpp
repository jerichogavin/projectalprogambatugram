
//  ____   ____  ___ ___  ____    ___   __ __  ______  ______   ___    ____  ____    ____  ___ ___ 
// |    | /    ||   |   ||    \  /   \ |  |  ||      ||      | /   \  /    ||    \  /    ||   |   |
//  |  | |  o  || _   _ ||  o  )|     ||  |  ||      ||      ||     ||   __||  D  )|  o  || _   _ |
//  |  | |     ||  \_/  ||     ||  O  ||  |  ||_|  |_||_|  |_||  O  ||  |  ||    / |     ||  \_/  |
//  |  | |  _  ||   |   ||  O  ||     ||  :  |  |  |    |  |  |     ||  |_ ||    \ |  _  ||   |   |
//  |  | |  |  ||   |   ||     ||     ||     |  |  |    |  |  |     ||     ||  .  \|  |  ||   |   |
// |____||__|__||___|___||_____| \___/  \__,_|  |__|    |__|   \___/ |___,_||__|\_||__|__||___|___|
                                                                                                


#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

class RFIDClient {
private:
    SOCKET clientSocket;
    bool connected;

public:
    RFIDClient() : connected(false) {
        #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
    }

    ~RFIDClient() {
        if (connected) {
            closesocket(clientSocket);
        }
        #ifdef _WIN32
        WSACleanup();
        #endif
    }

    bool connectToServer(const std::string& serverIP = "127.0.0.1", int port = 8080) {
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        
        #ifdef _WIN32
        serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
        #else
        if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid server IP address" << std::endl;
            return false;
        }
        #endif

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error connecting to server" << std::endl;
            return false;
        }

        connected = true;
        std::cout << "Connected to RFID server at " << serverIP << ":" << port << std::endl;
        return true;
    }

    void simulateRFIDScan(const std::string& npm) {
        if (!connected) {
            std::cout << "Not connected to server" << std::endl;
            return;
        }

        std::string message = "SCAN:" + npm;
        send(clientSocket, message.c_str(), message.length(), 0);

        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string response(buffer);
            
            std::cout << "RFID Scan Result for NPM: " << npm << std::endl;
            if (response.substr(0, 5) == "FOUND") {
                size_t pos1 = response.find(':', 5);
                size_t pos2 = response.find(':', pos1 + 1);
                
                std::string name = response.substr(6, pos1 - 6);
                std::string program = response.substr(pos1 + 1);
                
                std::cout << "  Student Found!" << std::endl;
                std::cout << "  Name: " << name << std::endl;
                std::cout << "  Program: " << program << std::endl;
            } else {
                std::cout << "  Student not found in database" << std::endl;
            }
            std::cout << "Scan logged successfully\n" << std::endl;
        }
    }

    void showMenu() {
        std::cout << "\n=== RFID Lab Attendance System ===" << std::endl;
        std::cout << "1. Scan RFID (Enter NPM)" << std::endl;
        std::cout << "2. Exit" << std::endl;
        std::cout << "Choose option (1-2): ";
    }

    void run() {
        if (!connectToServer()) {
            return;
        }

        int choice;
        std::string npm;

        while (true) {
            showMenu();
            std::cin >> choice;

            switch (choice) {
                case 1:
                    std::cout << "Enter NPM: ";
                    std::cin >> npm;
                    simulateRFIDScan(npm);
                    break;
                    
                case 2:
                    std::cout << "Exiting..." << std::endl;
                    closesocket(clientSocket);
                    return;
                    
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
            }
        }
    }
};

int main() {
    std::cout << "=== RFID Lab Attendance Client ===" << std::endl;
    std::cout << "Initializing connection to server..." << std::endl;
    
    RFIDClient client;
    client.run();
    
    return 0;
}