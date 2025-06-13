#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define closesocket close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

struct Student {
    std::string npm;
    std::string name;
    std::string program;
};

struct LogEntry {
    std::string npm;
    std::string name;
    std::string timestamp;
    std::string action;
};

class RFIDServer {
private:
    std::vector<Student> students;
    std::vector<LogEntry> logs;
    std::mutex logMutex;
    SOCKET serverSocket;
    bool running;

public:
    RFIDServer() : running(false) {
        #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        loadDatabase();
    }

    ~RFIDServer() {
        #ifdef _WIN32
        WSACleanup();
        #endif
    }

    void loadDatabase() {
        std::ifstream file("iambouttogram.json");
        if (!file.is_open()) {
            std::cout << "Warning: Could not open iambouttogram.json" << std::endl;
            return;
        }

        std::string line;
        Student student;
        bool inObject = false;
        
        while (std::getline(file, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
            
            if (line == "{") {
                inObject = true;
                student = Student();
            } else if (line == "}," || line == "}") {
                if (inObject && !student.npm.empty()) {
                    students.push_back(student);
                }
                inObject = false;
            } else if (inObject) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(0, colonPos);
                    std::string value = line.substr(colonPos + 1);
                    
                    key.erase(std::remove(key.begin(), key.end(), '"'), key.end());
                    value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
                    value.erase(std::remove(value.begin(), value.end(), ','), value.end());
                    
                    if (key == "npm") student.npm = value;
                    else if (key == "name") student.name = value;
                    else if (key == "program") student.program = value;
                }
            }
        }
        file.close();
        std::cout << "Loaded " << students.size() << " students from database" << std::endl;
    }

    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    void addLog(const std::string& npm, const std::string& action) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::string name = "Unknown";
        for (const auto& student : students) {
            if (student.npm == npm) {
                name = student.name;
                break;
            }
        }
        
        LogEntry entry;
        entry.npm = npm;
        entry.name = name;
        entry.timestamp = getCurrentTime();
        entry.action = action;
        logs.push_back(entry);
        
        std::cout << "[LOG] " << entry.timestamp << " | " << entry.npm 
                  << " | " << entry.name << " | " << entry.action << std::endl;
        
        saveDailyLog();
    }

    void saveDailyLog() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream filename;
        filename << "attendance_" << std::put_time(std::localtime(&time_t), "%Y%m%d") << ".log";
        
        std::ofstream file(filename.str(), std::ios::app);
        if (file.is_open()) {
            if (!logs.empty()) {
                const auto& entry = logs.back();
                file << entry.timestamp << "," << entry.npm << "," 
                     << entry.name << "," << entry.action << std::endl;
            }
            file.close();
        }
    }

    void exportToJSON() {
        std::ofstream file("attendance_export.json");
        if (!file.is_open()) return;
        
        file << "[\n";
        for (size_t i = 0; i < logs.size(); ++i) {
            const auto& entry = logs[i];
            file << "  {\n";
            file << "    \"npm\": \"" << entry.npm << "\",\n";
            file << "    \"name\": \"" << entry.name << "\",\n";
            file << "    \"timestamp\": \"" << entry.timestamp << "\",\n";
            file << "    \"action\": \"" << entry.action << "\"\n";
            file << "  }";
            if (i < logs.size() - 1) file << ",";
            file << "\n";
        }
        file << "]\n";
        file.close();
        std::cout << "Logs exported to attendance_export.json" << std::endl;
    }

    void handleClient(SOCKET clientSocket) {
        char buffer[1024];
        while (running) {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) break;
            
            buffer[bytesRead] = '\0';
            std::string message(buffer);
            
            if (message.substr(0, 5) == "SCAN:") {
                std::string npm = message.substr(5);
                addLog(npm, "SCAN");
                
                std::string response = "OK";
                for (const auto& student : students) {
                    if (student.npm == npm) {
                        response = "FOUND:" + student.name + ":" + student.program;
                        break;
                    }
                }
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        closesocket(clientSocket);
    }

    void start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        #ifndef _WIN32
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        #endif

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(8080);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error binding socket" << std::endl;
            return;
        }

        if (listen(serverSocket, 5) == SOCKET_ERROR) {
            std::cerr << "Error listening on socket" << std::endl;
            return;
        }

        running = true;
        std::cout << "RFID Server started on port 8080" << std::endl;
        std::cout << "Commands: 'export' to export logs, 'quit' to exit" << std::endl;

        std::thread inputThread([this]() {
            std::string command;
            while (running && std::getline(std::cin, command)) {
                if (command == "export") {
                    exportToJSON();
                } else if (command == "quit") {
                    running = false;
                    break;
                }
            }
        });

        while (running) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientSocket != INVALID_SOCKET) {
                std::cout << "Client connected" << std::endl;
                std::thread clientThread(&RFIDServer::handleClient, this, clientSocket);
                clientThread.detach();
            }
        }

        inputThread.join();
        closesocket(serverSocket);
    }
};

int main() {
    RFIDServer server;
    server.start();
    return 0;
}