//   __   __   _  _  ____   __   _  _  ____  ____  __    ___  ____   __   _  _ 
//  (  ) / _\ ( \/ )(  _ \ /  \ / )( \(_  _)(_  _)/  \  / __)(  _ \ / _\ ( \/ )
//   )( /    \/ \/ \ ) _ ((  O )) \/ (  )(    )( (  O )( (_ \ )   //    \/ \/ \
//  (__)\_/\_/\_)(_/(____/ \__/ \____/ (__)  (__) \__/  \___/(__\_)\_/\_/\_)(_/

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