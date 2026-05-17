# 🦈 SAS-Firmware (Predatory OS)

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-orange.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Framework-Arduino-blue.svg" alt="Framework">
  <img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License">
  <img src="https://img.shields.io/badge/Build-Passing-brightgreen.svg" alt="Build Status">
</p>

---

## 📌 Overview

**SAS-Firmware (Predatory OS)** is a premium, high-performance, and feature-rich offensive security testing firmware tailored for ESP32 microcontrollers, specifically designed for **M5Stack (M5StickC Plus 1.1 / Plus 2)** hardware. 

Inspired by professional cyber security tools, SAS-Firmware features a custom, high-contrast Flipper Zero-inspired **Vertical List Menu** with custom-scaled vector miniature logos, responsive event scrollbars, and a signature **internal flash-based cat boot animation** that plays directly on startup.

This project is created and maintained under **PutamStudios** (led by Ataberk Celil).

---

## ⚡ Core Features

### 📡 1. Sub-GHz & RF Suite (CC1101 / RF24)
* **Custom SubGhz**: Replay, record, and transmit custom RF payload protocols.
* **Spectrum Analyzer & Waterfall**: Real-time signal graphing and scanner.
* **Bruteforcing & Jamming**: Signal penetration and noise injection testing.

### 📶 2. Wi-Fi Auditing & Attacks
* **Targeted Attacks & Deauther**: Disrupt client connections and capture PMKID handshakes.
* **Beacon Spam & Karma**: Inject custom SSIDs and probe response payloads.
* **Wardriving & WiGLE**: Map surrounding access points with GPS integration.
* **Evil Portal**: Phishing gateway testing with local DNS interception.
* **Network Scan & ARP Tools**: DHCP Starvation, MAC Flooding, ARP Spoofing, and host port scanners.

### 🔵 3. BLE (Bluetooth Low Energy) Suite
* **Apple & Android Spam**: Target discovery and payload spamming.
* **Ninebot & Smart Lock Testing**: Protocol exploitation and BLE services.
* **Reconnaissance**: Sniff nearby BLE devices, signal strength (RSSI), and active advertising.

### 🔴 4. Infrared (IR) WORLD Suite
* **TV-B-Gone**: Universal TV power cycles utilizing global IR code banks.
* **IR Jammer & Reader**: Clone and repeat infrared remotes natively.

### 🔑 5. RFID / NFC & Emulation
* **PN532 / RC522 integration**: Read, clone, and emulate Mifare tags.
* **Amiibo & Chameleon Ultra**: Emulator linkages for physical access keys.

### 💻 6. Advanced Tools & Utilities
* **BadUSB Ducky Scripts**: Automate keyboard inputs for rapid payload execution.
* **Reverse Shell TCP**: Establish command execution tunnels remotely.
* **JS Interpreter**: Run custom JavaScript code dynamically on the ESP32.

---

## 🎨 Theme & User Interface

SAS-Firmware comes loaded with a gorgeous, high-contrast **Flipper Yellow Theme**:
* **Background**: `0xFFE0` (Sleek Warm Yellow)
* **Highlight/Text**: `0x0000` (Deep Carbon Black)
* **Menu**: Beautiful vertical list scrollable interface showing 3 rows at a time with a border-box indicator, miniature left icons, and a fast scrollbar.

---

## 🛠️ Build & Installation

This project is fully managed using **PlatformIO**.

### Prerequisites
1. Install [VSCode](https://code.visualstudio.com/).
2. Install the **PlatformIO IDE** extension.
3. Ensure PlatformIO Penvironment is configured on your system.

### Build and Flash
To build the project and flash it directly to your device (e.g. on `COM10` port):

```powershell
# Set console encoding to UTF-8 to prevent compilation checkmark crashes
$env:PYTHONIOENCODING="utf-8"

# Run PlatformIO compile and upload target to port COM10
pio run -e m5stack-cplus1_1 --target upload --upload-port COM10
```

---

## 📄 License

This project is licensed under the permissive **MIT License** - see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  Developed with ❤️ by <a href="https://github.com/PutamStudios">PutamStudios</a> (Ataberk Celil)
</p>
