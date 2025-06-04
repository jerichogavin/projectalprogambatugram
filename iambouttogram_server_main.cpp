//   __   __   _  _  ____   __   _  _  ____  ____  __    ___  ____   __   _  _ 
//  (  ) / _\ ( \/ )(  _ \ /  \ / )( \(_  _)(_  _)/  \  / __)(  _ \ / _\ ( \/ )
//   )( /    \/ \/ \ ) _ ((  O )) \/ (  )(    )( (  O )( (_ \ )   //    \/ \/ \
//  (__)\_/\_/\_)(_/(____/ \__/ \____/ (__)  (__) \__/  \___/(__\_)\_/\_/\_)(_/

#include <iostream>
#include <vector>
#include <string>
#include <chrono>    // For Option 5 timestamp
#include <limits>    // For std::numeric_limits
#include <iomanip>   // For std::setw, std::left
#include <ctime>     // For time_t, tm, strftime, localtime
#include <cstring>   // For strncpy, memset (in case of time format errors)

#include "attendance_system.h" // The core attendance logic
#include "attendance_common.h" // For LogEntry struct and STUDENT_ID_MAX_LEN

int main() {
    AttendanceSystem attendanceSystem; // Create an instance of the attendance system
    std::cout << "iambouttogram_server started." << std::endl;
    // std::cout << "Listening for client connections (conceptual)..." << std::endl; // Optional conceptual message

    char choice;
    std::string userInputID; // Used for various ID inputs

    do {
        std::cout << "\n--- iambouttogram_server Admin Menu ---" << std::endl;
        std::cout << "1. Lihat Semua Log" << std::endl;
        std::cout << "2. Cari Log berdasarkan ID" << std::endl;
        std::cout << "3. Urutkan Log berdasarkan Waktu" << std::endl;
        std::cout << "4. Ekspor Log ke JSON" << std::endl;
        std::cout << "5. Simulasikan Scan Baru (Manual Server Input)" << std::endl;
        std::cout << "6. Proses Scan Masuk dari Klien" << std::endl;
        std::cout << "0. Keluar" << std::endl;
        std::cout << "Masukkan pilihan Anda: ";

        if (!(std::cin >> choice)) { // Basic input error check
            std::cout << "Input error. Silakan coba lagi." << std::endl;
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the rest of the line

        switch (choice) {
            case '1':
                attendanceSystem.viewAllLogs();
                break;
            case '2': // Cari Log berdasarkan ID
                std::cout << "Masukkan ID Mahasiswa/Staf untuk dicari: ";
                if (!std::getline(std::cin, userInputID)) {
                    std::cout << "Input error saat membaca ID. Kembali ke menu." << std::endl;
                    break;
                }
                if (userInputID.empty()){
                    std::cout << "ID pencarian tidak boleh kosong." << std::endl;
                    break;
                }
                { // Scope for results vector
                    std::vector<LogEntry> results = attendanceSystem.searchLogsByID(userInputID);
                    if (results.empty()) {
                        std::cout << "Tidak ada log ditemukan untuk ID: " << userInputID << std::endl;
                    } else {
                        std::cout << "Ditemukan " << results.size() << " log untuk ID: " << userInputID << std::endl;
                        // Display headers for search results
                        std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << "ID Mahasiswa"
                                  << std::setw(20) << "Timestamp (Epoch)"
                                  << "Waktu (Human-readable)" << std::endl;
                        std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30 + 5, '-') << std::endl;

                        for (const auto& entry : results) {
                            std::time_t t = entry.timestamp;
                            std::tm tm_info_buf_search; // Buffer for tm struct
                            char buffer_search[35];   // Buffer for formatted time string

#ifdef _MSC_VER
                            if (localtime_s(&tm_info_buf_search, &t) != 0) {
                                // std::cerr << "[Warning] Case 2: localtime_s gagal untuk timestamp: " << t << std::endl;
                                strncpy(buffer_search, "Time Conv. Error", sizeof(buffer_search) - 1);
                                buffer_search[sizeof(buffer_search) - 1] = '\0';
                            } else {
                                if (std::strftime(buffer_search, sizeof(buffer_search), "%Y-%m-%d %H:%M:%S", &tm_info_buf_search) == 0) {
                                    // std::cerr << "[Warning] Case 2: strftime gagal (MSVC) untuk timestamp: " << t << std::endl;
                                    strncpy(buffer_search, "Time Format Error", sizeof(buffer_search) - 1);
                                    buffer_search[sizeof(buffer_search) - 1] = '\0';
                                }
                            }
#else // For other compilers (GCC, Clang)
                            std::tm* tm_info_ptr_search = std::localtime(&t);
                            if (tm_info_ptr_search) {
                                tm_info_buf_search = *tm_info_ptr_search; // Copy result
                                if (std::strftime(buffer_search, sizeof(buffer_search), "%Y-%m-%d %H:%M:%S", &tm_info_buf_search) == 0) {
                                    // std::cerr << "[Warning] Case 2: strftime gagal untuk timestamp: " << t << std::endl;
                                    strncpy(buffer_search, "Time Format Error", sizeof(buffer_search) - 1);
                                    buffer_search[sizeof(buffer_search) - 1] = '\0';
                                }
                            } else { // localtime returned nullptr for the original timestamp
                                // std::cerr << "[Warning] Case 2: std::localtime gagal untuk timestamp: " << t << ". Mencoba epoch." << std::endl;
                                std::time_t epoch_time_search = 0;
                                tm_info_ptr_search = std::localtime(&epoch_time_search);
                                if (tm_info_ptr_search) {
                                    tm_info_buf_search = *tm_info_ptr_search;
                                    if (std::strftime(buffer_search, sizeof(buffer_search), "%Y-%m-%d %H:%M:%S (epoch)", &tm_info_buf_search) == 0) {
                                         strncpy(buffer_search, "Epoch Format Error", sizeof(buffer_search) - 1);
                                         buffer_search[sizeof(buffer_search) - 1] = '\0';
                                    }
                                } else {
                                    strncpy(buffer_search, "Time System Error", sizeof(buffer_search) - 1);
                                    buffer_search[sizeof(buffer_search) - 1] = '\0';
                                }
                            }
#endif
                            std::cout << std::left << std::setw(STUDENT_ID_MAX_LEN) << entry.studentID
                                      << std::setw(20) << entry.timestamp
                                      << buffer_search << std::endl;
                        }
                        std::cout << std::string(STUDENT_ID_MAX_LEN + 20 + 30 + 5, '-') << std::endl;
                    }
                } // End scope for results vector
                break;
            case '3':
                attendanceSystem.sortLogsByTime();
                attendanceSystem.viewAllLogs(); // Show sorted logs immediately
                break;
            case '4':
                attendanceSystem.exportLogsToJSON();
                break;
            case '5': // Simulasikan Scan Baru (Manual Server Input)
                std::cout << "Masukkan ID Mahasiswa/Staf untuk scan baru: ";
                if (!std::getline(std::cin, userInputID)) {
                     std::cout << "Input error saat membaca ID. Kembali ke menu." << std::endl;
                    break;
                }

                if (!userInputID.empty()) {
                    // STUDENT_ID_MAX_LEN includes the null terminator for the char array in LogEntry
                    // So the maximum length of the input string is STUDENT_ID_MAX_LEN - 1
                    if (userInputID.length() >= STUDENT_ID_MAX_LEN) {
                        std::cout << "Error: ID terlalu panjang. Maksimum "
                                  << STUDENT_ID_MAX_LEN - 1 << " karakter." << std::endl;
                    } else {
                        long long ts = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                        std::cout << "Menggunakan timestamp saat ini: " << ts << std::endl;
                        attendanceSystem.recordScan(userInputID, ts);
                    }
                } else {
                    std::cout << "Input ID kosong, tidak ada scan yang dicatat." << std::endl;
                }
                break;
            case '6':
                attendanceSystem.processIncomingScansFromFile();
                break;
            case '0':
                std::cout << "Menutup iambouttogram_server." << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid (" << choice << "). Silakan coba lagi." << std::endl;
        }
    } while (choice != '0');

    return 0;
}