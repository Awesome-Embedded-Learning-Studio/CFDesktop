/**
 * @file dimm_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief DIMM (Memory Module) Information Query Implementation via SMBIOS (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "dimm_info.h"
#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <vector>

namespace cf {
namespace win_impl {

namespace {

// ============================================================
// SMBIOS Structures
// ============================================================

#pragma pack(push, 1)

/**
 * @brief Raw SMBIOS data header returned by GetSystemFirmwareTable.
 */
struct RawSMBIOSData {
    uint8_t Used20CallingMethod;
    uint8_t SMBIOSMajorVersion;
    uint8_t SMBIOSMinorVersion;
    uint8_t DmiRevision;
    uint32_t Length;
    uint8_t SMBIOSTableData[];
};

/**
 * @brief Generic SMBIOS structure header.
 */
struct SMBIOSHeader {
    uint8_t Type;
    uint8_t Length;
    uint16_t Handle;
};

/**
 * @brief SMBIOS Type 17 - Memory Device.
 * Format based on SMBIOS 3.2 specification.
 */
struct MemoryDevice {
    SMBIOSHeader Header;
    uint16_t PhysMemArrayHandle; // Offset 0x04
    uint16_t MemErrorInfoHandle; // Offset 0x06
    uint16_t TotalWidth;         // Offset 0x08
    uint16_t DataWidth;          // Offset 0x0A
    uint16_t Size;               // Offset 0x0C
    uint8_t FormFactor;          // Offset 0x0E
    uint8_t DeviceSet;           // Offset 0x0F
    uint8_t DeviceLocator;       // Offset 0x10 (string index)
    uint8_t BankLocator;         // Offset 0x11 (string index)
    uint8_t MemoryType;          // Offset 0x12
    uint16_t TypeDetail;         // Offset 0x13
    uint16_t Speed;              // Offset 0x15
    uint8_t Manufacturer;        // Offset 0x17 (string index)
    uint8_t SerialNumber;        // Offset 0x18 (string index)
    uint8_t AssetTag;            // Offset 0x19 (string index)
    uint8_t PartNumber;          // Offset 0x1A (string index)
    // uint32_t Attributes;           // Offset 0x1B (SMBIOS 2.8+)
    // uint32_t ExtendedSize;         // Offset 0x1F (SMBIOS 3.2+)
    // uint16_t ConfiguredSpeed;      // Offset 0x23 (SMBIOS 3.2+)
};

#pragma pack(pop)

// ============================================================
// SMBIOS Memory Type Enum
// ============================================================

enum class SMBiosMemoryType : uint8_t {
    Other = 0x01,
    Unknown = 0x02,
    DRAM = 0x03,
    EDRAM = 0x04,
    VRAM = 0x05,
    SRAM = 0x06,
    RAM = 0x07,
    ROM = 0x08,
    FLASH = 0x09,
    EEPROM = 0x0A,
    FEPROM = 0x0B,
    EPROM = 0x0C,
    CDRAM = 0x0D,
    _3DRAM = 0x0E,
    SDRAM = 0x0F,
    DDR = 0x10,
    DDR2 = 0x11,
    DDR2_FBDIMM = 0x12,
    DDR3 = 0x13,
    FBD2 = 0x14,
    DDR4 = 0x14,
    LPDDR = 0x15,
    LPDDR2 = 0x16,
    LPDDR3 = 0x17,
    LPDDR4 = 0x18,
    LPDDR4_X = 0x19,
    LPDDR5 = 0x1A,
    DDR5 = 0x1B,
    LPDDR5_X = 0x1C
};

/**
 * @brief Convert SMBIOS memory type to our MemoryType enum.
 */
MemoryType smbiosToMemoryType(uint8_t smbType) {
    switch (static_cast<SMBiosMemoryType>(smbType)) {
        case SMBiosMemoryType::DDR2:
            return MemoryType::DDR2;
        case SMBiosMemoryType::DDR3:
            return MemoryType::DDR3;
        case SMBiosMemoryType::DDR4:
            return MemoryType::DDR4;
        case SMBiosMemoryType::DDR5:
            return MemoryType::DDR5;
        case SMBiosMemoryType::LPDDR3:
            return MemoryType::LPDDR3;
        case SMBiosMemoryType::LPDDR4:
            return MemoryType::LPDDR4;
        case SMBiosMemoryType::LPDDR4_X:
            return MemoryType::LPDDR4X;
        case SMBiosMemoryType::LPDDR5:
            return MemoryType::LPDDR5;
        case SMBiosMemoryType::SDRAM:
            return MemoryType::SDRAM;
        default:
            return MemoryType::UNKNOWN;
    }
}

/**
 * @brief Get string from SMBIOS string table.
 * String indexes are 1-based; 0 means no string.
 */
const char* getSmbiosString(const uint8_t* structStart, uint8_t stringIndex,
                            const uint8_t* tableEnd) {
    if (stringIndex == 0)
        return "";

    const uint8_t* fmtEnd = structStart + structStart[1]; // structStart[1] = Length
    const uint8_t* strStart = fmtEnd;

    // Skip to the requested string (1-based)
    for (uint8_t i = 1; i < stringIndex; ++i) {
        while (*strStart != 0 && strStart < tableEnd) {
            ++strStart;
        }
        if (*strStart == 0)
            ++strStart; // Skip null terminator
    }

    // Check if we're past the table
    if (strStart >= tableEnd)
        return "";

    return reinterpret_cast<const char*>(strStart);
}

/**
 * @brief Parse SMBIOS Type 17 (Memory Device) structures.
 */
void parseSmbiosMemoryDevices(const uint8_t* data, uint32_t length, std::vector<DimmInfo>& dimms) {
    const uint8_t* p = data;
    const uint8_t* end = data + length;

    while (p + sizeof(SMBIOSHeader) <= end) {
        const SMBIOSHeader* hdr = reinterpret_cast<const SMBIOSHeader*>(p);

        // End of table marker
        if (hdr->Type == 127 && hdr->Length == 4) {
            break;
        }

        // Find next structure (skip formatted section + strings)
        const uint8_t* next = p + hdr->Length;
        if (next >= end)
            break;

        // Skip string section (two consecutive null bytes mark end)
        while (next + 1 < end && !(next[0] == 0 && next[1] == 0)) {
            ++next;
        }
        next += 2; // Skip the double null terminator

        // Process Type 17 (Memory Device)
        if (hdr->Type == 17 && hdr->Length >= 0x15) {
            const MemoryDevice* memDev = reinterpret_cast<const MemoryDevice*>(p);

            // Check if memory device is present (Size != 0)
            // Size is in MB, or 0x7FFF for extended size (32GB+)
            uint16_t sizeValue = memDev->Size;
            if (sizeValue == 0 || (sizeValue & 0x8000) != 0) {
                // Either empty or uses extended size field
                // For now, skip if not present
                if (sizeValue != 0) {
                    // Bit 15 set: check extended size
                    if (hdr->Length >= 0x23) {
                        const uint8_t* extSizePtr = p + 0x1F; // ExtendedSize offset
                        uint32_t extSize = *reinterpret_cast<const uint32_t*>(extSizePtr);
                        if (extSize == 0) {
                            p = next;
                            continue;
                        }
                    }
                } else {
                    p = next;
                    continue;
                }
            }

            DimmInfo dimm{};
            dimm.type = smbiosToMemoryType(memDev->MemoryType);

            // Parse size (in MB)
            if ((sizeValue & 0x8000) == 0) {
                dimm.capacity_bytes = static_cast<uint64_t>(sizeValue) * 1024 * 1024;
            } else {
                // Extended size field
                if (hdr->Length >= 0x23) {
                    uint32_t extSize = *reinterpret_cast<const uint32_t*>(p + 0x1F);
                    dimm.capacity_bytes = static_cast<uint64_t>(extSize) * 1024 * 1024;
                }
            }

            // Speed (offset 0x15, present if Length > 0x15)
            if (hdr->Length > 0x15) {
                dimm.frequency_mhz = memDev->Speed;
            } else if (hdr->Length >= 0x25) {
                // ConfiguredClockSpeed at offset 0x23 (SMBIOS 3.2+)
                uint16_t configuredSpeed = *reinterpret_cast<const uint16_t*>(p + 0x23);
                dimm.frequency_mhz = configuredSpeed;
            }

            // String fields (Manufacturer, SerialNumber, PartNumber, DeviceLocator)
            if (hdr->Length >= 0x1C) {
                const char* manufacturer = getSmbiosString(p, memDev->Manufacturer, end);
                dimm.manufacturer = manufacturer;

                const char* serial = getSmbiosString(p, memDev->SerialNumber, end);
                dimm.serial_number = serial;

                const char* partNum = getSmbiosString(p, memDev->PartNumber, end);
                dimm.part_number = partNum;
            }

            const char* locator = getSmbiosString(p, memDev->DeviceLocator, end);
            // Try to extract slot/Channel info from locator string
            // Common formats: "DIMM0", "ChannelA-DIMM0", "Slot 1", etc.
            // For now, just count based on position
            dimm.slot = static_cast<uint8_t>(dimms.size());

            dimms.push_back(dimm);
        }

        p = next;
    }
}

} // anonymous namespace

void queryDimmInfo(std::vector<DimmInfo>& dimms) {
    // 'RSMB' = Raw SMBIOS
    const DWORD signature = 'RSMB';

    // Get buffer size
    DWORD bufferSize = GetSystemFirmwareTable(signature, 0, nullptr, 0);
    if (bufferSize == 0) {
        return;
    }

    // Allocate buffer
    std::vector<uint8_t> buffer(bufferSize);
    DWORD result = GetSystemFirmwareTable(signature, 0, buffer.data(), bufferSize);
    if (result != bufferSize) {
        return;
    }

    // Parse SMBIOS data
    const RawSMBIOSData* smbios = reinterpret_cast<const RawSMBIOSData*>(buffer.data());
    parseSmbiosMemoryDevices(smbios->SMBIOSTableData, smbios->Length, dimms);
}

} // namespace win_impl
} // namespace cf
