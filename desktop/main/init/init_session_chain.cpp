/**
 * @file init_session.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Init Session Chain 实现
 * @version 0.1
 * @date 2026-03-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "init_session_chain.h"
#include "cflog.h"
#include "init_stage.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <sstream>
#include <unordered_map>

namespace cf::desktop::init_session {

// ============================================================================
// IInitStage::StageResult 静态工厂方法实现
// ============================================================================

IInitStage::StageResult IInitStage::StageResult::ok(const QString& msg) {
    return StageResult{StatusCode::OK, msg, QString()};
}

IInitStage::StageResult IInitStage::StageResult::partial_failed(const QString& msg,
                                                                const QString& detail) {
    return StageResult{StatusCode::PartialFailed, msg, detail};
}

IInitStage::StageResult IInitStage::StageResult::critical_failed(const QString& msg,
                                                                 const QString& detail) {
    return StageResult{StatusCode::CriticalFailed, msg, detail};
}

// ============================================================================
// IInitStage 默认实现
// ============================================================================

std::vector<aex::WeakPtr<IInitStage>> IInitStage::request_before_actions_init() const {
    return {}; // 默认无依赖
}

namespace {
aex::WeakPtr<InitSessionChain> g_chain_ref;
} // namespace

InitSessionChain::InitSessionChain(QObject* parent) : QObject(parent), weak_ptr_factory_(this) {}

InitSessionChain::~InitSessionChain() {
    if (!has_run) {
        execute();
    }
    g_chain_ref.Reset();
}

aex::WeakPtr<InitSessionChain> InitSessionChain::GetChainRef() {
    return g_chain_ref;
}

void InitSessionChain::add_stage_back(std::unique_ptr<IInitStage> stage) {
    if (!stage) {
        cf::log::trace("Attempted to add null stage, skipping");
        return;
    }

    stages.push_back(std::move(stage));
    has_run = false; // Need to rebuild dependency graph after adding new stage
    is_valid = true;
    execution_order.clear();
    cf::log::traceftag("InitSession", "Stage added, total stages: {}", stages.size());
}

void InitSessionChain::reset() {
    cf::log::traceftag("InitSession", "Resetting InitSessionChain");
    has_run = false;
    is_valid = true;
    execution_order.clear();
    last_debug_string.clear();
}

aex::WeakPtr<IInitStage> InitSessionChain::find_stage(std::string_view stage_name) const {
    for (const auto& stage : stages) {
        if (stage->name() == stage_name) {
            return stage->get_weak_ptr();
        }
    }
    return nullptr;
}

QString InitSessionChain::get_dependency_graph_debug_string() const {
    if (!last_debug_string.isEmpty()) {
        return last_debug_string;
    }

    // Build dependency graph first to get debug info if not yet run
    const_cast<InitSessionChain*>(this)->build_dependency_graph();
    return last_debug_string;
}

std::vector<InitSessionChain::GraphNode> InitSessionChain::build_dependency_graph() {
    cf::log::traceftag("InitSession", "Building dependency graph with {} stages", stages.size());
    std::vector<GraphNode> graph;
    graph.reserve(stages.size());

    // Step 1: Create all nodes
    for (auto& stage : stages) {
        GraphNode node;
        node.stage = stage.get();
        node.stage_name = std::string(stage->name());
        node.in_degree = 0;
        graph.push_back(std::move(node));
    }

    // Step 2: Build dependency relationships
    for (auto& node : graph) {
        auto deps = node.stage->request_before_actions_init();

        for (const auto& weak_dep : deps) {
            auto* dep = weak_dep.Get();
            if (!dep) {
                continue; // Invalid weak reference, skip
            }

            // Find the depended stage in graph
            auto it = std::find_if(graph.begin(), graph.end(),
                                   [dep](const GraphNode& n) { return n.stage == dep; });

            if (it != graph.end()) {
                node.dependencies.push_back(it->stage);
                node.in_degree++;
            }
        }
    }

    // Check for circular dependencies
    if (has_circular_dependency(graph)) {
        is_valid = false;
        cf::log::errorftag("InitSession", "Circular dependency detected!");
        last_debug_string = QStringLiteral("Error: Circular dependency detected!\n");
        return {};
    }

    // Generate debug string
    last_debug_string = generate_debug_string(graph);

    return graph;
}

QString InitSessionChain::generate_debug_string(const std::vector<GraphNode>& graph) const {
    std::ostringstream oss;
    oss << "=== Dependency Graph Structure ===\n";
    oss << "Total Stages: " << graph.size() << "\n\n";

    for (const auto& node : graph) {
        oss << "Stage: " << node.stage_name << "\n";
        oss << "  Dependencies: " << node.dependencies.size() << "\n";
        if (!node.dependencies.empty()) {
            oss << "  Dependency List: ";
            for (size_t i = 0; i < node.dependencies.size(); ++i) {
                auto dep_it =
                    std::find_if(graph.begin(), graph.end(), [&node, i](const GraphNode& n) {
                        return n.stage == node.dependencies[i];
                    });
                if (dep_it != graph.end()) {
                    oss << dep_it->stage_name;
                    if (i < node.dependencies.size() - 1)
                        oss << ", ";
                }
            }
            oss << "\n";
        }
        oss << "\n";
    }
    oss << "==================================\n";
    return QString::fromStdString(oss.str());
}

bool InitSessionChain::has_circular_dependency(const std::vector<GraphNode>& graph) const {
    // Use DFS to detect cycles
    std::unordered_map<IInitStage*, int> state; // 0=unvisited, 1=visiting, 2=visited

    std::function<bool(IInitStage*)> dfs = [&](IInitStage* stage) -> bool {
        auto it = std::find_if(graph.begin(), graph.end(),
                               [stage](const GraphNode& n) { return n.stage == stage; });
        if (it == graph.end())
            return false;

        state[stage] = 1; // Mark as visiting

        for (auto dep : it->dependencies) {
            if (state[dep] == 1)
                return true; // Back edge found, cycle exists
            if (state[dep] == 0 && dfs(dep))
                return true;
        }

        state[stage] = 2; // Mark as visited
        return false;
    };

    for (const auto& node : graph) {
        if (state[node.stage] == 0) {
            if (dfs(node.stage))
                return true;
        }
    }

    return false;
}

std::vector<IInitStage*> InitSessionChain::topological_sort(std::vector<GraphNode>& graph) {
    cf::log::traceftag("InitSession", "Performing topological sort");
    std::vector<IInitStage*> result;
    result.reserve(graph.size());

    // Kahn's algorithm
    std::queue<IInitStage*> queue;

    // Enqueue all nodes with in-degree 0
    for (auto& node : graph) {
        if (node.in_degree == 0) {
            queue.push(node.stage);
        }
    }

    while (!queue.empty()) {
        auto* current = queue.front();
        queue.pop();
        result.push_back(current);

        // Decrease in-degree for all nodes that depend on current node
        for (auto& node : graph) {
            auto it = std::find(node.dependencies.begin(), node.dependencies.end(), current);
            if (it != node.dependencies.end()) {
                node.in_degree--;
                if (node.in_degree == 0) {
                    queue.push(node.stage);
                }
            }
        }
    }

    // If result size doesn't equal node count, circular dependency exists
    if (result.size() != graph.size()) {
        cf::log::errorftag("InitSession", "Topological sort failed - circular dependency");
        return {};
    }

    cf::log::traceftag("InitSession", "Topological sort completed, order: {}", result.size());
    return result;
}

IInitStage::StageResult InitSessionChain::execute() {
    cf::log::traceftag("InitSession", "Executing InitSessionChain");
    // Update global reference
    g_chain_ref = weak_ptr_factory_.GetWeakPtr();

    if (has_run) {
        return IInitStage::StageResult::critical_failed(
            QStringLiteral("Execution failed"),
            QStringLiteral("InitSessionChain already executed, call reset() first"));
    }

    if (stages.empty()) {
        has_run = true;
        emit chain_finished(IInitStage::StageResult::ok(QStringLiteral("No stages to execute")));
        return IInitStage::StageResult::ok(QStringLiteral("No stages to execute"));
    }

    // Build dependency graph
    auto graph = build_dependency_graph();
    if (!is_valid) {
        has_run = true;
        auto result = IInitStage::StageResult::critical_failed(
            QStringLiteral("Failed to build dependency graph"), last_debug_string);
        emit chain_finished(result);
        return result;
    }

    // Topological sort
    execution_order = topological_sort(graph);
    if (execution_order.empty()) {
        has_run = true;
        auto result = IInitStage::StageResult::critical_failed(
            QStringLiteral("Topological sort failed"),
            QStringLiteral("Unresolvable dependency relationship"));
        emit chain_finished(result);
        return result;
    }

    has_run = true;

    // Execute all stages
    IInitStage::StageResult final_result = IInitStage::StageResult::ok();
    int success_count = 0;
    int partial_failed_count = 0;

    for (size_t i = 0; i < execution_order.size(); ++i) {
        auto* stage = execution_order[i];
        QString stage_name = QString::fromStdString(std::string(stage->name()));

        cf::log::traceftag("InitSession", "Executing stage [{}/{}]: {}", i + 1,
                           execution_order.size(), stage_name.toStdString());

        // Send start signal
        BootStageInfo info{stage_name, static_cast<int>(i)};
        emit stage_start(info);

        // Execute stage
        auto result = stage->run_session();

        // Handle result
        switch (result.code) {
            case IInitStage::StatusCode::OK:
                success_count++;
                cf::log::traceftag("InitSession", "Stage '{}' completed successfully",
                                   stage_name.toStdString());
                emit stage_completed(stage_name, true);
                break;

            case IInitStage::StatusCode::PartialFailed:
                partial_failed_count++;
                cf::log::warningftag("InitSession", "Stage '{}' partially failed: {}",
                                     stage_name.toStdString(), result.message.toStdString());
                emit stage_completed(stage_name, false);
                emit stage_partial_failed(stage_name, result.message, result.error_detail);
                // Update final_result to partial_failed (if previously ok)
                if (final_result.code == IInitStage::StatusCode::OK) {
                    final_result = result;
                }
                break;

            case IInitStage::StatusCode::CriticalFailed:
                cf::log::errorftag("InitSession", "Stage '{}' critically failed: {}",
                                   stage_name.toStdString(), result.error_detail.toStdString());
                emit stage_completed(stage_name, false);
                // Stop execution immediately
                final_result = IInitStage::StageResult::critical_failed(
                    QStringLiteral("Initialization failed at stage '") + stage_name +
                        QStringLiteral("'"),
                    result.error_detail);
                emit chain_finished(final_result);
                return final_result;
        }
    }

    // All stages completed, generate final result
    if (partial_failed_count > 0) {
        final_result = IInitStage::StageResult::partial_failed(
            QStringLiteral("Initialization completed with ") +
                QString::number(partial_failed_count) + QStringLiteral(" stages partially failed"),
            QStringLiteral("Success: ") + QString::number(success_count) +
                QStringLiteral(", Partial Failed: ") + QString::number(partial_failed_count));
    } else {
        final_result =
            IInitStage::StageResult::ok(QStringLiteral("All ") + QString::number(success_count) +
                                        QStringLiteral(" stages initialized successfully"));
    }

    cf::log::infoftag("InitSession",
                      "InitSessionChain execution completed: {} success, {} partial failed",
                      success_count, partial_failed_count);
    emit chain_finished(final_result);
    return final_result;
}

} // namespace cf::desktop::init_session
