#  projectalprogambatugram - RFID Lab Attendance System - Simulasi Pencatatan Kehadiran Lab 💻🔬

## 💻 Language Used in Project:
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![JWT](https://img.shields.io/badge/JWT-black?style=for-the-badge&logo=JSON%20web%20tokens) ![Markdown](https://img.shields.io/badge/markdown-%23000000.svg?style=for-the-badge&logo=markdown&logoColor=white)

## 🌐 IamBoutToGram Member & Socials:
- Luthfi Abdillah Wahid - 2306266685 [![Luthfi](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/luthfiwhd) 
- Muhammad Gavin Jericho - 2306215002 [![Gavin](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/gav.jericho)
- Rafandra Gifarrel Maritza Alifa - 2306216390 [![Gavin](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/rappandra)
- M. Fathoni - 2306156826 [![Gavin](https://img.shields.io/badge/Instagram-%23E4405F.svg?logo=Instagram&logoColor=white)](https://instagram.com/fathoni0110)

--- 

Selamat datang di proyek Simulasi Sistem Pencatatan Kehadiran Laboratorium menggunakan RFID. Proyek ini merupakan implementasi C++ untuk menangani pencatatan kehadiran mahasiswa dan staf secara *real-time* (disimulasikan), dengan fokus pada arsitektur client-server (disimulasikan), persistensi data, dan penanganan konkurensi.

## 📜 Latar Belakang Masalah

Dalam lingkungan laboratorium modern, pencatatan kehadiran yang akurat dan *real-time* sangat penting untuk keamanan dan audit. Studi kasus yang mendasari proyek ini adalah:

> Untuk kepentingan keamanan dan audit kehadiran, mahasiswa dan staf diwajibkan memindai kartu RFID setiap masuk atau keluar laboratorium. Saat ini log pemindaian hanya tersimpan di *reader* lokal dan dipindahkan secara manual ke server pusat sekali sehari, sehingga data *realtime* tidak tersedia. Proses ini juga memerlukan intervensi staf laboratorium, yang berarti kemungkinan kehilangan atau duplikasi data cukup tinggi. Tanpa sistem client-server yang menyalurkan data scan segera ke pusat, sulit untuk membuat laporan kehadiran harian otomatis atau mendeteksi aktivitas mencurigakan.

## ✨ Solusi yang Diimplementasikan

Proyek ini mengatasi masalah tersebut dengan menyediakan simulasi sistem terpusat di mana:
1.  **Simulasi Client-Server**: RFID *reader* (disimulasikan sebagai *client*) "mengirimkan" data pemindaian (ID dan timestamp) ke sistem pusat (server).
2.  **Pencarian & Pengurutan Data**: Menyediakan mekanisme untuk mencari log berdasarkan ID pengguna dan mengurutkan kunjungan berdasarkan waktu.
3.  **Persistensi Data**:
    * Semua log pemindaian disimpan secara persisten dalam file biner (`attendance.dat`).
    * Log harian dapat diekspor ke file JSON (misalnya, `attendance_YYYYMMDD.json`) untuk kemudahan integrasi dengan *dashboard* administrasi, tanpa menggunakan pustaka JSON eksternal.
4.  **Pemrosesan Paralel & Sinkronisasi**: Mampu menangani banyak pemindaian (disimulasikan dari berbagai *thread*) secara bersamaan dan memastikan integritas file log menggunakan mekanisme *mutex*.

## 🚀 Fitur Utama

* **Simulasi Pencatatan Real-time**: Setiap "scan" RFID dicatat segera oleh sistem.
* **Penyimpanan Log Persisten**: Data kehadiran disimpan dalam file biner dan dimuat saat aplikasi dimulai.
* **Ekspor Data Fleksibel**: Kemampuan untuk mengekspor semua data log ke format JSON dengan nama file berbasis tanggal.
* **Manajemen Data**:
    * Lihat semua log kehadiran.
    * Cari log berdasarkan ID Mahasiswa/Staf.
    * Urutkan log berdasarkan waktu pemindaian.
* **Simulasi Multi-Client**: Menggunakan `std::thread` untuk mensimulasikan beberapa RFID *reader* yang mengirimkan data secara bersamaan.
* **Keamanan Thread (Thread-Safety)**: Menggunakan `std::mutex` untuk melindungi akses ke data bersama dan operasi file, mencegah *race conditions* dan korupsi data.
* **Implementasi C++ Murni**: Ditulis sepenuhnya dalam C++ standar (C++11 atau lebih baru) tanpa dependensi pustaka eksternal untuk fungsionalitas inti (termasuk pembuatan JSON).
* **Antarmuka Menu Sederhana**: Interaksi dengan sistem melalui menu berbasis konsol.

## 🏗️ Struktur Kode (Single File)

Proyek ini diimplementasikan dalam satu file C++ (`.cpp`) untuk kesederhanaan. Komponen utama dalam kode adalah:

1.  **`struct LogEntry`**:
    * Mendefinisikan struktur data untuk setiap catatan kehadiran, menyimpan `studentID` (char array) dan `timestamp` (long long).
2.  **`class AttendanceSystem`**:
    * Merangkum semua logika "server".
    * Mengelola `std::vector<LogEntry>` untuk menyimpan log di memori.
    * Menyediakan `std::mutex` untuk sinkronisasi.
    * Fungsi untuk:
        * `recordScan()`: Menerima dan memproses data scan.
        * `loadLogsFromBinary()`, `appendLogToBinary()`: Mengelola persistensi file biner.
        * `searchLogsByID()`, `sortLogsByTime()`: Fitur pencarian dan pengurutan.
        * `exportLogsToJSON()`: Membuat output JSON secara manual.
        * `viewAllLogs()`: Menampilkan log.
3.  **`simulateRFIDClient()` function**:
    * Fungsi yang dijalankan oleh `std::thread` untuk mensimulasikan perilaku RFID *client* yang mengirimkan data scan.
4.  **`main()` function**:
    * Titik masuk aplikasi.
    * Menginisialisasi `AttendanceSystem`.
    * Membuat beberapa *thread* untuk `simulateRFIDClient`.
    * Menyediakan antarmuka menu interaktif untuk pengguna.

## 🛠️ Teknologi yang Digunakan

* **Bahasa**: C++ (dengan standar C++11 atau lebih baru)
* **Pustaka Standar C++**:
    * `<iostream>` untuk input/output konsol.
    * `<vector>` untuk manajemen data log dinamis.
    * `<string>` untuk manipulasi string.
    * `<fstream>` untuk operasi file (biner dan teks).
    * `<algorithm>` untuk fungsi pengurutan.
    * `<thread>` untuk simulasi konkurensi.
    * `<mutex>` untuk sinkronisasi antar thread.
    * `<chrono>` untuk mendapatkan timestamp.
    * `<iomanip>`, `<sstream>` untuk format output dan manipulasi string stream.
    * `<ctime>`, `<cstring>` untuk fungsi terkait waktu dan C-string.
    * `<limits>` untuk `std::numeric_limits`.

## ⚙️ Cara Menggunakan

### Prasyarat

* Kompiler C++ yang mendukung C++11 atau lebih baru (misalnya, g++, Clang, MinGW untuk Windows, MSVC).

### Kompilasi

1.  Simpan kode sumber sebagai file (`iambouttogram.cpp`).
2.  Buka terminal atau command prompt Anda.
3.  Pindah ke direktori tempat Anda menyimpan file tersebut.
4.  Kompilasi menggunakan perintah berikut (contoh untuk g++/MinGW):

    ```bash
    g++ iambouttogram.cpp -o iambouttogram -std=c++11 -pthread
    ```
    * `-o rfid_system`: Menentukan nama file output (executable).
    * `-std=c++11`: Menggunakan standar C++11.
    * `-pthread`: Diperlukan untuk dukungan `std::thread` pada beberapa konfigurasi g++/MinGW. Untuk MSVC, ini biasanya tidak diperlukan secara eksplisit.

### Menjalankan Program

Setelah kompilasi berhasil, jalankan program dari terminal:

```bash
./iambouttogram  