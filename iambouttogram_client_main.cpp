//   __   __   _  _  ____   __   _  _  ____  ____  __    ___  ____   __   _  _ 
//  (  ) / _\ ( \/ )(  _ \ /  \ / )( \(_  _)(_  _)/  \  / __)(  _ \ / _\ ( \/ )
//   )( /    \/ \/ \ ) _ ((  O )) \/ (  )(    )( (  O )( (_ \ )   //    \/ \/ \
//  (__)\_/\_/\_)(_/(____/ \__/ \____/ (__)  (__) \__/  \___/(__\_)\_/\_/\_)(_/

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>   // For sleep
#include <fstream>  // <-- BARU: Untuk std::ofstream
#include <sstream>  // <-- BARU: Untuk std::ostringstream
#include "attendance_common.h" 

// Function to send data to the server via a shared file
void sendScanDataToServer(const LogEntry& entry) {
    std::cout << "[Client] Menulis data ke pipe file (" << IPC_PIPE_FILE << "):" << std::endl;
    std::cout << "  StudentID: " << entry.studentID << std::endl;
    std::cout << "  Timestamp: " << entry.timestamp << std::endl;

    std::ofstream pipeFile(IPC_PIPE_FILE, std::ios::app); // Buka dalam mode append
    if (pipeFile.is_open()) {
        pipeFile << entry.studentID << "," << entry.timestamp << std::endl;
        pipeFile.close();
        std::cout << "[Client] Data berhasil ditulis ke " << IPC_PIPE_FILE << std::endl;
    } else {
        std::cerr << "[Client] Error: Tidak dapat membuka pipe file (" << IPC_PIPE_FILE << ") untuk mengirim data." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string studentID_str; // Ganti nama variabel agar tidak bentrok
    int numScans = 1;
    int delayMs = 1000; 

    if (argc < 2) {
        std::cerr << "Penggunaan: " << argv[0] << " <StudentID> [numScans] [delayMs]" << std::endl;
        std::cout << "Tidak ada ID diberikan, menggunakan ID default 'TESTCLIENT001' untuk 1 scan." << std::endl;
        studentID_str = "TESTCLIENT001";
    } else {
        studentID_str = argv[1];
        if (argc > 2) {
            try {
                numScans = std::stoi(argv[2]);
                if (numScans <= 0) numScans = 1;
            } catch (const std::exception& e) {
                std::cerr << "Error: Jumlah scan tidak valid: " << argv[2] << ". Menggunakan default 1." << std::endl;
                numScans = 1;
            }
        }
        if (argc > 3) {
            try {
                delayMs = std::stoi(argv[3]);
                if (delayMs < 0) delayMs = 0;
            } catch (const std::exception& e) {
                std::cerr << "Error: Delay tidak valid: " << argv[3] << ". Menggunakan default 1000ms." << std::endl;
                delayMs = 1000;
            }
        }
    }
    
    if (studentID_str.length() >= STUDENT_ID_MAX_LEN) {
        std::cerr << "[Client] Error: Student ID terlalu panjang (maks " 
                    << STUDENT_ID_MAX_LEN -1 << " karakter)." << std::endl;
        return 1;
    }

    std::cout << "iambouttogram_client dimulai untuk ID: " << studentID_str 
                << ", Jumlah Scan: " << numScans 
                << ", Delay: " << delayMs << "ms" << std::endl;

    for (int i = 0; i < numScans; ++i) {
        long long currentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        LogEntry scanToSend(studentID_str, currentTimestamp);
        
        std::cout << "\n[Client] (" << i+1 << "/" << numScans <<") Mensimulasikan scan..." << std::endl;
        sendScanDataToServer(scanToSend);

        if (numScans > 1 && i < numScans - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    
    std::cout << "\niambouttogram_client selesai." << std::endl;
    return 0;
}