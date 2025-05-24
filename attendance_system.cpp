#include "attendance_system.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip> // Untuk std::put_time, std::setw, std::setfill
#include <sstream> // Untuk string stream manipulations
#include <ctime>   // Untuk time_t, tm, strftime, localtime
#include <limits>  // Untuk std::numeric_limits
#include <cstring> // For memset, strncpy

// Definisi getCurrentDateString (asumsi tidak berubah dan sudah benar)
std::string AttendanceSystem::getCurrentDateString() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm_buf; 

#ifdef _MSC_VER 
    localtime_s(&now_tm_buf, &now_time_t);
#else 
    std::tm* now_tm_ptr = std::localtime(&now_time_t);
    if (now_tm_ptr) {
        now_tm_buf = *now_tm_ptr;
    } else {
        std::cerr << "Warning [getCurrentDateString]: Gagal mendapatkan waktu lokal. Menggunakan epoch." << std::endl;
        std::time_t epoch_time = 0;
        now_tm_ptr = std::localtime(&epoch_time);
        if (now_tm_ptr) {
            now_tm_buf = *now_tm_ptr;
        } else {
            // Fallback if epoch also fails (highly unlikely)
            memset(&now_tm_buf, 0, sizeof(now_tm_buf)); 
        }
    }
#endif
    std::ostringstream oss;
    oss << std::put_time(&now_tm_buf, "%Y%m%d");
    return oss.str();
}

// Definisi appendLogToBinary (asumsi tidak berubah)
void AttendanceSystem::appendLogToBinary(const LogEntry& entry) {
    std::ofstream outFile(BINARY_LOG_FILE, std::ios::binary | std::ios::app);
    if (!outFile) {
        std::cerr << "Error: Tidak dapat membuka file log biner (" << BINARY_LOG_FILE << ") untuk penambahan." << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(&entry), sizeof(LogEntry));
    outFile.close();
}

// Konstruktor
AttendanceSystem::AttendanceSystem() {
    loadLogsFromBinary();
}

// Definisi recordScan (asumsi tidak berubah)
void AttendanceSystem::recordScan(const std::string& studentID, long long timestamp) {
    if (studentID.length() >= STUDENT_ID_MAX_LEN) {
        std::cerr << "Error: Student ID terlalu panjang: " << studentID << std::endl;
        return;
    }
    LogEntry newEntry(studentID, timestamp);

    std::lock_guard<std::mutex> guard(logMutex);
    logs.push_back(newEntry);
    appendLogToBinary(newEntry);
    std::cout << "[Server] Tercatat: ID=" << newEntry.studentID << ", Timestamp=" << newEntry.timestamp << std::endl;
}

// Definisi loadLogsFromBinary (DIPERBAIKI)
void AttendanceSystem::loadLogsFromBinary() {
    std::ifstream inFile(BINARY_LOG_FILE, std::ios::binary);
    if (!inFile) {
        std::cout << "Info: File log biner (" << BINARY_LOG_FILE << ") tidak ditemukan. Memulai dengan log kosong." << std::endl;
        return;
    }
    
    LogEntry entry;
    std::lock_guard<std::mutex> guard(logMutex);
    logs.clear(); 
    int loadedCount = 0;
    while (inFile.read(reinterpret_cast<char*>(&entry), sizeof(LogEntry))) {
        // PASTIKAN null termination untuk studentID yang dibaca dari file
        entry.studentID[STUDENT_ID_MAX_LEN - 1] = '\0'; 
        
        // Validasi sederhana untuk ID yang dimuat
        if (strlen(entry.studentID) > 0) { // Cukup periksa apakah tidak kosong setelah null term
            logs.push_back(entry);
            loadedCount++;
        } else {
            std::cerr << "Warning: Membaca entri dengan ID kosong atau tidak valid dari file." << std::endl;
        }
    }
    inFile.close();

    if (loadedCount > 0) {
        std::cout << "Info: " << loadedCount << " log dimuat dari file biner (" << BINARY_LOG_FILE << ")." << std::endl;
    } else {
         // Mengecek apakah file memang kosong atau semua entri tidak valid
        inFile.open(BINARY_LOG_FILE, std::ios::binary | std::ios::ate); // Buka lagi untuk cek size
        if (inFile.tellg() == 0) {
            std::cout << "Info: File log biner (" << BINARY_LOG_FILE << ") ada tetapi kosong." << std::endl;
        } else {
            std::cout << "Info: File log biner (" << BINARY_LOG_FILE << ") dibuka, tetapi tidak ada entri valid yang dimuat." << std::endl;
        }
        inFile.close();
    }
}

// Definisi searchLogsByID (asumsi tidak berubah)
std::vector<LogEntry> AttendanceSystem::searchLogsByID(const std::string& studentID_query) const {
    std::vector<LogEntry> results;
    std::lock_guard<std::mutex> guard(logMutex);
    for (const auto& entry : logs) {
        // Pastikan entry.studentID aman untuk strcmp (sudah dihandle di loadLogsFromBinary)
        if (strcmp(entry.studentID, studentID_query.c_str()) == 0) {
            results.push_back(entry);
        }
    }
    return results;
}

// Definisi sortLogsByTime (asumsi tidak berubah)
void AttendanceSystem::sortLogsByTime() {
    std::lock_guard<std::mutex> guard(logMutex);
    std::sort(logs.begin(), logs.end(), [](const LogEntry& a, const LogEntry& b) {
        return a.timestamp < b.timestamp;
    });
    std::cout << "Log telah diurutkan berdasarkan waktu." << std::endl;
}

// Definisi exportLogsToJSON (asumsi tidak berubah)
void AttendanceSystem::exportLogsToJSON() const {
    std::string filename = "attendance_" + getCurrentDateString() + ".json";
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error: Tidak dapat membuka file JSON untuk penulisan: " << filename << std::endl;
        return;
    }

    std::lock_guard<std::mutex> guard(logMutex);
    outFile << "[" << std::endl;
    for (size_t i = 0; i < logs.size(); ++i) {
        outFile << "  {" << std::endl;
        outFile << "    \"studentID\": \"" << logs[i].studentID << "\"," << std::endl;
        outFile << "    \"timestamp\": " << logs[i].timestamp << std::endl;
        outFile << "  }";
        if (i < logs.size() - 1) {
            outFile << ",";
        }
        outFile << std::endl;
    }
    outFile << "]" << std::endl;
    outFile.close();
    std::cout << "Log diekspor ke " << filename << std::endl;
}

// Definisi viewAllLogs (DIPERBAIKI dengan DEBUGGING dan ROBUST TIME HANDLING)
void AttendanceSystem::viewAllLogs() const {
    std::lock_guard<std::mutex> guard(logMutex);
    std::cout << "[Debug] Fungsi viewAllLogs dipanggil. Jumlah log dalam memori: " << logs.size() << std::endl;

    if (logs.empty()) {
        std::cout << "Tidak ada log untuk ditampilkan." << std::endl;
        return;
    }
    std::cout << "\n--- Semua Log Kehadiran ---" << std::endl;
    std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << "ID Mahasiswa"
              << std::setw(20) << "Timestamp (Epoch)"
              << "Waktu (Human-readable)" << std::endl;
    std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30, '-') << std::endl; // Adjust width if needed

    for (const auto& entry : logs) {
        // std::cout << "[Debug] Memproses Log: ID='" << entry.studentID << "', Raw Timestamp=" << entry.timestamp << std::endl;
        
        std::time_t t = entry.timestamp;
        std::tm tm_info_buf;
        char buffer[30]; // Buffer untuk string waktu yang diformat

#ifdef _MSC_VER
        if (localtime_s(&tm_info_buf, &t) != 0) {
            // Error dengan localtime_s
            std::cerr << "[Warning] localtime_s gagal untuk timestamp: " << t << std::endl;
            strncpy(buffer, "Time Conv. Error", sizeof(buffer) - 1);
            buffer[sizeof(buffer)-1] = '\0';
        } else {
            if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info_buf) == 0) {
                std::cerr << "[Warning] strftime gagal memformat waktu (MSVC) untuk timestamp: " << t << std::endl;
                strncpy(buffer, "Time Format Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        }
#else // Untuk kompiler lain (GCC, Clang)
        std::tm* tm_info_ptr = std::localtime(&t);
        if (tm_info_ptr) {
            tm_info_buf = *tm_info_ptr; // Salin hasil karena localtime bisa menggunakan buffer statis
            if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info_buf) == 0) {
                std::cerr << "[Warning] strftime gagal memformat waktu untuk timestamp: " << t << std::endl;
                strncpy(buffer, "Time Format Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        } else {
            // localtime mengembalikan nullptr
            std::cerr << "[Warning] std::localtime gagal untuk timestamp: " << t << ". Mencoba epoch." << std::endl;
            std::time_t epoch_time = 0;
            tm_info_ptr = std::localtime(&epoch_time);
            if (tm_info_ptr) {
                tm_info_buf = *tm_info_ptr;
                if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S (epoch fallback)", &tm_info_buf) == 0) {
                     strncpy(buffer, "Epoch Format Error", sizeof(buffer) - 1);
                     buffer[sizeof(buffer)-1] = '\0';
                }
            } else {
                std::cerr << "[Critical Warning] std::localtime gagal bahkan untuk epoch. Time string akan default." << std::endl;
                strncpy(buffer, "Time System Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        }
#endif
        // Pastikan entry.studentID aman untuk dicetak (sudah dihandle di loadLogsFromBinary)
        std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << entry.studentID
                  << std::setw(20) << entry.timestamp
                  << buffer << std::endl;
    }
    std::cout << "-------------------------------------------------------------" << std::endl; // Adjust width
}