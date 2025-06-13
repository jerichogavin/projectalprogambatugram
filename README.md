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

## 🏗️ Struktur Proyek

Proyek ini dibagi menjadi beberapa file untuk memisahkan logika server dan klien:
1.  **`server.cpp`**: Implementasi server yang menangani koneksi, menerima data, mencatat log, dan mengekspor data.
2.  **`client.cpp`**: Implementasi client yang terhubung ke server dan mengirimkan data scan RFID (NPM).
3.  **`iambouttogram.json`**: File database awal yang berisi daftar mahasiswa (NPM dan Nama) yang valid.

## 🛠️ Teknologi yang Digunakan

* **Bahasa**: C++ (standar C++11 atau lebih baru)
* **Pustaka Standar C++**:
    * `<iostream>`, `<vector>`, `<string>`, `<fstream>`
    * `<thread>`, `<mutex>` untuk konkurensi dan sinkronisasi.
    * `<chrono>`, `<iomanip>` untuk manajemen waktu dan timestamp.
* **Networking**:
    * **Windows**: `winsock2.h`
    * **Linux/Unix**: `sys/socket.h`, `netinet/in.h`

## ⚙️ Build & Run Guide

### Kompilasi (Build Instructions)

Pastikan Anda memiliki kompiler C++ (seperti g++ pada MinGW/MSYS2 untuk Windows atau g++ pada Linux).

**Windows (MinGW/MSYS2)**
```bash
g++ -o server.exe server.cpp -lws2_32 -pthread
g++ -o client.exe client.cpp -lws2_32 -pthread


### Menjalankan Program (Run Instructions)

#### Langkah 1: Jalankan Server

Buka terminal dan jalankan file executable server.

Windows: server.exe
Linux: ./server
Server akan:

Memuat database dari iambouttogram.json.
Mendengarkan koneksi pada port 8080.
Menampilkan log aktivitas secara real-time di konsol.
Menyimpan log kehadiran harian ke dalam file .log (contoh: attendance_20250613.log).
Perintah Server (Server commands):

* export - Mengekspor semua log yang tercatat ke dalam file JSON.
* quit - Menghentikan dan keluar dari server.

Langkah 2: Jalankan Client (di Terminal baru)

Buka terminal baru untuk setiap client yang ingin Anda jalankan.

Windows: `client.exe`
Linux: `./client`

Client akan Terhubung ke server yang berjalan di localhost:8080. Menampilkan menu interaktif untuk pengguna. Mensimulasikan proses scan RFID dengan meminta input NPM dari pengguna.

