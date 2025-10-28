/**
* @file HubCommandTest.cxx
 * @brief Google Tests for HubCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include <gtest/gtest.h>
#include "../../server/include/HubCommand.hxx"
#include "../../services/Event.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

class MockEventBus : public ISubject {
public:
    void attach(std::shared_ptr<IObserver> observer) override {
        // Простая реализация без сложной логики
    }

    void detach(std::shared_ptr<IObserver> observer) override {
        // Простая реализация без сложной логики
    }

    void notify(const Event& event) const override {
        // Просто сохраняем события
        const_cast<MockEventBus*>(this)->events.push_back(event);
    }

    mutable std::vector<Event> events;

    bool containsEventType(Event::Type type) const {
        for (const auto& event : events) {
            if (event.type == type) {
                return true;
            }
        }
        return false;
    }

    void clear() {
        events.clear();
    }
};

class MockRepositoryManager : public RepositoryManager {
public:
    MockRepositoryManager(std::shared_ptr<ISubject> bus)
        : RepositoryManager(bus) {}

    bool repositoryExists() const {
        return true;
    }

    std::filesystem::path getRepositoryPath() const {
        return "/tmp/test-repo";
    }
};

class HubCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        event_bus = std::make_shared<MockEventBus>();
        repo_manager = std::make_shared<MockRepositoryManager>(event_bus);
        command = std::make_unique<HubCommand>(event_bus, repo_manager);

        test_repo_path = "/tmp/test_hub_repo.svcs";
        cleanupTestRepo();
    }

    void TearDown() override {
        cleanupTestRepo();
    }

    void cleanupTestRepo() {
        if (fs::exists(test_repo_path)) {
            fs::remove_all(test_repo_path);
        }
    }

    bool repoExists() const {
        return fs::exists(test_repo_path);
    }

    bool hasSvcsSubdir() const {
        return fs::exists(fs::path(test_repo_path) / ".svcs");
    }

    bool hasConfigFile() const {
        return fs::exists(fs::path(test_repo_path) / ".svcs" / "config");
    }

    bool hasHEADFile() const {
        return fs::exists(fs::path(test_repo_path) / ".svcs" / "HEAD");
    }

    bool hasObjectsDir() const {
        return fs::exists(fs::path(test_repo_path) / ".svcs" / "objects");
    }

    bool hasRefsDir() const {
        return fs::exists(fs::path(test_repo_path) / ".svcs" / "refs" / "heads") &&
               fs::exists(fs::path(test_repo_path) / ".svcs" / "refs" / "tags");
    }

    std::string readFile(const std::string& path) const {
        std::ifstream file(path);
        if (!file.is_open()) return "";
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::shared_ptr<MockEventBus> event_bus;
    std::shared_ptr<MockRepositoryManager> repo_manager;
    std::unique_ptr<HubCommand> command;
    std::string test_repo_path = "/tmp/test_hub_repo.svcs";
};

TEST_F(HubCommandTest, GetName) {
    EXPECT_EQ(command->getName(), "hub");
}

TEST_F(HubCommandTest, GetDescription) {
    EXPECT_FALSE(command->getDescription().empty());
}

TEST_F(HubCommandTest, GetUsage) {
    EXPECT_FALSE(command->getUsage().empty());
    EXPECT_TRUE(command->getUsage().find("hub") != std::string::npos);
}

TEST_F(HubCommandTest, ExecuteSuccess) {
    std::vector<std::string> args = {test_repo_path};

    bool result = command->execute(args);

    EXPECT_TRUE(result);
    EXPECT_TRUE(repoExists());
    EXPECT_TRUE(hasSvcsSubdir());
    EXPECT_TRUE(hasConfigFile());
    EXPECT_TRUE(hasHEADFile());
    EXPECT_TRUE(hasObjectsDir());
    EXPECT_TRUE(hasRefsDir());
}

TEST_F(HubCommandTest, ExecuteAutoAddsExtension) {
    std::string path_without_extension = "/tmp/test_hub_repo_no_ext";
    std::vector<std::string> args = {path_without_extension};

    bool result = command->execute(args);

    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists(path_without_extension + ".svcs"));

    fs::remove_all(path_without_extension + ".svcs");
}

TEST_F(HubCommandTest, ExecuteWithExistingExtension) {
    std::vector<std::string> args = {test_repo_path};

    bool result = command->execute(args);

    EXPECT_TRUE(result);
    EXPECT_TRUE(repoExists());
}

TEST_F(HubCommandTest, ExecuteNoArguments) {
    std::vector<std::string> args;

    bool result = command->execute(args);

    EXPECT_FALSE(result);
    EXPECT_FALSE(repoExists());
}

TEST_F(HubCommandTest, ExecuteTooManyArguments) {
    std::vector<std::string> args = {test_repo_path, "extra_arg"};

    bool result = command->execute(args);

    EXPECT_FALSE(result);
    EXPECT_FALSE(repoExists());
}

TEST_F(HubCommandTest, ExecutePathAlreadyExists) {
    fs::create_directories(test_repo_path);
    std::vector<std::string> args = {test_repo_path};

    bool result = command->execute(args);

    EXPECT_FALSE(result);
}

TEST_F(HubCommandTest, ConfigFileContent) {
    std::vector<std::string> args = {test_repo_path};
    command->execute(args);

    std::string config_path = (fs::path(test_repo_path) / ".svcs" / "config").string();
    std::string config_content = readFile(config_path);

    EXPECT_FALSE(config_content.empty());
}

TEST_F(HubCommandTest, HEADFileContent) {
    std::vector<std::string> args = {test_repo_path};
    command->execute(args);

    std::string head_path = (fs::path(test_repo_path) / ".svcs" / "HEAD").string();
    std::string head_content = readFile(head_path);

    EXPECT_FALSE(head_content.empty());
    EXPECT_TRUE(head_content.find("ref:") != std::string::npos);
}

TEST_F(HubCommandTest, DirectoryStructure) {
    std::vector<std::string> args = {test_repo_path};
    command->execute(args);

    EXPECT_TRUE(hasObjectsDir());
    EXPECT_TRUE(hasRefsDir());
}

TEST_F(HubCommandTest, ShowHelp) {
    testing::internal::CaptureStdout();
    command->showHelp();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("hub") != std::string::npos);
}