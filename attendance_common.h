//   __   __   _  _  ____   __   _  _  ____  ____  __    ___  ____   __   _  _ 
//  (  ) / _\ ( \/ )(  _ \ /  \ / )( \(_  _)(_  _)/  \  / __)(  _ \ / _\ ( \/ )
//   )( /    \/ \/ \ ) _ ((  O )) \/ (  )(    )( (  O )( (_ \ )   //    \/ \/ \
//  (__)\_/\_/\_)(_/(____/ \__/ \____/ (__)  (__) \__/  \___/(__\_)\_/\_/\_)(_/


#ifndef ATTENDANCE_COMMON_H
#define ATTENDANCE_COMMON_H

#include <string>
#include <cstring> // For strncpy

// --- Konfigurasi ---
const char* const BINARY_LOG_FILE = "attendance.dat";
const char* const IPC_PIPE_FILE = "ipc_pipe.txt"; // <-- BARU: File untuk IPC
const int STUDENT_ID_MAX_LEN = 20; 

// ... (sisa struct LogEntry tetap sama) ...
struct LogEntry {
    char studentID[STUDENT_ID_MAX_LEN];
    long long timestamp; // Unix timestamp (detik sejak epoch)

    // Konstruktor default
    LogEntry() : timestamp(0) {
        studentID[0] = '\0';
    }

    // Konstruktor dengan parameter
    LogEntry(const std::string& id, long long ts) : timestamp(ts) {
        strncpy(studentID, id.c_str(), STUDENT_ID_MAX_LEN - 1);
        studentID[STUDENT_ID_MAX_LEN - 1] = '\0';
    }
};

#endif // ATTENDANCE_COMMON_H