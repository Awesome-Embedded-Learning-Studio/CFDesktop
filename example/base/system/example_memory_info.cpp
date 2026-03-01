#include "system/memory/memory_info.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

// Helper function to convert bytes to human-readable format
std::string formatBytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

// Helper function to convert memory type enum to string
const char* memoryTypeToString(cf::MemoryType type) {
    switch (type) {
        case cf::MemoryType::DDR2:
            return "DDR2";
        case cf::MemoryType::DDR3:
            return "DDR3";
        case cf::MemoryType::DDR4:
            return "DDR4";
        case cf::MemoryType::DDR5:
            return "DDR5";
        case cf::MemoryType::LPDDR3:
            return "LPDDR3";
        case cf::MemoryType::LPDDR4:
            return "LPDDR4";
        case cf::MemoryType::LPDDR4X:
            return "LPDDR4X";
        case cf::MemoryType::LPDDR5:
            return "LPDDR5";
        case cf::MemoryType::SDRAM:
            return "SDRAM";
        default:
            return "UNKNOWN";
    }
}

// Print a section header
void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

// Print a row with label and value
void printRow(const std::string& label, const std::string& value) {
    std::cout << "  " << std::left << std::setw(28) << label << ": " << value << "\n";
}

void printSeparator() {
    std::cout << "  " << std::string(56, '-') << "\n";
}

} // anonymous namespace

int main() {
    cf::MemoryInfo info;
    cf::getSystemMemoryInfo(info);

    // Print Physical Memory Information
    printHeader("PHYSICAL MEMORY");
    printRow("Total Memory", formatBytes(info.physical.total_bytes));
    printRow("Available Memory", formatBytes(info.physical.available_bytes));
    printRow("Free Memory", formatBytes(info.physical.free_bytes));

    // Calculate memory usage percentage
    if (info.physical.total_bytes > 0) {
        double used_percent =
            static_cast<double>(info.physical.total_bytes - info.physical.available_bytes) /
            info.physical.total_bytes * 100.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << used_percent << "%";
        printRow("Memory Usage", oss.str());
    }

    // Print Swap Memory Information
    printHeader("SWAP MEMORY");
    printRow("Total Swap", formatBytes(info.swap.total_bytes));
    printRow("Free Swap", formatBytes(info.swap.free_bytes));
    if (info.swap.total_bytes > 0) {
        uint64_t used = info.swap.total_bytes - info.swap.free_bytes;
        double used_percent = static_cast<double>(used) / info.swap.total_bytes * 100.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << used_percent << "%";
        printRow("Swap Usage", oss.str());
    }

    // Print Cached Memory Information (mainly Linux)
    printHeader("CACHED MEMORY");
    printRow("Buffers", formatBytes(info.cached.buffers_bytes));
    printRow("Cached", formatBytes(info.cached.cached_bytes));
    printRow("Shared", formatBytes(info.cached.shared_bytes));
    printRow("Slab", formatBytes(info.cached.slab_bytes));

    // Print Process Memory Information
    printHeader("PROCESS MEMORY");
    printRow("VM RSS (Resident Set Size)", formatBytes(info.process.vm_rss_bytes));
    printRow("VM Size (Virtual Memory)", formatBytes(info.process.vm_size_bytes));
    printRow("VM Peak", formatBytes(info.process.vm_peak_bytes));

    // Print DIMM Information
    if (!info.dimms.empty()) {
        printHeader("MEMORY MODULES (DIMM)");
        for (const auto& dimm : info.dimms) {
            std::cout << "\n";
            printRow("Channel", "Channel " + std::to_string(dimm.channel));
            printRow("Slot", "Slot " + std::to_string(dimm.slot));
            printSeparator();
            printRow("Capacity", formatBytes(dimm.capacity_bytes));
            printRow("Type", memoryTypeToString(dimm.type));
            printRow("Frequency", std::to_string(dimm.frequency_mhz) + " MHz");
            if (!dimm.manufacturer.empty()) {
                printRow("Manufacturer", dimm.manufacturer);
            }
            if (!dimm.part_number.empty()) {
                printRow("Part Number", dimm.part_number);
            }
            if (!dimm.serial_number.empty()) {
                printRow("Serial Number", dimm.serial_number);
            }
        }
    }

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  Memory Information Collection Complete\n";
    std::cout << std::string(60, '=') << "\n\n";

    return 0;
}
