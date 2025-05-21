#ifndef SCANLOG_HPP
#define SCANLOG_HPP

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

class ScanLog {
public:
    std::string id;
    std::string userId;
    std::string userName;
    std::string userRole;
    std::string rfidId;
    std::string readerId;
    std::string labId;
    std::string labName;
    std::chrono::system_clock::time_point timestamp;
    bool isEntry;

    ScanLog(std::string id, std::string userId, std::string userName, std::string userRole,
            std::string rfidId, std::string readerId, std::string labId, std::string labName,
            bool isEntry)
        : id(id), userId(userId), userName(userName), userRole(userRole),
          rfidId(rfidId), readerId(readerId), labId(labId), labName(labName),
          isEntry(isEntry) {
        timestamp = std::chrono::system_clock::now();
    }

    std::string getFormattedTimestamp() const {
        auto timeT = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

#endif // SCANLOG_HPP