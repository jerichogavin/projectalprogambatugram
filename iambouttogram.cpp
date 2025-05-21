#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip> // Untuk std::put_time, std::setw, std::setfill
#include <sstream> // Untuk string stream manipulations
#include <ctime>   // Untuk time_t, tm, strftime, localtime
#include <cstring> // <<-- DITAMBAHKAN: Untuk strncpy, strlen, strcmp
#include <limits>  // Untuk std::numeric_limits

// --- Konfigurasi ---
const char* BINARY_LOG_FILE = "attendance.dat";
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

// --- Kelas Sistem Absensi (Logika Server) ---
class AttendanceSystem {
private:
    std::vector<LogEntry> logs;
    mutable std::mutex logMutex; // mutable agar bisa di-lock di const methods

    // Helper untuk mendapatkan tanggal saat ini dalam format YYYYMMDD
    std::string getCurrentDateString() const {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        #ifdef _MSC_VER // Untuk kompiler MSVC
            std::tm now_tm;
            localtime_s(&now_tm, &now_time_t);
        #else // Untuk kompiler lain (GCC, Clang)
            std::tm now_tm_buf; // Buffer untuk hasil localtime
            std::tm* now_tm_ptr = std::localtime(&now_time_t);
            if (now_tm_ptr) { // Periksa apakah localtime mengembalikan pointer yang valid
                now_tm_buf = *now_tm_ptr;
            } else {
                // Handle error, misalnya dengan mengatur ke epoch atau tanggal default
                std::cerr << "Warning: Gagal mendapatkan waktu lokal. Menggunakan epoch." << std::endl;
                std::time_t epoch_time = 0;
                now_tm_ptr = std::localtime(&epoch_time);
                 if (now_tm_ptr) {
                    now_tm_buf = *now_tm_ptr;
                }
                // Jika masih gagal, ini adalah masalah yang lebih serius
            }
            std::tm& now_tm = now_tm_buf; // Gunakan referensi ke buffer
        #endif
        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%Y%m%d");
        return oss.str();
    }

    // Menyimpan satu entri log ke file biner (mode append)
    void appendLogToBinary(const LogEntry& entry) {
        // Mutex sudah di-lock oleh fungsi pemanggil (recordScan)
        std::ofstream outFile(BINARY_LOG_FILE, std::ios::binary | std::ios::app);
        if (!outFile) {
            std::cerr << "Error: Tidak dapat membuka file log biner untuk penambahan." << std::endl;
            return;
        }
        outFile.write(reinterpret_cast<const char*>(&entry), sizeof(LogEntry));
        outFile.close();
    }

public:
    AttendanceSystem() {
        loadLogsFromBinary();
    }

    // Merekam scan baru (ID mahasiswa dan timestamp dari client)
    void recordScan(const std::string& studentID, long long timestamp) {
        if (studentID.length() >= STUDENT_ID_MAX_LEN) {
            std::cerr << "Error: Student ID terlalu panjang: " << studentID << std::endl;
            return;
        }
        LogEntry newEntry(studentID, timestamp);

        std::lock_guard<std::mutex> guard(logMutex);
        logs.push_back(newEntry);
        appendLogToBinary(newEntry); // Tambahkan ke file biner
        std::cout << "[Server] Tercatat: ID=" << newEntry.studentID << ", Timestamp=" << newEntry.timestamp << std::endl;
    }
    
    // Memuat semua log dari file biner saat startup
    void loadLogsFromBinary() {
        std::ifstream inFile(BINARY_LOG_FILE, std::ios::binary);
        if (!inFile) {
            std::cout << "Info: File log biner tidak ditemukan atau kosong. Memulai baru." << std::endl;
            return;
        }
        
        LogEntry entry;
        std::lock_guard<std::mutex> guard(logMutex);
        logs.clear(); // Bersihkan log di memori sebelum memuat
        while (inFile.read(reinterpret_cast<char*>(&entry), sizeof(LogEntry))) {
            // Validasi sederhana untuk ID yang dimuat
            if (strlen(entry.studentID) > 0 && strlen(entry.studentID) < STUDENT_ID_MAX_LEN) {
                 logs.push_back(entry);
            }
        }
        inFile.close();
        std::cout << "Info: " << logs.size() << " log dimuat dari file biner." << std::endl;
    }

    // Mencari log berdasarkan ID mahasiswa
    std::vector<LogEntry> searchLogsByID(const std::string& studentID_query) const {
        std::vector<LogEntry> results;
        std::lock_guard<std::mutex> guard(logMutex);
        for (const auto& entry : logs) {
            if (strcmp(entry.studentID, studentID_query.c_str()) == 0) {
                results.push_back(entry);
            }
        }
        return results;
    }

    // Mengurutkan log berdasarkan timestamp
    void sortLogsByTime() {
        std::lock_guard<std::mutex> guard(logMutex);
        std::sort(logs.begin(), logs.end(), [](const LogEntry& a, const LogEntry& b) {
            return a.timestamp < b.timestamp;
        });
        std::cout << "Log telah diurutkan berdasarkan waktu." << std::endl;
    }

    // Mengekspor semua log saat ini ke file JSON
    void exportLogsToJSON() const {
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
    
    // Menampilkan semua log ke konsol
    void viewAllLogs() const {
        std::lock_guard<std::mutex> guard(logMutex);
        if (logs.empty()) {
            std::cout << "Tidak ada log untuk ditampilkan." << std::endl;
            return;
        }
        std::cout << "\n--- Semua Log Kehadiran ---" << std::endl;
        std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << "ID Mahasiswa"
                  << std::setw(20) << "Timestamp (Epoch)"
                  << "Waktu (Human-readable)" << std::endl;
        std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30, '-') << std::endl;

        for (const auto& entry : logs) {
            std::time_t t = entry.timestamp;
            #ifdef _MSC_VER
                std::tm tm_info;
                localtime_s(&tm_info, &t);
            #else
                std::tm tm_info_buf;
                std::tm* tm_info_ptr = std::localtime(&t);
                if (tm_info_ptr) {
                    tm_info_buf = *tm_info_ptr;
                } else {
                     // Atur ke nilai default atau log error
                    std::time_t epoch_time = 0;
                    tm_info_ptr = std::localtime(&epoch_time);
                    if (tm_info_ptr) tm_info_buf = *tm_info_ptr;
                }
                std::tm& tm_info = tm_info_buf;
            #endif
            char buffer[30];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info);

            std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << entry.studentID
                      << std::setw(20) << entry.timestamp
                      << buffer << std::endl;
        }
        std::cout << "-------------------------------------------------------" << std::endl;
    }
};

// --- Simulasi Client RFID ---
void simulateRFIDClient(AttendanceSystem& system, const std::string& studentID, int numScans, int delayMs) {
    for (int i = 0; i < numScans; ++i) {
        // Client menghasilkan ID dan timestamp
        long long currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
        std::cout << "[Client " << studentID << "] Mensimulasikan scan... ID: " << studentID << ", Timestamp: " << currentTimestamp << std::endl;
        system.recordScan(studentID, currentTimestamp); // "Mengirim" scan ke sistem
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

// --- Aplikasi Utama ---
int main() {
    AttendanceSystem attendanceSystem;

    // --- Mensimulasikan beberapa client melakukan scan secara bersamaan ---
    std::cout << "Memulai simulasi client..." << std::endl;
    std::thread client1(simulateRFIDClient, std::ref(attendanceSystem), "MHS001", 3, 100); // 3 scan, delay 100ms
    std::thread client2(simulateRFIDClient, std::ref(attendanceSystem), "STF007", 2, 150); // 2 scan, delay 150ms
    std::thread client3(simulateRFIDClient, std::ref(attendanceSystem), "MHS001", 2, 120); // MHS001 scan lagi

    client1.join();
    client2.join();
    client3.join();
    std::cout << "Simulasi client selesai.\n" << std::endl;

    // --- Menu Interaktif untuk Admin ---
    char choice;
    std::string searchID;
    do {
        std::cout << "\n--- Menu Admin ---" << std::endl;
        std::cout << "1. Lihat Semua Log" << std::endl;
        std::cout << "2. Cari Log berdasarkan ID" << std::endl;
        std::cout << "3. Urutkan Log berdasarkan Waktu" << std::endl;
        std::cout << "4. Ekspor Log ke JSON" << std::endl;
        std::cout << "5. Simulasikan Scan Baru (Manual)" << std::endl;
        std::cout << "0. Keluar" << std::endl;
        std::cout << "Masukkan pilihan Anda: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Konsumsi newline

        switch (choice) {
            case '1':
                attendanceSystem.viewAllLogs();
                break;
            case '2':
                std::cout << "Masukkan ID Mahasiswa/Staf untuk dicari: ";
                std::getline(std::cin, searchID);
                {
                    std::vector<LogEntry> results = attendanceSystem.searchLogsByID(searchID);
                    if (results.empty()) {
                        std::cout << "Tidak ada log ditemukan untuk ID: " << searchID << std::endl;
                    } else {
                        std::cout << "Ditemukan " << results.size() << " log untuk ID: " << searchID << std::endl;
                        for (const auto& entry : results) {
                             std::time_t t = entry.timestamp;
                            #ifdef _MSC_VER
                                std::tm tm_info;
                                localtime_s(&tm_info, &t);
                            #else
                                std::tm tm_info_buf;
                                std::tm* tm_info_ptr = std::localtime(&t);
                                if (tm_info_ptr) {
                                    tm_info_buf = *tm_info_ptr;
                                } else {
                                    std::time_t epoch_time = 0;
                                    tm_info_ptr = std::localtime(&epoch_time);
                                    if (tm_info_ptr) tm_info_buf = *tm_info_ptr;
                                }
                                std::tm& tm_info = tm_info_buf;
                            #endif
                             char buffer[30];
                             std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
                            std::cout << "  ID: " << entry.studentID << ", Timestamp: " << entry.timestamp << " (" << buffer << ")" << std::endl;
                        }
                    }
                }
                break;
            case '3':
                attendanceSystem.sortLogsByTime();
                attendanceSystem.viewAllLogs(); // Tampilkan log yang sudah diurutkan
                break;
            case '4':
                attendanceSystem.exportLogsToJSON();
                break;
            case '5':
                std::cout << "Masukkan ID Mahasiswa/Staf untuk scan baru: ";
                std::getline(std::cin, searchID);
                if (!searchID.empty()) {
                    if (searchID.length() >= STUDENT_ID_MAX_LEN) {
                        std::cout << "Error: ID terlalu panjang. Maksimum " << STUDENT_ID_MAX_LEN -1 << " karakter." << std::endl;
                    } else {
                        long long ts = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                        std::cout << "Menggunakan timestamp saat ini: " << ts << std::endl;
                        attendanceSystem.recordScan(searchID, ts);
                    }
                }
                break;
            case '0':
                std::cout << "Keluar dari sistem." << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid. Silakan coba lagi." << std::endl;
        }
    } while (choice != '0');
    
    return 0;
}