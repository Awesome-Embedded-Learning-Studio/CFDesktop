/**
 * @file test_config.h
 * @brief Test configuration for CFLogger tests
 *
 * Defines compile-time flags to control test scale and duration.
 *
 * @date 2026-03-16
 */
#pragma once

// =============================================================================
// Stress Test Configuration
// =============================================================================

/**
 * @brief Define this macro to enable stress tests with large data volumes
 *
 * When defined:
 * - Concurrent tests use more threads and iterations
 * - Queue overflow tests use larger message counts
 * - Performance tests run longer
 *
 * When undefined (default):
 * - Tests use reduced scale for fast CI/CD (<1 sec per test suite)
 * - Core functionality is still thoroughly tested
 */
// #define CFLOGGER_STRESS_TEST

#ifdef CFLOGGER_STRESS_TEST
// Stress test configuration - large scale
#    define CFLOGGER_CONCURRENCY_THREADS 16
#    define CFLOGGER_LOGS_PER_THREAD 10000
#    define CFLOGGER_QUEUE_FLOOD_COUNT 70000
#    define CFLOGGER_STRESS_WAIT_MS 500
#else
// Fast CI/CD configuration (default) - target <1s per test suite
#    define CFLOGGER_CONCURRENCY_THREADS 2
#    define CFLOGGER_LOGS_PER_THREAD 50
#    define CFLOGGER_QUEUE_FLOOD_COUNT 1000
#    define CFLOGGER_STRESS_WAIT_MS 20
#endif

// Queue capacity (must match AsyncPostQueue::kMaxNormalQueueSize)
#define CFLOGGER_QUEUE_CAPACITY 65536
