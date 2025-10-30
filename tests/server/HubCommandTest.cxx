#include <gtest/gtest.h>
#include "../../server/include/HubCommand.hxx"
#include "../../services/Event.hxx"
#include "../../services/EventBus.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

class SimpleEventBus : public ISubject {
public:
    void attach(std::shared_ptr<IObserver>) override {}
    void detach(std::shared_ptr<IObserver>) override {}
    void notify(const Event&) const override {}
};

class HubCommandTest : public ::testing::Test {
protected:
    std::shared_ptr<SimpleEventBus> event_bus;
    std::shared_ptr<RepositoryManager> repo_manager;
    std::unique_ptr<HubCommand> command;
    fs::path temp_dir;

    void SetUp() override {
        event_bus = std::make_shared<SimpleEventBus>();
        repo_manager = std::make_shared<RepositoryManager>(event_bus);
        command = std::make_unique<HubCommand>(event_bus, repo_manager);
        temp_dir = fs::temp_directory_path() / "hub_command_test";
        fs::remove_all(temp_dir);
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        command.reset();
        repo_manager.reset();
        event_bus.reset();
        fs::remove_all(temp_dir);
    }
};

TEST_F(HubCommandTest, FailsWithNoArguments) {
    EXPECT_FALSE(command->execute({}));
}

TEST_F(HubCommandTest, FailsWithTooManyArguments) {
    EXPECT_FALSE(command->execute({"one", "two"}));
}

TEST_F(HubCommandTest, FailsWithEmptyString) {
    EXPECT_FALSE(command->execute({""}));
}

TEST_F(HubCommandTest, CreatesRepositoryWithNonExistingPath) {
    fs::path repo_path = temp_dir / "new_repository";
    EXPECT_TRUE(command->execute({repo_path.string()}));
    EXPECT_TRUE(fs::exists(repo_path));
    EXPECT_TRUE(fs::exists(repo_path / ".svcs"));
}

TEST_F(HubCommandTest, FailsWhenPathAlreadyExists) {
    fs::path existing_path = temp_dir / "existing_repo";
    fs::create_directories(existing_path);

    EXPECT_FALSE(command->execute({existing_path.string()}));
}

TEST_F(HubCommandTest, CreatesRepositoryStructure) {
    fs::path repo_path = temp_dir / "test_repo";
    EXPECT_TRUE(command->execute({repo_path.string()}));

    EXPECT_TRUE(fs::exists(repo_path / ".svcs" / "objects"));
    EXPECT_TRUE(fs::exists(repo_path / ".svcs" / "refs" / "heads"));
    EXPECT_TRUE(fs::exists(repo_path / ".svcs" / "refs" / "tags"));
    EXPECT_TRUE(fs::exists(repo_path / ".svcs" / "config"));
    EXPECT_TRUE(fs::exists(repo_path / ".svcs" / "HEAD"));
}

TEST_F(HubCommandTest, CreatesMultipleRepositories) {
    fs::path repo1 = temp_dir / "repo1";
    fs::path repo2 = temp_dir / "repo2";

    EXPECT_TRUE(command->execute({repo1.string()}));
    EXPECT_TRUE(command->execute({repo2.string()}));

    EXPECT_TRUE(fs::exists(repo1 / ".svcs"));
    EXPECT_TRUE(fs::exists(repo2 / ".svcs"));
}

TEST_F(HubCommandTest, HandlesNestedPaths) {
    fs::path nested_repo = temp_dir / "level1" / "level2" / "nested_repo";
    EXPECT_TRUE(command->execute({nested_repo.string()}));
    EXPECT_TRUE(fs::exists(nested_repo / ".svcs"));
}