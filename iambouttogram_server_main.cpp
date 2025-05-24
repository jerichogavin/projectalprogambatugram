#include <iostream>
#include <vector>
#include <string>
// #include <thread> // No longer needed for client simulation here
#include <chrono>
#include <limits>
#include <iomanip>
#include <ctime>

#include "attendance_system.h" // Core server logic
#include "attendance_common.h" // For LogEntry if needed directly, and constants

int main() {
    AttendanceSystem attendanceSystem; // Initialize the server's attendance system

    std::cout << "iambouttogram_server started." << std::endl;
    std::cout << "Listening for client connections (conceptual)..." << std::endl;
    // In a real application, server listening logic (e.g., sockets) would go here.
    // The recordScan method would be called when data is received from a client.

    // --- Menu Interaktif untuk Admin ---
    char choice;
    std::string userInputID; 
    do {
        std::cout << "\n--- iambouttogram_server Admin Menu ---" << std::endl;
        std::cout << "1. Lihat Semua Log" << std::endl;
        std::cout << "2. Cari Log berdasarkan ID" << std::endl;
        std::cout << "3. Urutkan Log berdasarkan Waktu" << std::endl;
        std::cout << "4. Ekspor Log ke JSON" << std::endl;
        std::cout << "5. Simulasikan Scan Baru (Manual Server Input)" << std::endl;
        std::cout << "0. Keluar" << std::endl;
        std::cout << "Masukkan pilihan Anda: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        switch (choice) {
            case '1':
                attendanceSystem.viewAllLogs();
                break;
            case '2':
                std::cout << "Masukkan ID Mahasiswa/Staf untuk dicari: ";
                std::getline(std::cin, userInputID);
                {
                    std::vector<LogEntry> results = attendanceSystem.searchLogsByID(userInputID);
                    if (results.empty()) {
                        std::cout << "Tidak ada log ditemukan untuk ID: " << userInputID << std::endl;
                    } else {
                        std::cout << "Ditemukan " << results.size() << " log untuk ID: " << userInputID << std::endl;
                        for (const auto& entry : results) {
                            std::time_t t = entry.timestamp;
                            std::tm tm_info_buf_search;
#ifdef _MSC_VER
                            localtime_s(&tm_info_buf_search, &t);
#else
                            std::tm* tm_info_ptr_search = std::localtime(&t);
                            if (tm_info_ptr_search) {
                                tm_info_buf_search = *tm_info_ptr_search;
                            } else {
                                std::time_t epoch_time = 0;
                                tm_info_ptr_search = std::localtime(&epoch_time);
                                if(tm_info_ptr_search) tm_info_buf_search = *tm_info_ptr_search;
                            }
#endif
                            std::tm& tm_info_search = tm_info_buf_search;
                            char buffer_search[30];
                            std::strftime(buffer_search, sizeof(buffer_search), "%Y-%m-%d %H:%M:%S", &tm_info_search);
                            std::cout << "  ID: " << entry.studentID << ", Timestamp: " << entry.timestamp 
                                      << " (" << buffer_search << ")" << std::endl;
                        }
                    }
                }
                break;
            case '3':
                attendanceSystem.sortLogsByTime();
                attendanceSystem.viewAllLogs(); 
                break;
            case '4':
                attendanceSystem.exportLogsToJSON();
                break;
            case '5': // Manual scan entry at the server side
                std::cout << "Masukkan ID Mahasiswa/Staf untuk scan baru: ";
                std::getline(std::cin, userInputID);
                if (!userInputID.empty()) {
                    if (userInputID.length() >= STUDENT_ID_MAX_LEN) {
                        std::cout << "Error: ID terlalu panjang. Maksimum " 
                                  << STUDENT_ID_MAX_LEN - 1 << " karakter." << std::endl;
                    } else {
                        long long ts = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                        std::cout << "Menggunakan timestamp saat ini: " << ts << std::endl;
                        // Directly record scan as if it's a server-side administrative action
                        attendanceSystem.recordScan(userInputID, ts);
                    }
                }
                break;
            case '0':
                std::cout << "Menutup iambouttogram_server." << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid. Silakan coba lagi." << std::endl;
        }
    } while (choice != '0');
    
    return 0;
}