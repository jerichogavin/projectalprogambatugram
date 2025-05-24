# projectalprogambatugram - RFID Lab Attendance System - Simulasi Pencatatan Kehadiran Lab 💻🔬

## 💻 Language Used in Project:
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![Markdown](https://img.shields.io/badge/markdown-%23000000.svg?style=for-the-badge&logo=markdown&logoColor=white)

## 🌐 IamBoutToGram Member & Socials:
- Luthfi Abdillah Wahid - 2306266685 [![Luthfi](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/luthfiwhd)
- Muhammad Gavin Jericho - 2306215002 [![Gavin](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/gav.jericho)
- Rafandra Gifarrel Maritza Alifa - 2306216390 [![Rafandra](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/rappandra)
- M. Fathoni - 2306156826 [![Fathoni](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/fathoni0110)

---

Selamat datang di proyek Simulasi Sistem Pencatatan Kehadiran Laboratorium menggunakan RFID. Proyek ini merupakan implementasi C++ untuk menangani pencatatan kehadiran mahasiswa dan staf secara *real-time* (disimulasikan), dengan fokus pada arsitektur client-server, persistensi data, dan penanganan konkurensi.

## 📜 Latar Belakang Masalah

Dalam lingkungan laboratorium modern, pencatatan kehadiran yang akurat dan *real-time* sangat penting untuk keamanan dan audit. Studi kasus yang mendasari proyek ini adalah:

> Untuk kepentingan keamanan dan audit kehadiran, mahasiswa dan staf diwajibkan memindai kartu RFID setiap masuk atau keluar laboratorium. Saat ini log pemindaian hanya tersimpan di *reader* lokal dan dipindahkan secara manual ke server pusat sekali sehari, sehingga data *realtime* tidak tersedia. Proses ini juga memerlukan intervensi staf laboratorium, yang berarti kemungkinan kehilangan atau duplikasi data cukup tinggi. Tanpa sistem client-server yang menyalurkan data scan segera ke pusat, sulit untuk membuat laporan kehadiran harian otomatis atau mendeteksi aktivitas mencurigakan.

## ✨ Solusi yang Diimplementasikan

Proyek ini mengatasi masalah tersebut dengan menyediakan sistem terpusat di mana:
1.  **Arsitektur Client-Server**: RFID *reader* (disimulasikan sebagai program `iambouttogram_client`) "mengirimkan" data pemindaian (ID dan timestamp) ke sistem pusat (program `iambouttogram_server`). (Catatan: Komunikasi aktual antar proses seperti *sockets* tidak diimplementasikan dalam contoh ini; client mencetak data yang akan dikirim).
2.  **Pencarian & Pengurutan Data**: Server menyediakan mekanisme untuk mencari log berdasarkan ID pengguna dan mengurutkan kunjungan berdasarkan waktu.
3.  **Persistensi Data**:
    * Semua log pemindaian yang diterima server disimpan secara persisten dalam file biner (`attendance.dat`).
    * Log harian dapat diekspor oleh server ke file JSON (misalnya, `attendance_YYYYMMDD.json`) untuk kemudahan integrasi, tanpa menggunakan pustaka JSON eksternal.
4.  **Pemrosesan Paralel & Sinkronisasi (di Server)**: Server dirancang untuk menangani banyak data (jika menerima dari banyak klien) dan memastikan integritas file log menggunakan mekanisme *mutex*.

## 🚀 Fitur Utama

* **Simulasi Pencatatan Real-time**: Setiap "scan" RFID yang diterima server dicatat segera.
* **Penyimpanan Log Persisten**: Data kehadiran disimpan oleh server dalam file biner dan dimuat saat server dimulai.
* **Ekspor Data Fleksibel**: Server memiliki kemampuan untuk mengekspor semua data log ke format JSON dengan nama file berbasis tanggal.
* **Manajemen Data (oleh Server)**:
    * Lihat semua log kehadiran.
    * Cari log berdasarkan ID Mahasiswa/Staf.
    * Urutkan log berdasarkan waktu pemindaian.
* **Simulasi Multi-Client**: Dapat dijalankan beberapa instansi dari program `iambouttogram_client` untuk mensimulasikan beberapa RFID *reader*.
* **Keamanan Thread (Thread-Safety) di Server**: Server menggunakan `std::mutex` untuk melindungi akses ke data bersama (log dalam memori) dan operasi file, mencegah *race conditions* dan korupsi data.
* **Implementasi C++ Murni**: Ditulis sepenuhnya dalam C++ standar (C++11 atau lebih baru) tanpa dependensi pustaka eksternal untuk fungsionalitas inti (termasuk pembuatan JSON).
* **Antarmuka Menu Sederhana**: Interaksi dengan sistem server melalui menu berbasis konsol.

## 🏗️ Struktur Proyek (Multi-File)

Proyek ini dibagi menjadi beberapa file untuk memisahkan logika server, klien, dan komponen bersama:

1.  **`attendance_common.h`**:
    * Berisi definisi umum yang digunakan oleh server dan klien.
    * `struct LogEntry`: Mendefinisikan struktur data untuk setiap catatan kehadiran (menyimpan `studentID` dan `timestamp`).
    * Konstanta seperti `BINARY_LOG_FILE` dan `STUDENT_ID_MAX_LEN`.

2.  **`attendance_system.h`**:
    * File header untuk kelas `AttendanceSystem`.
    * Mendeklarasikan antarmuka kelas yang menangani semua logika inti server.

3.  **`attendance_system.cpp`**:
    * Implementasi dari kelas `AttendanceSystem`.
    * Mengelola `std::vector<LogEntry>` untuk menyimpan log di memori.
    * Menyediakan `std::mutex` untuk sinkronisasi.
    * Fungsi utama: `recordScan()`, `loadLogsFromBinary()`, `appendLogToBinary()`, `searchLogsByID()`, `sortLogsByTime()`, `exportLogsToJSON()`, `viewAllLogs()`.

4.  **`iambouttogram_server_main.cpp`**:
    * Titik masuk untuk program server (`iambouttogram_server`).
    * Menginisialisasi `AttendanceSystem`.
    * Menyediakan antarmuka menu interaktif admin untuk mengelola dan melihat log.
    * Secara konseptual, ini adalah tempat server akan "mendengarkan" data dari klien.

5.  **`iambouttogram_client_main.cpp`**:
    * Titik masuk untuk program klien (`iambouttogram_client`).
    * Mensimulasikan perilaku RFID *reader*.
    * Mengumpulkan ID pengguna (misalnya dari argumen baris perintah) dan menghasilkan *timestamp*.
    * "Mengirim" data ini (dalam simulasi ini, data dicetak ke konsol).

## 🛠️ Teknologi yang Digunakan

* **Bahasa**: C++ (dengan standar C++11 atau lebih baru)
* **Pustaka Standar C++**:
    * `<iostream>` untuk input/output konsol.
    * `<vector>` untuk manajemen data log dinamis.
    * `<string>` untuk manipulasi string.
    * `<fstream>` untuk operasi file (biner dan teks).
    * `<algorithm>` untuk fungsi pengurutan.
    * `<thread>` untuk konkurensi (digunakan di server jika menangani banyak tugas, dan di klien jika diperlukan).
    * `<mutex>` untuk sinkronisasi antar thread di server.
    * `<chrono>` untuk mendapatkan timestamp.
    * `<iomanip>`, `<sstream>` untuk format output dan manipulasi string stream.
    * `<ctime>`, `<cstring>` untuk fungsi terkait waktu dan C-string.
    * `<limits>` untuk `std::numeric_limits`.

## ⚙️ Cara Menggunakan

### Prasyarat

* Kompiler C++ yang mendukung C++11 atau lebih baru (misalnya, g++, Clang, MinGW untuk Windows, MSVC).
* Git untuk kloning repositori (opsional, jika kode sudah ada).

### Kompilasi

1.  Pastikan semua file sumber (`.h` dan `.cpp`) berada dalam direktori yang sama.
2.  Buka terminal atau command prompt Anda.
3.  Pindah ke direktori tempat Anda menyimpan file-file tersebut.
4.  Kompilasi program server:
    ```bash
    g++ -std=c++11 -pthread iambouttogram_server_main.cpp attendance_system.cpp -o iambouttogram_server
    ```
5.  Kompilasi program klien:
    ```bash
    g++ -std=c++11 -pthread iambouttogram_client_main.cpp -o iambouttogram_client
    ```
    * `-o <nama_executable>`: Menentukan nama file output (executable).
    * `-std=c++11`: Menggunakan standar C++11.
    * `-pthread`: Diperlukan untuk dukungan `std::thread` pada beberapa konfigurasi g++/MinGW. Untuk MSVC, ini biasanya tidak diperlukan secara eksplisit.

### Menjalankan Program

Setelah kompilasi berhasil:

1.  **Jalankan Server**:
    Buka terminal dan jalankan server. Server akan memuat log yang ada (jika ada `attendance.dat`) dan menunggu interaksi melalui menu admin.
    ```bash
    ./iambouttogram_server
    ```
    Server akan berjalan dan menampilkan menu admin. File `attendance.dat` akan dibuat/digunakan di direktori tempat server dijalankan.

2.  **Jalankan Klien (di terminal terpisah)**:
    Buka terminal lain untuk menjalankan satu atau lebih instansi klien. Klien akan mensimulasikan pengiriman data scan.
    ```bash
    ./iambouttogram_client <StudentID> [jumlahScan] [delayMs]
    ```
    * `<StudentID>`: ID mahasiswa atau staf (misalnya, `MHS123`).
    * `[jumlahScan]` (opsional): Berapa kali scan akan disimulasikan untuk ID ini (default: 1).
    * `[delayMs]` (opsional): Delay dalam milidetik antar scan jika `jumlahScan > 1` (default: 1000ms).

    **Contoh Client [IMPORTANT]:**
    ```bash
    ./iambouttogram_client MHS001
    ```
    (Mensimulasikan satu scan untuk MHS001)

    ```bash
    ./iambouttogram_client STF007 3 500
    ```
    (Mensimulasikan STF007 melakukan scan 3 kali dengan interval 500ms)



