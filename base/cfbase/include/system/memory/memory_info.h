/**
 * @file memory_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Memory Detections Using for System Detection
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "base/export.h"
#include <cstdint>
#include <string>
#include <vector>

namespace cf {

/**
 * @brief Memory type enumeration.
 */
enum class MemoryType : uint8_t {
    UNKNOWN = 0,
    DDR3,
    DDR4,
    DDR5,
    LPDDR3,
    LPDDR4,
    LPDDR4X,
    LPDDR5,
    DDR2,
    SDRAM
};

/**
 * @brief DIMM (Memory Module) information.
 */
struct DimmInfo {
    uint64_t capacity_bytes;   ///< Capacity in bytes.
    MemoryType type;           ///< Memory type (DDR3/DDR4/LPDDR4, etc.).
    uint32_t frequency_mhz;    ///< Operating frequency in MHz.
    std::string manufacturer;  ///< Manufacturer name (optional, empty if unavailable).
    std::string part_number;   ///< Part number (optional, empty if unavailable).
    std::string serial_number; ///< Serial number (optional, empty if unavailable).
    uint8_t channel;           ///< Memory channel number (0-based).
    uint8_t slot;              ///< Slot number on the channel (0-based).
};

/**
 * @brief Physical memory information.
 */
struct PhysicalMemory {
    uint64_t total_bytes;     ///< Total physical memory in bytes.
    uint64_t available_bytes; ///< Available physical memory in bytes.
    uint64_t free_bytes;      ///< Free physical memory in bytes.
};

/**
 * @brief Virtual memory / Swap space information.
 */
struct SwapMemory {
    uint64_t total_bytes; ///< Total swap space in bytes.
    uint64_t free_bytes;  ///< Free swap space in bytes.
};

/**
 * @brief Linux-specific cached memory information.
 */
struct CachedMemory {
    uint64_t buffers_bytes; ///< Memory used for buffers (in bytes).
    uint64_t cached_bytes;  ///< Memory used for page cache (in bytes).
    uint64_t shared_bytes;  ///< Memory used for shared memory (Shmem, in bytes).
    uint64_t slab_bytes;    ///< Memory used for kernel data structures (in bytes).
};

/**
 * @brief Process memory usage information.
 */
struct ProcessMemory {
    uint64_t vm_rss_bytes;  ///< Resident Set Size - actual physical memory used.
    uint64_t vm_size_bytes; ///< Virtual memory size (total virtual memory allocated).
    uint64_t vm_peak_bytes; ///< Peak virtual memory size.
};

/**
 * @brief Comprehensive memory information for system detection.
 */
struct MemoryInfo {
    PhysicalMemory physical;     ///< Physical memory statistics.
    SwapMemory swap;             ///< Swap space statistics.
    CachedMemory cached;         ///< Cached/buffered memory (mainly Linux).
    ProcessMemory process;       ///< Current process memory usage.
    std::vector<DimmInfo> dimms; ///< List of memory modules (DIMM info).
};

/**
 * @brief  Retrieves comprehensive system memory information.
 *
 * Populates the provided MemoryInfo structure with physical memory,
 * swap space, cached memory, process memory usage, and DIMM module
 * information.
 *
 * @param[out] info  MemoryInfo structure to populate with memory data.
 *
 * @return          void (populates output parameter).
 *
 * @throws          None
 *
 * @note            None
 *
 * @warning         None
 *
 * @since           0.1
 * @ingroup         system_memory
 */
void CF_BASE_EXPORT getSystemMemoryInfo(MemoryInfo& info);

} // namespace cf
