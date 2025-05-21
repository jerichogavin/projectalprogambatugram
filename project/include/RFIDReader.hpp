#ifndef RFIDREADER_HPP
#define RFIDREADER_HPP

#include <string>
#include <chrono>
#include <random>

class RFIDReader {
public:
    std::string id;
    std::string labId;
    bool online;
    std::chrono::system_clock::time_point lastPing;

    RFIDReader(std::string id, std::string labId)
        : id(id), labId(labId), online(true) {
        lastPing = std::chrono::system_clock::now();
    }

    void updateStatus() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        if (dis(gen) < 0.05) { // 5% chance to change status
            online = !online;
        }
        lastPing = std::chrono::system_clock::now();
    }
};

#endif // RFIDREADER_HPP