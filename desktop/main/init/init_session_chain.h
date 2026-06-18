/**
 * @file init_session_chain.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Init Session Chains - DAG-based initialization stage execution chain
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "aex/weak_ptr/weak_ptr.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include "boot_stage_info.h"
#include "init_stage.h"
#include <QObject>
#include <list>
#include <memory>
#include <vector>

namespace cf::desktop::init_session {

/**
 * @brief Initialization session chain that manages multiple IInitStage executions
 *
 * Features:
 * - Automatic DAG-based dependency resolution with topological sorting
 * - Circular dependency detection and error reporting
 * - OK/PartialFailed continues execution, CriticalFailed stops immediately
 * - Detailed execution debug information
 */
class InitSessionChain : public QObject {
    Q_OBJECT
  public:
    InitSessionChain(QObject* parent = nullptr);
    ~InitSessionChain();

    /**
     * @brief Gets the global Chain's aex::WeakPtr reference (singleton pattern)
     */
    static aex::WeakPtr<InitSessionChain> GetChainRef();

    /**
     * @brief Adds an initialization stage to the end of the chain
     * @param[in] stage Unique pointer to the Stage object
     */
    void add_stage_back(std::unique_ptr<IInitStage> stage);

    /**
     * @brief Executes the initialization chain
     * @return Final execution result (OK/PartialFailed/CriticalFailed)
     *
     * Execution logic:
     * 1. Resolve dependency graph and generate topological sort
     * 2. Execute stages in order
     * 3. Stop immediately on CriticalFailed
     * 4. Log PartialFailed but continue execution
     */
    IInitStage::StageResult execute();

    /**
     * @brief Gets the dependency graph debug string (for debugging and logging)
     * @return Human-readable string describing dependency relationships
     */
    QString get_dependency_graph_debug_string() const;

    /**
     * @brief Resets the chain state, allowing re-execution
     */
    void reset();

    /**
     * @brief Finds a Stage by name and returns its aex::WeakPtr
     * @param[in] stage_name Stage name
     * @return Valid aex::WeakPtr<IInitStage> if found, invalid aex::WeakPtr otherwise
     */
    aex::WeakPtr<IInitStage> find_stage(std::string_view stage_name) const;

    /**
     * @brief Gets the number of stages in the chain
     * @return Number of stages
     */
    size_t size() const { return execution_order.size(); }

  signals:
    /**
     * @brief Signal emitted when a stage starts execution
     */
    void stage_start(const BootStageInfo& bs_info);

    /**
     * @brief Signal emitted when a stage completes execution
     * @param[in] stage_name Stage name
     * @param[in] success Whether the stage succeeded (including partial success)
     */
    void stage_completed(const QString& stage_name, bool success);

    /**
     * @brief Signal emitted when a stage partially fails
     * @param[in] stage_name Stage name
     * @param[in] message Failure message
     * @param[in] detail Error detail
     */
    void stage_partial_failed(const QString& stage_name, const QString& message,
                              const QString& detail);

    /**
     * @brief Signal emitted when the chain execution finishes
     * @param[in] final_result Final result
     */
    void chain_finished(const IInitStage::StageResult& final_result);

  private:
    /**
     * @brief Dependency graph node for topological sorting
     */
    struct GraphNode {
        IInitStage* stage;                     // Stage pointer
        std::vector<IInitStage*> dependencies; // List of dependent stages
        std::string stage_name;                // Stage name (for debugging)
        int in_degree;                         // In-degree (for Kahn's algorithm)
    };

    /**
     * @brief Builds the dependency graph
     * @return Sorted stage list on success, empty on failure (circular dependency)
     */
    std::vector<GraphNode> build_dependency_graph();

    /**
     * @brief Topological sort using Kahn's algorithm
     * @param graph Dependency graph
     * @return Sorted list of stage pointers
     */
    std::vector<IInitStage*> topological_sort(std::vector<GraphNode>& graph);

    /**
     * @brief Detects circular dependencies
     * @param graph Dependency graph
     * @return true if circular dependency exists
     */
    bool has_circular_dependency(const std::vector<GraphNode>& graph) const;

    /**
     * @brief Generates dependency graph debug string
     * @param graph Dependency graph
     * @return Formatted debug information string
     */
    QString generate_debug_string(const std::vector<GraphNode>& graph) const;

  private:
    std::list<std::unique_ptr<IInitStage>> stages;
    std::vector<IInitStage*> execution_order; // Topologically sorted execution order
    QString last_debug_string;                // Last dependency graph debug string
    bool has_run{false};
    bool is_valid{true}; // Whether the dependency graph is valid (no circular dependencies)

    aex::WeakPtrFactory<InitSessionChain> weak_ptr_factory_;
};

} // namespace cf::desktop::init_session
