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

TEST_F(HubCommandTest, CreatesRepositoryInsideSpecifiedDirectory) {
    fs::path target_dir = temp_dir / "my_project";
    EXPECT_TRUE(command->execute({target_dir.string()}));

    // Должен создаться каталог .svcs внутри указанной директории
    EXPECT_TRUE(fs::exists(target_dir / ".svcs"));
    EXPECT_TRUE(fs::exists(target_dir / ".svcs" / "config"));
}

TEST_F(HubCommandTest, CreatesRepositoryInExistingDirectory) {
    fs::path existing_dir = temp_dir / "existing_dir";
    fs::create_directories(existing_dir);

    EXPECT_TRUE(command->execute({existing_dir.string()}));
    EXPECT_TRUE(fs::exists(existing_dir / ".svcs"));
}

TEST_F(HubCommandTest, CreatesParentDirectoriesIfNeeded) {
    fs::path nested_dir = temp_dir / "level1" / "level2" / "project";
    EXPECT_TRUE(command->execute({nested_dir.string()}));

    EXPECT_TRUE(fs::exists(nested_dir / ".svcs"));
}

TEST_F(HubCommandTest, FailsWhenRepositoryAlreadyExists) {
    fs::path target_dir = temp_dir / "existing_repo";
    fs::create_directories(target_dir / ".svcs");

    EXPECT_FALSE(command->execute({target_dir.string()}));
}

TEST_F(HubCommandTest, CreatesRepositoryStructure) {
    fs::path target_dir = temp_dir / "test_project";
    EXPECT_TRUE(command->execute({target_dir.string()}));

    fs::path repo_dir = target_dir / ".svcs";
    EXPECT_TRUE(fs::exists(repo_dir / "objects"));
    EXPECT_TRUE(fs::exists(repo_dir / "refs" / "heads"));
    EXPECT_TRUE(fs::exists(repo_dir / "refs" / "tags"));
    EXPECT_TRUE(fs::exists(repo_dir / "hooks"));
    EXPECT_TRUE(fs::exists(repo_dir / "info"));
    EXPECT_TRUE(fs::exists(repo_dir / "config"));
    EXPECT_TRUE(fs::exists(repo_dir / "HEAD"));
    EXPECT_TRUE(fs::exists(repo_dir / "description"));
}

TEST_F(HubCommandTest, CreatesMultipleRepositories) {
    fs::path project1 = temp_dir / "project1";
    fs::path project2 = temp_dir / "project2";

    EXPECT_TRUE(command->execute({project1.string()}));
    EXPECT_TRUE(command->execute({project2.string()}));

    EXPECT_TRUE(fs::exists(project1 / ".svcs"));
    EXPECT_TRUE(fs::exists(project2 / ".svcs"));
}

TEST_F(HubCommandTest, ConfigFileContent) {
    fs::path target_dir = temp_dir / "config_test";
    EXPECT_TRUE(command->execute({target_dir.string()}));

    std::ifstream config_file(target_dir / ".svcs" / "config");
    std::string content((std::istreambuf_iterator<char>(config_file)),
                        std::istreambuf_iterator<char>());

    EXPECT_TRUE(content.find("bare = true") != std::string::npos);
    EXPECT_TRUE(content.find("hub = true") != std::string::npos);
}

TEST_F(HubCommandTest, HeadFileContent) {
    fs::path target_dir = temp_dir / "head_test";
    EXPECT_TRUE(command->execute({target_dir.string()}));

    std::ifstream head_file(target_dir / ".svcs" / "HEAD");
    std::string content((std::istreambuf_iterator<char>(head_file)),
                        std::istreambuf_iterator<char>());

    EXPECT_TRUE(content.find("ref: refs/heads/main") != std::string::npos);
}