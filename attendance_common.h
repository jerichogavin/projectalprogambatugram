#ifndef ATTENDANCE_COMMON_H
#define ATTENDANCE_COMMON_H

#include <string>
#include <cstring> // For strncpy

// --- Konfigurasi ---
const char* const BINARY_LOG_FILE = "attendance.dat";
const int STUDENT_ID_MAX_LEN = 20; // Panjang maksimum ID mahasiswa (termasuk null terminator)

// --- Struktur Data Log ---
struct LogEntry {
    char studentID[STUDENT_ID_MAX_LEN];
    long long timestamp; // Unix timestamp (detik sejak epoch)

    // Konstruktor default
    LogEntry() : timestamp(0) {
        studentID[0] = '\0';
    }

    // Konstruktor dengan parameter
    LogEntry(const std::string& id, long long ts) : timestamp(ts) {
        // Salin ID dan pastikan null termination
        strncpy(studentID, id.c_str(), STUDENT_ID_MAX_LEN - 1);
        studentID[STUDENT_ID_MAX_LEN - 1] = '\0';
    }
};

#endif // ATTENDANCE_COMMON_H