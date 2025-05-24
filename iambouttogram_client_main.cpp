#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread> // For sleep
#include "attendance_common.h" // For LogEntry and constants

// Function to simulate sending data to the server
// In a real application, this would use sockets or another IPC mechanism.
void sendScanDataToServer(const LogEntry& entry) {
    std::cout << "[Client] Mengirim data ke server (simulasi):" << std::endl;
    std::cout << "  StudentID: " << entry.studentID << std::endl;
    std::cout << "  Timestamp: " << entry.timestamp << std::endl;
    // Example: write to a file, send over network, etc.
    // For now, we just print it.
    // If using file IPC:
    // std::ofstream outfile("scan_pipe.txt", std::ios::app);
    // if (outfile.is_open()) {
    //     outfile << entry.studentID << "," << entry.timestamp << std::endl;
    //     outfile.close();
    // } else {
    //     std::cerr << "[Client] Error: Tidak dapat membuka pipe file untuk mengirim data." << std::endl;
    // }
}

int main(int argc, char* argv[]) {
    std::string studentID;
    int numScans = 1;
    int delayMs = 1000; // Default delay between scans if numScans > 1

    if (argc < 2) {
        std::cerr << "Penggunaan: " << argv[0] << " <StudentID> [numScans] [delayMs]" << std::endl;
        std::cerr << "Contoh: " << argv[0] << " MHS123" << std::endl;
        std::cerr << "Contoh: " << argv[0] << " STF007 5 500 (untuk 5 scan dengan delay 500ms)" << std::endl;
        // Defaulting to a test ID if no arguments are provided for easier testing
        std::cout << "Tidak ada ID diberikan, menggunakan ID default 'TEST001' untuk 1 scan." << std::endl;
        studentID = "TEST001";
    } else {
        studentID = argv[1];
        if (argc > 2) {
            try {
                numScans = std::stoi(argv[2]);
            } catch (const std::exception& e) {
                std::cerr << "Error: Jumlah scan tidak valid: " << argv[2] << ". Menggunakan default 1." << std::endl;
                numScans = 1;
            }
        }
        if (argc > 3) {
             try {
                delayMs = std::stoi(argv[3]);
            } catch (const std::exception& e) {
                std::cerr << "Error: Delay tidak valid: " << argv[3] << ". Menggunakan default 1000ms." << std::endl;
                delayMs = 1000;
            }
        }
    }
    

    if (studentID.length() >= STUDENT_ID_MAX_LEN) {
        std::cerr << "[Client] Error: Student ID terlalu panjang (maks " 
                  << STUDENT_ID_MAX_LEN -1 << " karakter)." << std::endl;
        return 1;
    }

    std::cout << "iambouttogram_client dimulai untuk ID: " << studentID 
              << ", Jumlah Scan: " << numScans 
              << ", Delay: " << delayMs << "ms" << std::endl;

    for (int i = 0; i < numScans; ++i) {
        long long currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        LogEntry scanToSend(studentID, currentTimestamp);
        
        std::cout << "\n[Client] (" << i+1 << "/" << numScans <<") Mensimulasikan scan..." << std::endl;
        sendScanDataToServer(scanToSend);

        if (numScans > 1 && i < numScans - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    
    std::cout << "\niambouttogram_client selesai." << std::endl;
    return 0;
}