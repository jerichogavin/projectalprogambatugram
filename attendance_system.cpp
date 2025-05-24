//   __   __   _  _  ____   __   _  _  ____  ____  __    ___  ____   __   _  _ 
//  (  ) / _\ ( \/ )(  _ \ /  \ / )( \(_  _)(_  _)/  \  / __)(  _ \ / _\ ( \/ )
//   )( /    \/ \/ \ ) _ ((  O )) \/ (  )(    )( (  O )( (_ \ )   //    \/ \/ \
//  (__)\_/\_/\_)(_/(____/ \__/ \____/ (__)  (__) \__/  \___/(__\_)\_/\_/\_)(_/

#include "attendance_system.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip> // For std::put_time, std::setw, std::setfill, std::hex
#include <sstream> // For string stream manipulations
#include <ctime>   // For time_t, tm, strftime, localtime
#include <limits>  // For std::numeric_limits
#include <cstring> // For memset, strncpy

// --- Helper function to escape strings for JSON ---
// Placed here or in an anonymous namespace if only used in this file
namespace { // Anonymous namespace for internal linkage
std::string escapeJsonString(const char* input_cstr) {
    if (!input_cstr) return "";
    std::string input(input_cstr); // Convert C-string to std::string
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b";  break;
            case '\f': oss << "\\f";  break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            default:
                // Output printable characters directly.
                // Control characters (0x00-0x1F) should be escaped as \uXXXX.
                // For simplicity, this basic escape handles common cases.
                // A full JSON library would handle all Unicode escapes.
                if (c >= 0 && c < 32) { // Basic control character check
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}
} // end anonymous namespace


// --- AttendanceSystem Method Implementations ---

// DIPERBAIKI: getCurrentDateString
std::string AttendanceSystem::getCurrentDateString() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm_buf;
    bool time_ok = false;

#ifdef _MSC_VER
    if (localtime_s(&now_tm_buf, &now_time_t) == 0) {
        time_ok = true;
    }
#else
    std::tm* now_tm_ptr = std::localtime(&now_time_t);
    if (now_tm_ptr) {
        now_tm_buf = *now_tm_ptr;
        time_ok = true;
    }
#endif

    if (!time_ok) {
        std::cerr << "Warning [getCurrentDateString]: Gagal mendapatkan waktu lokal. Mencoba epoch." << std::endl;
        std::time_t epoch_time = 0;
#ifdef _MSC_VER
        if (localtime_s(&now_tm_buf, &epoch_time) == 0) {
            time_ok = true;
        }
#else
        std::tm* epoch_tm_ptr = std::localtime(&epoch_time);
        if (epoch_tm_ptr) {
            now_tm_buf = *epoch_tm_ptr;
            time_ok = true;
        }
#endif
    }

    if (!time_ok) {
        std::cerr << "Critical Warning [getCurrentDateString]: Gagal mendapatkan waktu (bahkan epoch). Menggunakan tanggal placeholder 'ERR_DATE'." << std::endl;
        return "ERR_DATE"; // Return a distinct placeholder
    }

    std::ostringstream oss;
    oss << std::put_time(&now_tm_buf, "%Y%m%d");
    std::string date_str = oss.str();

    // Check if put_time produced a reasonably formatted date string
    if (date_str.empty() || date_str.length() < 6) { // e.g. YYYYMMDD is 8, but be lenient
        std::cerr << "Error [getCurrentDateString]: Gagal memformat tanggal dari struct tm. Menggunakan tanggal placeholder 'BADFMT_DATE'." << std::endl;
        return "BADFMT_DATE";
    }
    return date_str;
}

// DIPERBAIKI: appendLogToBinary (robust write check)
void AttendanceSystem::appendLogToBinary(const LogEntry& entry) {
    // Mutex sudah di-lock oleh fungsi pemanggil (recordScan)
    std::ofstream outFile(BINARY_LOG_FILE, std::ios::binary | std::ios::app);
    if (!outFile) {
        std::cerr << "Error: Tidak dapat membuka file log biner ("<< BINARY_LOG_FILE <<") untuk penambahan." << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(&entry), sizeof(LogEntry));
    if (!outFile.good()) { // Periksa setelah operasi tulis
        std::cerr << "Error: Gagal menulis data log ke file biner (" << BINARY_LOG_FILE << "). File mungkin rusak atau disk penuh." << std::endl;
    }
    outFile.close();
}

AttendanceSystem::AttendanceSystem() {
    loadLogsFromBinary();
}

// recordScan - Option 5 related logic
void AttendanceSystem::recordScan(const std::string& studentID, long long timestamp) {
    if (studentID.length() >= STUDENT_ID_MAX_LEN) {
        std::cerr << "Error [recordScan]: Student ID terlalu panjang: " << studentID << std::endl;
        return;
    }
    LogEntry newEntry(studentID, timestamp); // Constructor handles safe copy and null termination

    std::lock_guard<std::mutex> guard(logMutex);
    logs.push_back(newEntry);
    appendLogToBinary(newEntry); // Uses the improved version
    std::cout << "[Server] Tercatat: ID=" << newEntry.studentID << ", Timestamp=" << newEntry.timestamp << std::endl;
}

// loadLogsFromBinary (assuming previous fixes are good)
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
        entry.studentID[STUDENT_ID_MAX_LEN - 1] = '\0'; 
        if (strlen(entry.studentID) > 0) { 
            logs.push_back(entry);
            loadedCount++;
        } else {
            std::cerr << "Warning [loadLogs]: Membaca entri dengan ID kosong atau tidak valid dari file." << std::endl;
        }
    }
    inFile.close();
    if (loadedCount > 0) {
        std::cout << "Info: " << loadedCount << " log dimuat dari file biner (" << BINARY_LOG_FILE << ")." << std::endl;
    } else {
        std::cout << "Info: File log biner (" << BINARY_LOG_FILE << ") dibuka, tetapi tidak ada entri valid yang dimuat atau file kosong." << std::endl;
    }
}

std::vector<LogEntry> AttendanceSystem::searchLogsByID(const std::string& studentID_query) const {
    std::vector<LogEntry> results;
    std::lock_guard<std::mutex> guard(logMutex);
    for (const auto& entry : logs) {
        if (strcmp(entry.studentID, studentID_query.c_str()) == 0) {
            results.push_back(entry);
        }
    }
    return results;
}

void AttendanceSystem::sortLogsByTime() {
    std::lock_guard<std::mutex> guard(logMutex);
    std::sort(logs.begin(), logs.end(), [](const LogEntry& a, const LogEntry& b) {
        return a.timestamp < b.timestamp;
    });
    std::cout << "Log telah diurutkan berdasarkan waktu." << std::endl;
}

// DIPERBAIKI: exportLogsToJSON (uses escapeJsonString) - Option 4 related logic
void AttendanceSystem::exportLogsToJSON() const {
    std::string datePart = getCurrentDateString();
    if (datePart.empty() || datePart.find("ERR") != std::string::npos || datePart.find("BADFMT") != std::string::npos) {
        std::cerr << "Error [exportLogsToJSON]: Gagal mendapatkan tanggal valid untuk nama file. Menggunakan 'export_error_date.json'." << std::endl;
        datePart = "export_error_date";
    }
    std::string filename = "attendance_" + datePart + ".json";
    std::ofstream outFile(filename);

    if (!outFile) {
        std::cerr << "Error: Tidak dapat membuka file JSON untuk penulisan: " << filename << std::endl;
        return;
    }

    std::lock_guard<std::mutex> guard(logMutex);
    outFile << "[" << std::endl;
    for (size_t i = 0; i < logs.size(); ++i) {
        outFile << "  {" << std::endl;
        // Gunakan escapeJsonString untuk studentID
        outFile << "    \"studentID\": \"" << escapeJsonString(logs[i].studentID) << "\"," << std::endl;
        outFile << "    \"timestamp\": " << logs[i].timestamp << std::endl;
        outFile << "  }";
        if (i < logs.size() - 1) {
            outFile << ",";
        }
        outFile << std::endl;
    }
    outFile << "]" << std::endl;
    outFile.close();
    if(outFile.fail()){ // Check error state after closing
        std::cerr << "Error: Terjadi masalah saat menulis atau menutup file JSON: " << filename << std::endl;
    } else {
        std::cout << "Log diekspor ke " << filename << std::endl;
    }
}

// viewAllLogs (assuming previous fixes for time formatting are good)
void AttendanceSystem::viewAllLogs() const {
    std::lock_guard<std::mutex> guard(logMutex);
    // std::cout << "[Debug] Fungsi viewAllLogs dipanggil. Jumlah log dalam memori: " << logs.size() << std::endl; // Uncomment for debug

    if (logs.empty()) {
        std::cout << "Tidak ada log untuk ditampilkan." << std::endl;
        return;
    }
    std::cout << "\n--- Semua Log Kehadiran ---" << std::endl;
    std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << "ID Mahasiswa"
              << std::setw(20) << "Timestamp (Epoch)"
              << "Waktu (Human-readable)" << std::endl;
    std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30 + 5, '-') << std::endl; // Adjusted width slightly

    for (const auto& entry : logs) {
        std::time_t t = entry.timestamp;
        std::tm tm_info_buf;
        char buffer[35]; // Buffer for formatted time string, increased size a bit

#ifdef _MSC_VER
        if (localtime_s(&tm_info_buf, &t) != 0) {
            std::cerr << "[Warning] localtime_s gagal untuk timestamp: " << t << std::endl;
            strncpy(buffer, "Time Conv. Error", sizeof(buffer) - 1);
            buffer[sizeof(buffer)-1] = '\0';
        } else {
            if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info_buf) == 0) {
                strncpy(buffer, "Time Format Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        }
#else 
        std::tm* tm_info_ptr = std::localtime(&t);
        if (tm_info_ptr) {
            tm_info_buf = *tm_info_ptr; 
            if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info_buf) == 0) {
                strncpy(buffer, "Time Format Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        } else {
            std::time_t epoch_time = 0;
            tm_info_ptr = std::localtime(&epoch_time);
            if (tm_info_ptr) {
                tm_info_buf = *tm_info_ptr;
                // Indicate fallback in output
                if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S (epoch)", &tm_info_buf) == 0) {
                     strncpy(buffer, "Epoch Format Error", sizeof(buffer) - 1);
                     buffer[sizeof(buffer)-1] = '\0';
                }
            } else {
                strncpy(buffer, "Time System Error", sizeof(buffer) - 1);
                buffer[sizeof(buffer)-1] = '\0';
            }
        }
#endif
        std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << entry.studentID
                  << std::setw(20) << entry.timestamp
                  << buffer << std::endl;
    }
    std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30 + 5, '-') << std::endl;
}

// processIncomingScansFromFile (assuming previous version is mostly okay, ensure includes are present)
void AttendanceSystem::processIncomingScansFromFile() {
    std::cout << "[Server] Memproses scan masuk dari file " << IPC_PIPE_FILE << "..." << std::endl;
    std::ifstream pipeFile(IPC_PIPE_FILE);
    if (!pipeFile.is_open()) {
        std::cout << "[Server] Info: Tidak ada file pipe (" << IPC_PIPE_FILE << ") ditemukan atau tidak bisa dibuka." << std::endl;
        return;
    }

    std::string line;
    int processedCount = 0;
    int failedCount = 0;
    std::vector<std::string> tempLines; // Store all lines first to avoid issues with modifying file while reading
    while (std::getline(pipeFile, line)) {
        tempLines.push_back(line);
    }
    pipeFile.close(); // Close before processing and potentially rewriting/truncating

    if (tempLines.empty()){
        std::cout << "[Server] Info: File pipe (" << IPC_PIPE_FILE << ") kosong." << std::endl;
        // No need to truncate if already empty and closed.
        return;
    }

    for (const std::string& currentLine : tempLines) {
        if (currentLine.empty()) continue;

        std::istringstream iss(currentLine);
        std::string studentID_str;
        std::string timestamp_str;
        long long timestamp_val;

        if (std::getline(iss, studentID_str, ',') && std::getline(iss, timestamp_str)) {
            try {
                timestamp_val = std::stoll(timestamp_str); // Use different variable names
                if (studentID_str.length() < STUDENT_ID_MAX_LEN && !studentID_str.empty()) {
                    recordScan(studentID_str, timestamp_val); 
                    processedCount++;
                } else {
                    std::cerr << "[Server] Error parsing: Student ID tidak valid atau terlalu panjang dari baris: " << currentLine << std::endl;
                    failedCount++;
                }
            } catch (const std::invalid_argument& ia) {
                std::cerr << "[Server] Error parsing: Timestamp tidak valid dari baris: " << currentLine << " (" << ia.what() << ")" << std::endl;
                failedCount++;
            } catch (const std::out_of_range& oor) {
                std::cerr << "[Server] Error parsing: Timestamp diluar jangkauan dari baris: " << currentLine << " (" << oor.what() << ")" << std::endl;
                failedCount++;
            }
        } else {
            std::cerr << "[Server] Error parsing: Format baris tidak sesuai (StudentID,Timestamp) pada baris: " << currentLine << std::endl;
            failedCount++;
        }
    }
    
    // Clear the IPC file after attempting to process all lines from it
    std::ofstream clearPipeFile(IPC_PIPE_FILE, std::ios::trunc); 
    if (!clearPipeFile.is_open()) {
         std::cerr << "[Server] Warning: Gagal mengosongkan file " << IPC_PIPE_FILE << " setelah diproses." << std::endl;
    }
    clearPipeFile.close();

    if (processedCount > 0) {
        std::cout << "[Server] Selesai memproses. " << processedCount << " scan berhasil dicatat." << std::endl;
    }
    if (failedCount > 0) {
        std::cout << "[Server] " << failedCount << " scan gagal diproses dari file pipe. File pipe telah dikosongkan." << std::endl;
    }
     if (processedCount == 0 && failedCount == 0 && !tempLines.empty()){
        // This case should ideally not happen if tempLines was not empty,
        // unless all lines were empty strings, which is handled by 'continue'.
        std::cout << "[Server] Info: Tidak ada data valid yang ditemukan di file pipe meskipun file tidak kosong. File pipe telah dikosongkan." << std::endl;
    }
}