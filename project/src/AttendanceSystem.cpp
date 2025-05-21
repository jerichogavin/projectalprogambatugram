#include "../include/AttendanceSystem.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

AttendanceSystem::AttendanceSystem() : running(true) {
    initializeMockData();
    loggerThread = std::thread(&AttendanceSystem::processLogQueue, this);
}

AttendanceSystem::~AttendanceSystem() {
    running = false;
    logCV.notify_all();
    if (loggerThread.joinable()) {
        loggerThread.join();
    }
}

void AttendanceSystem::initializeMockData() {
    users = {
        User("1", "John Doe", "student", "RF001"),
        User("2", "Jane Smith", "student", "RF002"),
        User("3", "Dr. Robert Brown", "staff", "RF003"),
        User("4", "Admin User", "admin", "RF004")
    };

    labs = {
        Lab("1", "Computer Science Lab", "Building A, Floor 2", 30),
        Lab("2", "Biomedical Lab", "Building B, Floor 1", 25)
    };

    readers = {
        RFIDReader("1", "1"),
        RFIDReader("2", "1"),
        RFIDReader("3", "2")
    };
}

void AttendanceSystem::simulateRFIDScan(std::string rfidId, std::string readerId) {
    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    
    auto user = std::find_if(users.begin(), users.end(),
        [&](const User& u) { return u.rfidId == rfidId; });
    
    auto reader = std::find_if(readers.begin(), readers.end(),
        [&](const RFIDReader& r) { return r.id == readerId; });

    if (user == users.end() || reader == readers.end() || !reader->online) {
        std::cout << "Invalid scan or reader offline" << std::endl;
        return;
    }

    auto lab = std::find_if(labs.begin(), labs.end(),
        [&](const Lab& l) { return l.id == reader->labId; });

    if (lab == labs.end()) {
        std::cout << "Lab not found" << std::endl;
        return;
    }

    bool isEntry = true;
    if (!logs.empty()) {
        auto lastLog = std::find_if(logs.rbegin(), logs.rend(),
            [&](const ScanLog& log) { return log.userId == user->id; });
        if (lastLog != logs.rend()) {
            isEntry = !lastLog->isEntry;
        }
    }

    std::string logId = "log-" + std::to_string(logs.size() + 1);
    ScanLog newLog(logId, user->id, user->name, user->role,
                  user->rfidId, reader->id, lab->id, lab->name, isEntry);

    if (isEntry) {
        lab->currentOccupancy++;
    } else {
        lab->currentOccupancy--;
    }

    {
        std::lock_guard<std::mutex> lock(logMutex);
        logQueue.push(newLog);
    }
    logCV.notify_one();

    std::cout << user->name << (isEntry ? " entered " : " exited ") 
              << lab->name << " at " << newLog.getFormattedTimestamp() << std::endl;
}

void AttendanceSystem::processLogQueue() {
    while (running) {
        std::unique_lock<std::mutex> lock(logMutex);
        logCV.wait(lock, [this] { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            ScanLog log = logQueue.front();
            logQueue.pop();
            logs.push_back(log);
            saveToBinaryFile(log);
            exportToJson();
        }
    }
}

void AttendanceSystem::saveToBinaryFile(const ScanLog& log) {
    std::ofstream file("logs.bin", std::ios::binary | std::ios::app);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&log), sizeof(ScanLog));
        file.close();
    }
}

void AttendanceSystem::exportToJson() {
    json j = json::array();
    for (const auto& log : logs) {
        j.push_back({
            {"id", log.id},
            {"userId", log.userId},
            {"userName", log.userName},
            {"userRole", log.userRole},
            {"rfidId", log.rfidId},
            {"readerId", log.readerId},
            {"labId", log.labId},
            {"labName", log.labName},
            {"timestamp", log.getFormattedTimestamp()},
            {"isEntry", log.isEntry}
        });
    }

    std::ofstream file("logs.json");
    file << std::setw(2) << j << std::endl;
}

void AttendanceSystem::displayStatus() {
    std::lock_guard<std::mutex> consoleLock(consoleMutex);
    std::cout << "\n=== System Status ===" << std::endl;
    
    std::cout << "\nLab Occupancy:" << std::endl;
    for (const auto& lab : labs) {
        std::cout << lab.name << ": " << lab.currentOccupancy 
                 << "/" << lab.capacity << std::endl;
    }

    std::cout << "\nReader Status:" << std::endl;
    for (auto& reader : readers) {
        reader.updateStatus();
        std::cout << "Reader " << reader.id << " (" 
                 << (reader.online ? "Online" : "Offline") << ")" << std::endl;
    }

    std::cout << "\nRecent Scans:" << std::endl;
    int count = 0;
    for (auto it = logs.rbegin(); it != logs.rend() && count < 5; ++it, ++count) {
        std::cout << it->userName << (it->isEntry ? " entered " : " exited ")
                 << it->labName << " at " << it->getFormattedTimestamp() << std::endl;
    }
    std::cout << std::endl;
}