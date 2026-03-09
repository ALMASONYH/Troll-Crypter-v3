# Troll Crypter v3

Advanced Python file protector with multi-layer encryption — built in C++ with ImGui GUI.

**Mason Group | Freemasonry | Developed by ABOLHB**

![C++17](https://img.shields.io/badge/C%2B%2B-17-purple.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-darkviolet.svg)

---

## Features

- **Multi-Layer Encryption** — 2 to 7 nested encryption layers
- **Dual Cipher** — AES-256-GCM + ChaCha20-Poly1305
- **Key Derivation** — scrypt KDF with SHA-512 + HMAC-SHA512 stretching
- **Anti-Debug** — Detects debuggers, tracers, and analysis tools
- **Anti-VM** — Detects virtual machines and sandboxes
- **HMAC Protection** — HMAC-SHA512 tamper detection on output
- **Junk Code Injection** — Random dead code to confuse decompilers
- **Dark Purple GUI** — Custom ImGui theme with smooth animations

---

## Project Structure

```
TrollCrypter-CPP/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── icon.ico              ← App icon (you provide this)
├── main.cpp
├── app.rc
├── core/
│   ├── types.h
│   ├── config.h
│   ├── orchestrator.h
│   ├── orchestrator.cpp
│   └── validator.h
├── crypto/
│   ├── engines.h
│   ├── engines.cpp
│   ├── random.h
│   └── random.cpp
├── codegen/
│   ├── codegen.h
│   ├── codegen.cpp
│   └── builders.cpp
└── ui/
    ├── theme.h
    ├── main_window.h
    └── main_window.cpp
```

---

## Requirements

- **Windows 10/11**
- **Visual Studio 2022** (Community is free) with "Desktop development with C++" workload
- **CMake 3.20+**
- **Git**

---

## Build Instructions

Open **Developer Command Prompt for VS 2022** (or regular CMD/PowerShell).

### Step 1: Install CMake and Git (skip if already installed)

```cmd
winget install Kitware.CMake --accept-package-agreements
winget install Git.Git --accept-package-agreements
```

Close and reopen your terminal after installing.

### Step 2: Clone vcpkg and install dependencies

```cmd
cd TrollCrypter-CPP
git clone https://github.com/microsoft/vcpkg.git --depth 1
vcpkg\bootstrap-vcpkg.bat -disableMetrics
vcpkg\vcpkg.exe install openssl:x64-windows zlib:x64-windows
```

This takes 5-10 minutes on first run. vcpkg will download and compile OpenSSL and zlib.

### Step 3: Configure with CMake

```cmd
mkdir build
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022"
```

> If you have VS 2019 instead, replace `"Visual Studio 17 2022"` with `"Visual Studio 16 2019"`.

### Step 4: Compile

```cmd
cmake --build build --config Release --parallel
```

### Step 5: Run

```cmd
build\bin\Release\TrollCrypter.exe
```

---

## Clean Rebuild

```cmd
rmdir /s /q build
mkdir build
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022"
cmake --build build --config Release --parallel
```

---

## App Icon

Place your `icon.ico` file in the project root (next to CMakeLists.txt). It will be embedded into the exe automatically during build.

---

## Protection Modes

| Mode | Layers | Cipher | KDF Rounds | Key Bits |
|------|--------|--------|------------|----------|
| Quick | 2 | AES-256-GCM | 100-250 | 128 |
| Standard | 3 | AES + ChaCha20 | 400-700 | 128 |
| Paranoid | 5 | AES + ChaCha20 | 800-1400 | 256 |
| Custom | 2-7 | Configurable | 100-2000 | 64/128/256 |

---

## Running Protected Files

Protected output files need Python 3.8+ and pycryptodome:

```cmd
pip install pycryptodome
python protected_file.py
```

---

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) — GUI framework
- [GLFW](https://www.glfw.org/) — Window management
- [OpenSSL](https://www.openssl.org/) — Cryptography
- [zlib](https://zlib.net/) — Compression

---

## Author

- Developed by **ABOLHB** — [abolhb.com](https://abolhb.com)
- Group: **Mason Group / Freemasonry**
- GitHub: [ALMASONYH](https://github.com/ALMASONYH)
- Discord: [discord.gg/uCwQuJK](https://discord.gg/uCwQuJK)
