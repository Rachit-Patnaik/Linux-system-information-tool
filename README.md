# 🖥️ Linux System Information Tool

<p align="center">
  <img src="https://img.shields.io/badge/Language-C99-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C99">
  <img src="https://img.shields.io/badge/Platform-Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux">
  <img src="https://img.shields.io/badge/Build-Make-A42E2B?style=for-the-badge&logo=gnu&logoColor=white" alt="Make">
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License: MIT">
</p>

<p align="center">
A lightweight, real-time, command-line system monitor written in <b>pure C</b> — no third-party monitoring libraries, no bloat. It reads live hardware and OS telemetry directly from the Linux <code>/proc</code> virtual filesystem and POSIX system calls.
</p>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Directory Structure](#-directory-structure)
- [Technical Deep Dive](#-technical-deep-dive)
  - [CPU Usage Calculation](#cpu-usage-calculation)
  - [Memory Usage Calculation](#memory-usage-calculation)
- [Prerequisites](#-prerequisites)
- [Build & Run](#-build--run)
- [Sample Output](#-sample-output)
- [How It Works](#-how-it-works)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🔍 Overview

**Linux System Information Tool** is a real-time terminal dashboard that reports live system stats — hostname, OS/kernel version, uptime, CPU model, core count, CPU utilization, and memory usage — refreshed every second directly in your terminal.

Instead of shelling out to external tools like `top`, `htop`, or `neofetch`, this project parses raw kernel-exposed data straight from `/proc/cpuinfo`, `/proc/stat`, and `/proc/meminfo`, and uses POSIX calls (`gethostname`, `uname`, `sysinfo`) for OS-level metadata. This makes it fast, dependency-free, and a great reference for understanding how Linux exposes system telemetry.

Built and tested on **Ubuntu (WSL2)**.

---

## ✨ Features

| Category | Detail |
|---|---|
| 🖥️ **System Info** | Hostname (`gethostname`), OS name & kernel release (`uname`), uptime in `days:hours:minutes:seconds` (`sysinfo`) |
| ⚙️ **CPU Info** | Processor model string, logical core count (from `/proc/cpuinfo`), live CPU utilization % (delta-sampled from `/proc/stat`), ASCII usage bar |
| 🧠 **Memory Info** | Total / used / available RAM in GB (from `/proc/meminfo`), usage %, ASCII memory gauge |
| 🔁 **Live TUI Engine** | 1-second refresh loop with terminal screen control |
| 🧹 **Graceful Exit** | `SIGINT` (Ctrl+C) handler restores terminal cursor state and cleans up before exit |

---

## 📁 Directory Structure

```
Linux-system-information-tool/
├── Makefile                # Build rules: compile, link, clean
├── .gitignore
└── src/
    ├── main.c               # Entry point: main loop, screen refresh, signal handling
    ├── system_info.h        # Declarations: hostname, OS, kernel, uptime
    ├── system_info.c        # Implementation: gethostname/uname/sysinfo logic
    ├── cpu_info.h            # Declarations: CPU model, core count, usage %
    ├── cpu_info.c            # Implementation: /proc/cpuinfo & /proc/stat parsing
    ├── mem_info.h             # Declarations: memory stats
    └── mem_info.c             # Implementation: /proc/meminfo parsing
```

---

## 🧮 Technical Deep Dive

### CPU Usage Calculation

CPU load isn't directly exposed by the kernel as a percentage — it must be **derived** from cumulative tick counters exposed in `/proc/stat`. The first line of that file looks like:

```
cpu  user nice system idle iowait irq softirq steal guest guest_nice
```

Each value is the total number of clock ticks the CPU has spent in that state **since boot**. Because these are cumulative counters, a single snapshot is meaningless — usage must be computed as a **delta between two samples** taken one second apart.

**Step 1 — Classify tick states:**

$$
\text{Idle} = \text{idle} + \text{iowait}
$$

$$
\text{NonIdle} = \text{user} + \text{nice} + \text{system} + \text{irq} + \text{softirq} + \text{steal}
$$

$$
\text{Total} = \text{Idle} + \text{NonIdle}
$$

**Step 2 — Take two samples ($t_0$ and $t_1$), one second apart:**

$$
\Delta\text{Total} = \text{Total}_{t_1} - \text{Total}_{t_0}
$$

$$
\Delta\text{Idle} = \text{Idle}_{t_1} - \text{Idle}_{t_0}
$$

**Step 3 — Compute utilization percentage:**

$$
\text{CPU Usage \%} = \left( \frac{\Delta\text{Total} - \Delta\text{Idle}}{\Delta\text{Total}} \right) \times 100
$$

This gives the fraction of CPU time that was **not** spent idling during the sampling window — i.e., real utilization. The tool performs this two-sample delta internally every refresh cycle to keep the reading current.

### Memory Usage Calculation

Memory statistics are parsed from key-value pairs in `/proc/meminfo` (values in kB):

```
MemTotal:        16384000 kB
MemFree:          2048000 kB
MemAvailable:     9500000 kB
...
```

The tool specifically uses `MemTotal` and `MemAvailable` — **not** `MemFree` — because `MemAvailable` already accounts for reclaimable caches and buffers, giving a realistic estimate of memory actually available to applications.

**Used memory:**

$$
\text{MemUsed} = \text{MemTotal} - \text{MemAvailable}
$$

**Usage percentage:**

$$
\text{Memory Usage \%} = \left( \frac{\text{MemUsed}}{\text{MemTotal}} \right) \times 100
$$

**Conversion to GB** (values from `/proc/meminfo` are in kB):

$$
\text{GB} = \frac{\text{kB}}{1024 \times 1024}
$$

---

## 🛠️ Prerequisites

- A Linux environment (native Linux or **WSL2** on Windows)
- `gcc` (GNU Compiler Collection)
- `make`

Install on Debian/Ubuntu if needed:

```bash
sudo apt update
sudo apt install build-essential
```

---

## 🚀 Build & Run

### Option 1 — Using `make` (recommended)

```bash
# Clone the repository
git clone https://github.com/Rachit-Patnaik/Linux-system-information-tool.git
cd Linux-system-information-tool

# Build
make

# Run
./sysinfo_tool
```

Clean build artifacts:

```bash
make clean
```

### Option 2 — Direct `gcc` compilation

```bash
gcc -Wall -Wextra -O2 -std=c99 -Isrc src/main.c src/system_info.c src/cpu_info.c src/mem_info.c -o sysinfo_tool
./sysinfo_tool
```

Press **Ctrl+C** at any time to exit — the tool will restore your terminal cursor state cleanly before quitting.

---

## 🖼️ Sample Output

```
==== SYSTEM INFORMATION ====
Hostname   : rachit-pc
OS         : Ubuntu 22.04.3 LTS
Kernel     : 5.15.146.1-microsoft-standard
Uptime     : 2d 04h 13m 27s

==== CPU INFORMATION ====
Model      : Intel(R) Core(TM) i5-1135G7
Cores      : 8
Usage      : 37.4%
[###########.................]

==== MEMORY INFORMATION ====
Total      : 15.6 GB
Used       : 6.1 GB
Usage      : 39.1%
[############................]

Refreshing every 1s — Ctrl+C to exit
```
---

## ⚙️ How It Works

1. **`main.c`** starts the render loop, clears the screen using terminal control sequences, and registers a `SIGINT` handler.
2. Each refresh cycle:
   - **`system_info.c`** fetches hostname, OS name, kernel release, and uptime.
   - **`cpu_info.c`** parses `/proc/cpuinfo` for static info (model, core count) and samples `/proc/stat` twice, one second apart, to compute live CPU usage.
   - **`mem_info.c`** parses `/proc/meminfo` for `MemTotal` and `MemAvailable` to compute used memory and usage percentage.
3. All values are rendered into a formatted terminal panel with ASCII usage bars.
4. On **Ctrl+C**, the signal handler restores the terminal cursor and exits cleanly — no leftover terminal corruption.

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

<p align="center"><sub>© 2025 Rachit Patnaik · Built with C99 for Linux/POSIX systems</sub></p>
