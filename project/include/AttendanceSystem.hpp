#ifndef ATTENDANCESYSTEM_HPP
#define ATTENDANCESYSTEM_HPP

#include "User.hpp"
#include "Lab.hpp"
#include "RFIDReader.hpp"
#include "ScanLog.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <nlohmann/json.hpp>

class AttendanceSystem {
private:
    std::vector<User> users;
    std::vector<Lab> labs;
    std::vector<RFIDReader> readers;
    std::vector<ScanLog> logs;
    std::condition_variable logCV;
    bool running;
    std::thread loggerThread;
    std::queue<ScanLog> logQueue;
    std::mutex logMutex;
    std::mutex consoleMutex;

    void initializeMockData();
    void processLogQueue();
    void saveToBinaryFile(const ScanLog& log);
    void exportToJson();

public:
    AttendanceSystem();
    ~AttendanceSystem();
    void simulateRFIDScan(std::string rfidId, std::string readerId);
    void displayStatus();
};

#endif // ATTENDANCESYSTEM_HPP