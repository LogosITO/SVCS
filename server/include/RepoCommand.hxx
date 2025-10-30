#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../core/include/Repository.hxx"
#include "../../services/ISubject.hxx"
#include "RemoteManager.hxx"
#include <memory>

class RepoCommand : public ICommand {
private:
    std::shared_ptr<ISubject> event_bus_;
    std::shared_ptr<RepositoryManager> repo_manager_;

public:
    RepoCommand(std::shared_ptr<ISubject> event_bus,
                std::shared_ptr<RepositoryManager> repo_manager);

    std::string getName() const override;
    std::string getDescription() const override;
    std::string getUsage() const override;
    bool execute(const std::vector<std::string>& args) override;
    void showHelp() const override;

private:
    bool handleAdd(RemoteManager& remote_manager, const std::vector<std::string>& args);
    bool handleRemove(RemoteManager& remote_manager, const std::vector<std::string>& args);
    bool handleList(RemoteManager& remote_manager);
    bool handleRename(RemoteManager& remote_manager, const std::vector<std::string>& args);
};
