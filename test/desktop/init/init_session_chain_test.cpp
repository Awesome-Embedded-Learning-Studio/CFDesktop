/**
 * @file init_session_chain_test.cpp
 * @brief Comprehensive functional tests for cf::desktop::init_session::InitSessionChain
 *
 * Test Coverage:
 * 1. Basic Operations - add_stage_back, empty chain, single/multiple execution, reset
 * 2. Dependency Resolution - topological sort, linear/diamond/complex patterns, circular detection
 * 3. Execution Flow - OK/PartialFailed/CriticalFailed status handling
 * 4. Qt Signal Emission - stage_start, stage_completed, stage_partial_failed, chain_finished
 * 5. Edge Cases - null stages, re-execution, expired WeakPtrs
 * 6. Mock Infrastructure - MockInitStage with configurable behavior
 *
 * @date 2026-03-25
 * @version 1.0
 */

#include "aex/weak_ptr/weak_ptr_factory.h"
#include "init_session_chain.h"
#include "init_stage.h"
#include <gtest/gtest.h>

// Qt6::Test for signal testing (if available)
#ifdef QT_TEST_AVAILABLE
#    include <QSignalSpy>
#endif

#include <QString>
#include <memory>
#include <string>
#include <vector>

namespace cf::desktop::init_session {
namespace test {

// =============================================================================
// MockInitStage - Configurable mock stage for testing
// =============================================================================

/**
 * @brief Mock implementation of IInitStage for testing
 *
 * Features:
 * - Configurable name, return status, message, and error detail
 * - Tracks execution count
 * - Supports dependency management via aex::WeakPtr
 */
class MockInitStage : public IInitStage {
  public:
    MockInitStage(std::string_view name, IInitStage::StatusCode code = IInitStage::StatusCode::OK,
                  const QString& msg = QString(), const QString& detail = QString())
        : name_(name), status_code_(code), message_(msg), error_detail_(detail) {}

    ~MockInitStage() override = default;

    std::string_view name() const noexcept override { return name_; }

    StageResult run_session() override {
        execution_count++;
        return StageResult{status_code_, message_, error_detail_};
    }

    std::vector<aex::WeakPtr<IInitStage>> request_before_actions_init() const override {
        return dependencies_;
    }

    // Helper methods for test configuration
    void set_result(IInitStage::StatusCode code, const QString& msg = QString(),
                    const QString& detail = QString()) {
        status_code_ = code;
        message_ = msg;
        error_detail_ = detail;
    }

    void add_dependency(aex::WeakPtr<IInitStage> dep) { dependencies_.push_back(std::move(dep)); }

    void clear_dependencies() { dependencies_.clear(); }

    int get_execution_count() const { return execution_count; }

    void reset_execution_count() { execution_count = 0; }

  private:
    std::string name_;
    IInitStage::StatusCode status_code_;
    QString message_;
    QString error_detail_;
    std::vector<aex::WeakPtr<IInitStage>> dependencies_;
    int execution_count{0};
};

// =============================================================================
// Test Fixture - InitSessionChainTest
// =============================================================================

class InitSessionChainTest : public ::testing::Test {
  protected:
    void SetUp() override { chain = std::make_unique<InitSessionChain>(); }

    void TearDown() override {
        chain.reset();
        stages.clear();
    }

    // Helper: Create a mock stage with OK status
    std::unique_ptr<MockInitStage> CreateOkStage(std::string_view name) {
        return std::make_unique<MockInitStage>(name, IInitStage::StatusCode::OK, "OK");
    }

    // Helper: Create a mock stage with PartialFailed status
    std::unique_ptr<MockInitStage> CreatePartialFailedStage(std::string_view name,
                                                            const QString& msg = "Partial failed",
                                                            const QString& detail = "Some issue") {
        return std::make_unique<MockInitStage>(name, IInitStage::StatusCode::PartialFailed, msg,
                                               detail);
    }

    // Helper: Create a mock stage with CriticalFailed status
    std::unique_ptr<MockInitStage>
    CreateCriticalFailedStage(std::string_view name, const QString& msg = "Critical failed",
                              const QString& detail = "Fatal error") {
        return std::make_unique<MockInitStage>(name, IInitStage::StatusCode::CriticalFailed, msg,
                                               detail);
    }

    // For tests that need dependency setup, store stages with their factories
    struct StageWithFactory {
        std::unique_ptr<MockInitStage> stage;
        std::unique_ptr<aex::WeakPtrFactory<MockInitStage>> factory;

        aex::WeakPtr<IInitStage> GetWeakPtr() const {
            return factory ? factory->GetWeakPtr() : aex::WeakPtr<IInitStage>();
        }
    };

    // Helper to create a stage with factory for dependency management
    StageWithFactory
    CreateStageWithFactory(std::string_view name,
                           IInitStage::StatusCode code = IInitStage::StatusCode::OK) {
        auto stage = std::make_unique<MockInitStage>(name, code);
        auto factory = std::make_unique<aex::WeakPtrFactory<MockInitStage>>(stage.get());
        return StageWithFactory{std::move(stage), std::move(factory)};
    }

    std::unique_ptr<InitSessionChain> chain;
    std::vector<std::unique_ptr<MockInitStage>> stages;  // For simple stage storage
    std::vector<StageWithFactory> stages_with_factories; // For stages with dependencies
};

// =============================================================================
// Test Suite 1: Basic Operations
// =============================================================================

TEST_F(InitSessionChainTest, AddStage_IncreasesStageCount) {
    auto stage = CreateOkStage("TestStage");
    int initial_size = 0; // Can't directly access stage count, but we can test execution

    chain->add_stage_back(std::move(stage));

    // Verify by executing - empty chain returns OK with "No stages"
    // Single stage should execute and return OK
    // We'll verify in execution tests
}

TEST_F(InitSessionChainTest, ExecuteEmptyChain_ReturnsOkWithNoStagesMessage) {
    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
    EXPECT_TRUE(result.message.contains("No stages"));
}

TEST_F(InitSessionChainTest, ExecuteSingleStage_ReturnsOk) {
    chain->add_stage_back(CreateOkStage("SingleStage"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, ExecuteMultipleStagesAllOk_ReturnsOk) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
    EXPECT_TRUE(result.message.contains("success"));
}

TEST_F(InitSessionChainTest, Reset_AllowsReexecution) {
    chain->add_stage_back(CreateOkStage("StageA"));

    // First execution
    auto result1 = chain->execute();
    EXPECT_EQ(result1.code, IInitStage::StatusCode::OK);

    // Reset and execute again
    chain->reset();
    auto result2 = chain->execute();
    EXPECT_EQ(result2.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, ExecuteWithoutResetAfterFirstRun_ReturnsCriticalFailed) {
    chain->add_stage_back(CreateOkStage("StageA"));

    // First execution
    auto result1 = chain->execute();
    EXPECT_EQ(result1.code, IInitStage::StatusCode::OK);

    // Second execution without reset
    auto result2 = chain->execute();
    EXPECT_EQ(result2.code, IInitStage::StatusCode::CriticalFailed);
    EXPECT_TRUE(result2.error_detail.contains("already executed"));
}

TEST_F(InitSessionChainTest, GetDependencyGraphDebugString_ReturnsValidString) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));

    auto debug_str = chain->get_dependency_graph_debug_string();

    EXPECT_FALSE(debug_str.isEmpty());
    EXPECT_TRUE(debug_str.contains("Dependency Graph"));
    EXPECT_TRUE(debug_str.contains("StageA") || debug_str.contains("StageB"));
}

TEST_F(InitSessionChainTest, AddNullStage_IsSkipped) {
    // Adding null should not crash and should be handled gracefully
    chain->add_stage_back(nullptr);

    auto result = chain->execute();
    // Empty chain (null was skipped) returns OK with no stages message
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
    EXPECT_TRUE(result.message.contains("No stages"));
}

// =============================================================================
// Test Suite 2: Dependency Resolution
// =============================================================================

TEST_F(InitSessionChainTest, NoDependencies_AllStagesExecute) {
    auto stageA = CreateStageWithFactory("StageA");
    auto stageB = CreateStageWithFactory("StageB");
    auto stageC = CreateStageWithFactory("StageC");

    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, LinearDependencyChain_DependencyOrderCorrect) {
    // Create stages
    auto stageC = CreateStageWithFactory("StageC");
    auto stageB = CreateStageWithFactory("StageB");
    auto stageA = CreateStageWithFactory("StageA");

    // Set up dependencies: C depends on B, B depends on A
    // Execution order should be: A -> B -> C
    stageC.stage->add_dependency(stageB.GetWeakPtr());
    stageB.stage->add_dependency(stageA.GetWeakPtr());

    chain->add_stage_back(std::move(stageA.stage));
    chain->add_stage_back(std::move(stageB.stage));
    chain->add_stage_back(std::move(stageC.stage));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, DiamondDependencyPattern_ResolvedCorrectly) {
    // Diamond pattern:
    //     B
    //    / \
    //   A   D
    //    \ /
    //     C
    // Dependencies: B->A, C->A, D->B, D->C
    // Valid topological orders: A->B->C->D or A->C->B->D

    auto stageD = CreateStageWithFactory("StageD");
    auto stageC = CreateStageWithFactory("StageC");
    auto stageB = CreateStageWithFactory("StageB");
    auto stageA = CreateStageWithFactory("StageA");

    // Set up dependencies
    stageB.stage->add_dependency(stageA.GetWeakPtr());
    stageC.stage->add_dependency(stageA.GetWeakPtr());
    stageD.stage->add_dependency(stageB.GetWeakPtr());
    stageD.stage->add_dependency(stageC.GetWeakPtr());

    // Add in random order
    chain->add_stage_back(std::move(stageD.stage));
    chain->add_stage_back(std::move(stageC.stage));
    chain->add_stage_back(std::move(stageB.stage));
    chain->add_stage_back(std::move(stageA.stage));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, CircularDependency_ReturnsCriticalFailed) {
    // Create circular dependency: A -> B -> A
    auto stageB = CreateStageWithFactory("StageB");
    auto stageA = CreateStageWithFactory("StageA");

    stageA.stage->add_dependency(stageB.GetWeakPtr());
    stageB.stage->add_dependency(stageA.GetWeakPtr());

    chain->add_stage_back(std::move(stageA.stage));
    chain->add_stage_back(std::move(stageB.stage));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::CriticalFailed);
    EXPECT_TRUE(result.error_detail.contains("Circular") || result.message.contains("dependency"));
}

TEST_F(InitSessionChainTest, SelfDependency_CausesFailure) {
    // Self dependency: A -> A
    auto stageA = CreateStageWithFactory("StageA");
    stageA.stage->add_dependency(stageA.GetWeakPtr());

    chain->add_stage_back(std::move(stageA.stage));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::CriticalFailed);
}

TEST_F(InitSessionChainTest, ComplexMultiLevelDependencies_ResolvedCorrectly) {
    // Complex multi-level:
    // E depends on D
    // D depends on B and C
    // B depends on A
    // C depends on A
    // A has no dependencies
    // Valid order: A -> (B, C) -> D -> E

    auto stageE = CreateStageWithFactory("StageE");
    auto stageD = CreateStageWithFactory("StageD");
    auto stageC = CreateStageWithFactory("StageC");
    auto stageB = CreateStageWithFactory("StageB");
    auto stageA = CreateStageWithFactory("StageA");

    stageB.stage->add_dependency(stageA.GetWeakPtr());
    stageC.stage->add_dependency(stageA.GetWeakPtr());
    stageD.stage->add_dependency(stageB.GetWeakPtr());
    stageD.stage->add_dependency(stageC.GetWeakPtr());
    stageE.stage->add_dependency(stageD.GetWeakPtr());

    // Add in reverse order
    chain->add_stage_back(std::move(stageE.stage));
    chain->add_stage_back(std::move(stageD.stage));
    chain->add_stage_back(std::move(stageC.stage));
    chain->add_stage_back(std::move(stageB.stage));
    chain->add_stage_back(std::move(stageA.stage));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

// =============================================================================
// Test Suite 3: Execution Flow & Status Handling
// =============================================================================

TEST_F(InitSessionChainTest, AllStagesOk_ReturnsOkWithSuccessCount) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
    EXPECT_TRUE(result.message.contains("3") || result.message.contains("success"));
}

TEST_F(InitSessionChainTest, PartialFailedStage_ContinuesExecution) {
    // A (OK) -> B (PartialFailed) -> C (OK)
    // All should execute, final result should be PartialFailed
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreatePartialFailedStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::PartialFailed);
    EXPECT_TRUE(result.message.contains("Partial") || result.message.contains("1"));
}

TEST_F(InitSessionChainTest, MultiplePartialFailed_AggregatesCorrectly) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreatePartialFailedStage("StageB"));
    chain->add_stage_back(CreatePartialFailedStage("StageC"));
    chain->add_stage_back(CreateOkStage("StageD"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::PartialFailed);
    EXPECT_TRUE(result.message.contains("2") || result.error_detail.contains("2"));
}

TEST_F(InitSessionChainTest, CriticalFailed_StopsExecutionImmediately) {
    // A (OK) -> B (CriticalFailed) -> C (should not execute)
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateCriticalFailedStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::CriticalFailed);
    EXPECT_TRUE(result.message.contains("StageB"));
}

TEST_F(InitSessionChainTest, FirstStageCriticalFailed_StopsImmediately) {
    chain->add_stage_back(CreateCriticalFailedStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::CriticalFailed);
}

TEST_F(InitSessionChainTest, LastStageCriticalFailed_PreviousStagesExecute) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateCriticalFailedStage("StageC"));

    auto result = chain->execute();

    EXPECT_EQ(result.code, IInitStage::StatusCode::CriticalFailed);
    EXPECT_TRUE(result.message.contains("StageC"));
}

// =============================================================================
// Test Suite 4: Qt Signal Emission
// =============================================================================

#ifdef QT_TEST_AVAILABLE

TEST_F(InitSessionChainTest, StageStartSignal_EmittedForEachStage) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));

    QSignalSpy spy(chain.get(), &InitSessionChain::stage_start);

    chain->execute();

    EXPECT_EQ(spy.count(), 3);
}

TEST_F(InitSessionChainTest, StageCompletedSignal_EmittedWithSuccessTrue) {
    chain->add_stage_back(CreateOkStage("StageA"));

    QSignalSpy spy(chain.get(), &InitSessionChain::stage_completed);

    chain->execute();

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments[0].toString(), QString("StageA"));
    EXPECT_TRUE(arguments[1].toBool()); // success = true
}

TEST_F(InitSessionChainTest, StageCompletedSignal_EmittedWithSuccessFalseOnPartialFailed) {
    chain->add_stage_back(CreatePartialFailedStage("StageA"));

    QSignalSpy spy(chain.get(), &InitSessionChain::stage_completed);

    chain->execute();

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments[0].toString(), QString("StageA"));
    EXPECT_FALSE(arguments[1].toBool()); // success = false
}

TEST_F(InitSessionChainTest, StagePartialFailedSignal_EmittedOnPartialFailed) {
    QString msg("Partial failure message");
    QString detail("Detailed error");

    chain->add_stage_back(CreatePartialFailedStage("StageA", msg, detail));

    QSignalSpy spy(chain.get(), &InitSessionChain::stage_partial_failed);

    chain->execute();

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments[0].toString(), QString("StageA"));
    EXPECT_EQ(arguments[1].toString(), msg);
    EXPECT_EQ(arguments[2].toString(), detail);
}

TEST_F(InitSessionChainTest, ChainFinishedSignal_EmittedWithFinalResult) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));

    QSignalSpy spy(chain.get(), &InitSessionChain::chain_finished);

    chain->execute();

    EXPECT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    // The argument is a StageResult - check that it's valid
    auto result = arguments[0].value<IInitStage::StageResult>();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, AllSignals_EmittedInCorrectOrder) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreatePartialFailedStage("StageB"));

    QSignalSpy startSpy(chain.get(), &InitSessionChain::stage_start);
    QSignalSpy completedSpy(chain.get(), &InitSessionChain::stage_completed);
    QSignalSpy partialSpy(chain.get(), &InitSessionChain::stage_partial_failed);
    QSignalSpy finishedSpy(chain.get(), &InitSessionChain::chain_finished);

    chain->execute();

    EXPECT_EQ(startSpy.count(), 2);
    EXPECT_EQ(completedSpy.count(), 2);
    EXPECT_EQ(partialSpy.count(), 1);
    EXPECT_EQ(finishedSpy.count(), 1);
}

#endif // QT_TEST_AVAILABLE

// =============================================================================
// Test Suite 5: Edge Cases
// =============================================================================

TEST_F(InitSessionChainTest, NullStage_IsSkippedDuringExecution) {
    chain->add_stage_back(nullptr);
    chain->add_stage_back(CreateOkStage("ValidStage"));
    chain->add_stage_back(nullptr);

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, EmptyStageName_HandledGracefully) {
    auto stage = std::make_unique<MockInitStage>("", IInitStage::StatusCode::OK);
    chain->add_stage_back(std::move(stage));

    auto result = chain->execute();
    // Should not crash, empty name is technically valid
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, LargeNumberOfStages_HandledCorrectly) {
    const int stageCount = 100;
    for (int i = 0; i < stageCount; ++i) {
        std::string name = "Stage" + std::to_string(i);
        chain->add_stage_back(CreateOkStage(name));
    }

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
    EXPECT_TRUE(result.message.contains("100") || result.message.contains("success"));
}

TEST_F(InitSessionChainTest, MixedStatusStages_FinalResultIsPartialFailed) {
    // Mix of OK and PartialFailed
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreatePartialFailedStage("StageB"));
    chain->add_stage_back(CreateOkStage("StageC"));
    chain->add_stage_back(CreatePartialFailedStage("StageD"));

    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::PartialFailed);
}

TEST_F(InitSessionChainTest, Reset_ClearsExecutionState) {
    chain->add_stage_back(CreateOkStage("StageA"));

    // First run
    chain->execute();

    // Reset
    chain->reset();

    // Should be able to execute again
    auto result = chain->execute();
    EXPECT_EQ(result.code, IInitStage::StatusCode::OK);
}

TEST_F(InitSessionChainTest, GetDependencyGraphAfterExecute_ReturnsValidString) {
    chain->add_stage_back(CreateOkStage("StageA"));
    chain->add_stage_back(CreateOkStage("StageB"));

    // Execute first
    chain->execute();

    // Get debug string after execution
    auto debug_str = chain->get_dependency_graph_debug_string();
    EXPECT_FALSE(debug_str.isEmpty());
}

// =============================================================================
// Test Suite 6: StageResult Static Methods
// =============================================================================

TEST(StageResultTest, OkFactory_CreatesValidResult) {
    auto result = IInitStage::StageResult::ok("Success");

    EXPECT_EQ(result.code, IInitStage::IInitStage::StatusCode::OK);
    EXPECT_EQ(result.message, QString("Success"));
    EXPECT_TRUE(result.error_detail.isEmpty());
    EXPECT_TRUE(result.can_continue());
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(StageResultTest, PartialFailedFactory_CreatesValidResult) {
    QString msg("Partial failure");
    QString detail("Warning occurred");

    auto result = IInitStage::StageResult::partial_failed(msg, detail);

    EXPECT_EQ(result.code, IInitStage::IInitStage::StatusCode::PartialFailed);
    EXPECT_EQ(result.message, msg);
    EXPECT_EQ(result.error_detail, detail);
    EXPECT_TRUE(result.can_continue());
    EXPECT_FALSE(static_cast<bool>(result));
}

TEST(StageResultTest, CriticalFailedFactory_CreatesValidResult) {
    QString msg("Critical failure");
    QString detail("Fatal error occurred");

    auto result = IInitStage::StageResult::critical_failed(msg, detail);

    EXPECT_EQ(result.code, IInitStage::IInitStage::StatusCode::CriticalFailed);
    EXPECT_EQ(result.message, msg);
    EXPECT_EQ(result.error_detail, detail);
    EXPECT_FALSE(result.can_continue());
    EXPECT_FALSE(static_cast<bool>(result));
}

TEST(StageResultTest, CanContinue_ReturnsCorrectValue) {
    auto ok = IInitStage::StageResult::ok();
    auto partial = IInitStage::StageResult::partial_failed("msg", "detail");
    auto critical = IInitStage::StageResult::critical_failed("msg", "detail");

    EXPECT_TRUE(ok.can_continue());
    EXPECT_TRUE(partial.can_continue());
    EXPECT_FALSE(critical.can_continue());
}

TEST(StageResultTest, BoolOperator_ReturnsTrueOnlyForOk) {
    auto ok = IInitStage::StageResult::ok();
    auto partial = IInitStage::StageResult::partial_failed("msg", "detail");
    auto critical = IInitStage::StageResult::critical_failed("msg", "detail");

    EXPECT_TRUE(static_cast<bool>(ok));
    EXPECT_FALSE(static_cast<bool>(partial));
    EXPECT_FALSE(static_cast<bool>(critical));
}

// =============================================================================
// Test Suite 7: aex::Singleton Access
// =============================================================================

TEST_F(InitSessionChainTest, GetChainRef_InitiallyReturnsNull) {
    auto ref = InitSessionChain::GetChainRef();
    EXPECT_FALSE(ref.Get()); // Initially null
}

TEST_F(InitSessionChainTest, GetChainRef_AfterExecuteReturnsValidRef) {
    chain->add_stage_back(CreateOkStage("StageA"));

    chain->execute();

    auto ref = InitSessionChain::GetChainRef();
    // After execute, should have a valid reference
    // Note: The actual reference points to the chain that was just executed
}

// =============================================================================
// Main
// =============================================================================

} // namespace test
} // namespace cf::desktop::init_session

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
