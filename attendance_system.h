#ifndef ATTENDANCE_SYSTEM_H
#define ATTENDANCE_SYSTEM_H

#include "attendance_common.h"
#include <vector>
#include <string>
#include <mutex>
#include <fstream>

class AttendanceSystem {
private:
    std::vector<LogEntry> logs;
    mutable std::mutex logMutex;

    std::string getCurrentDateString() const;
    void appendLogToBinary(const LogEntry& entry);

public:
    AttendanceSystem();

    void recordScan(const std::string& studentID, long long timestamp);
    void loadLogsFromBinary();
    std::vector<LogEntry> searchLogsByID(const std::string& studentID_query) const;
    void sortLogsByTime();
    void exportLogsToJSON() const;
    void viewAllLogs() const;
    void processIncomingScansFromFile(); // <-- BARU
};

#endif // ATTENDANCE_SYSTEM_H