/**
 * @file dimm_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief DIMM (Memory Module) Information Query Implementation (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "dimm_info.h"

#include <cstdio>
#include <cstring>
#include <memory>

namespace cf {
namespace linux_impl {

namespace {

/**
 * @brief Convert dmidecode memory type string to MemoryType enum.
 */
MemoryType parseMemoryType(const char* typeStr) {
    if (typeStr == nullptr) {
        return MemoryType::UNKNOWN;
    }

    // Case-insensitive comparison
    auto match = [](const char* str, const char* pattern) -> bool {
        if (str == nullptr || pattern == nullptr) return false;
        size_t len = strlen(pattern);
        if (strlen(str) < len) return false;
        return strncasecmp(str, pattern, len) == 0;
    };

    if (match(typeStr, "DDR3")) return MemoryType::DDR3;
    if (match(typeStr, "DDR4")) return MemoryType::DDR4;
    if (match(typeStr, "DDR5")) return MemoryType::DDR5;
    if (match(typeStr, "LPDDR3")) return MemoryType::LPDDR3;
    if (match(typeStr, "LPDDR4")) return MemoryType::LPDDR4;
    if (match(typeStr, "LPDDR4X")) return MemoryType::LPDDR4X;
    if (match(typeStr, "LPDDR5")) return MemoryType::LPDDR5;
    if (match(typeStr, "DDR2")) return MemoryType::DDR2;
    if (match(typeStr, "SDRAM")) return MemoryType::SDRAM;

    return MemoryType::UNKNOWN;
}

/**
 * @brief Parse memory size string (e.g., "16384 MB", "16 GB") to bytes.
 */
uint64_t parseMemorySize(const char* sizeStr) {
    if (sizeStr == nullptr || strcmp(sizeStr, "No Module Installed") == 0) {
        return 0;
    }

    unsigned long value = 0;
    char unit[16] = {0};

    if (sscanf(sizeStr, "%lu %15s", &value, unit) == 2) {
        if (strcasecmp(unit, "MB") == 0 || strcasecmp(unit, "M") == 0) {
            return value * 1024 * 1024;
        } else if (strcasecmp(unit, "GB") == 0 || strcasecmp(unit, "G") == 0) {
            return value * 1024 * 1024 * 1024;
        } else if (strcasecmp(unit, "KB") == 0 || strcasecmp(unit, "K") == 0) {
            return value * 1024;
        }
        // Default to MB if no unit specified
        return value * 1024 * 1024;
    }

    return 0;
}

/**
 * @brief Parse frequency string (e.g., "3200 MT/s", "3200 MHz") to MHz.
 */
uint32_t parseFrequency(const char* freqStr) {
    if (freqStr == nullptr || strcmp(freqStr, "Unknown") == 0) {
        return 0;
    }

    unsigned long value = 0;
    if (sscanf(freqStr, "%lu", &value) == 1) {
        return static_cast<uint32_t>(value);
    }

    return 0;
}

/**
 * @brief Read a file content to a string.
 */
std::string readFile(const char* path) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        return "";
    }

    char buffer[256] = {0};
    if (fgets(buffer, sizeof(buffer), fp)) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }
        fclose(fp);
        return std::string(buffer);
    }

    fclose(fp);
    return "";
}

/**
 * @brief Parse dmidecode output to extract DIMM information.
 */
bool parseDmidecode(const char* output, std::vector<DimmInfo>& dimms) {
    if (output == nullptr) {
        return false;
    }

    const char* p = output;
    DimmInfo currentDimm{};
    bool inDevice = false;
    bool hasSize = false;

    while (*p != '\0') {
        // Skip leading whitespace
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        // Check for "Memory Device" marker
        if (strncmp(p, "Memory Device", 13) == 0) {
            // Save previous device if valid
            if (inDevice && hasSize && currentDimm.capacity_bytes > 0) {
                dimms.push_back(currentDimm);
            }

            // Start new device
            currentDimm = DimmInfo{};
            inDevice = true;
            hasSize = false;

            // Skip to next line
            while (*p != '\0' && *p != '\n') p++;
            if (*p == '\n') p++;
            continue;
        }

        if (inDevice) {
            // Check for field indentation (typically fields start with 4-8 spaces)
            // Check for end of device (empty line or next handle)
            if (*p == '\n' || (*p == '\0')) {
                if (*p == '\n') p++;
                // Save device if valid
                if (hasSize && currentDimm.capacity_bytes > 0) {
                    dimms.push_back(currentDimm);
                }
                inDevice = false;
                continue;
            }

            // Parse fields
            auto extractField = [&p](const char* fieldName) -> char* {
                char* result = nullptr;
                size_t nameLen = strlen(fieldName);

                // Check for field pattern: "FieldName:  value"
                if (strncmp(p, fieldName, nameLen) == 0 && p[nameLen] == ':') {
                    const char* valueStart = p + nameLen + 1;
                    while (*valueStart == ' ' || *valueStart == '\t') {
                        valueStart++;
                    }

                    // Find end of line
                    const char* valueEnd = valueStart;
                    while (*valueEnd != '\0' && *valueEnd != '\n') {
                        valueEnd++;
                    }

                    // Allocate and copy value
                    size_t valueLen = valueEnd - valueStart;
                    result = new char[valueLen + 1];
                    memcpy(result, valueStart, valueLen);
                    result[valueLen] = '\0';

                    p = valueEnd;
                }
                return result;
            };

            std::unique_ptr<char[]> field;

            if ((field.reset(extractField("Size")), field)) {
                currentDimm.capacity_bytes = parseMemorySize(field.get());
                hasSize = true;
            } else if ((field.reset(extractField("Type")), field)) {
                currentDimm.type = parseMemoryType(field.get());
            } else if ((field.reset(extractField("Speed")), field)) {
                currentDimm.frequency_mhz = parseFrequency(field.get());
            } else if ((field.reset(extractField("Manufacturer")), field)) {
                currentDimm.manufacturer = field.get();
            } else if ((field.reset(extractField("Serial Number")), field)) {
                currentDimm.serial_number = field.get();
            } else if ((field.reset(extractField("Part Number")), field)) {
                currentDimm.part_number = field.get();
            } else if ((field.reset(extractField("Locator")), field)) {
                // Try to extract slot number from locator
                // Common formats: "DIMM0", "ChannelA-DIMM0", "Slot 1", etc.
                const char* slot = field.get();
                // Simple heuristic: find last digit
                const char* pDigit = slot + strlen(slot) - 1;
                while (pDigit >= slot && (*pDigit < '0' || *pDigit > '9')) {
                    pDigit--;
                }
                if (pDigit >= slot) {
                    currentDimm.slot = static_cast<uint8_t>(*pDigit - '0');
                }
            }
        }

        // Move to next line
        while (*p != '\0' && *p != '\n') {
            p++;
        }
        if (*p == '\n') {
            p++;
        }
    }

    // Don't forget last device
    if (inDevice && hasSize && currentDimm.capacity_bytes > 0) {
        dimms.push_back(currentDimm);
    }

    return !dimms.empty();
}

/**
 * @brief Query DIMM info using dmidecode command.
 */
bool queryDimmViaDmidecode(std::vector<DimmInfo>& dimms) {
    FILE* pipe = popen("dmidecode -t memory 2>/dev/null", "r");
    if (!pipe) {
        return false;
    }

    // Read entire output
    std::string output;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int status = pclose(pipe);
    if (status != 0) {
        return false;
    }

    return parseDmidecode(output.c_str(), dimms);
}

/**
 * @brief Query DIMM info from /sys/class/dmi/id/ (fallback).
 * This provides limited information but doesn't require root.
 */
bool queryDimmViaSysFs(std::vector<DimmInfo>& dimms) {
    // /sys/class/dmi/id/ provides system-level info, not per-DIMM info
    // We can get some basic board info but not detailed DIMM characteristics
    // This is a minimal fallback that returns at least one entry with available info

    std::string manufacturer = readFile("/sys/class/dmi/id/board_vendor");
    std::string serial = readFile("/sys/class/dmi/id/board_serial");
    std::string product = readFile("/sys/class/dmi/id/board_name");

    // Check if we have any meaningful info
    if (manufacturer.empty() && serial.empty() && product.empty()) {
        return false;
    }

    // Create a minimal DIMM entry
    DimmInfo dimm{};
    dimm.manufacturer = manufacturer;
    dimm.serial_number = serial;
    dimm.part_number = product;
    dimm.type = MemoryType::UNKNOWN;  // Can't determine from /sys
    dimm.capacity_bytes = 0;          // Can't get individual DIMM size from /sys
    dimm.slot = 0;

    // Only add if we have some info (but capacity will be 0)
    dimms.push_back(dimm);

    return true;
}

} // anonymous namespace

void queryDimmInfo(std::vector<DimmInfo>& dimms) {
    dimms.clear();

    // Try dmidecode first (requires root)
    if (!queryDimmViaDmidecode(dimms)) {
        // Fall back to /sys/class/dmi/id/
        queryDimmViaSysFs(dimms);
    }
}

} // namespace linux_impl
} // namespace cf
