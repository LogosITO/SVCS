#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

/**
 * @file AddCommand.hxx
 * @brief Объявление класса AddCommand.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Реализует команду "add" для системы контроля версий (SVCS).
 * * Эта команда отвечает за добавление (индексацию) указанных файлов
 * в область индексации (staging area) репозитория для последующего коммита.
 *
 * Класс наследуется от ICommand и использует паттерн Команда (Command)
 * для выполнения своих операций. Он зависит от ISubject (шины событий)
 * для уведомления пользователя и RepositoryManager для выполнения
 * операций с репозиторием.
 */
class AddCommand : public ICommand {
private:
    /** @brief Общий указатель на шину событий, используемую для уведомления о событиях. */
    std::shared_ptr<ISubject> eventBus_;
    
    /** @brief Общий указатель на менеджер репозитория, используемый для управления файлами. */
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Конструктор AddCommand.
     * @param subject Общий указатель на шину событий (ISubject).
     * @param repoManager Общий указатель на RepositoryManager.
     */
    AddCommand(std::shared_ptr<ISubject> subject, 
               std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Выполняет логику команды "add".
     * * Пытается добавить файлы, указанные в args, в область индексации.
     * @param args Вектор строк, содержащих пути к файлам для индексации.
     * @return \c true в случае успеха, \c false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    std::vector<std::string> getAllFilesInDirectory(const std::string& directory) const;
    
    /**
     * @brief Возвращает имя команды.
     * @return Строка "add".
     */
    std::string getName() const override { return "add"; }
    
    /**
     * @brief Возвращает краткое описание команды.
     * @return Строка с описанием.
     */
    std::string getDescription() const override;
    
    /**
     * @brief Возвращает синтаксис использования команды.
     * @return Строка с примером использования.
     */
    std::string getUsage() const override;
    
    /**
     * @brief Отображает подробную справочную информацию для команды.
     */
    void showHelp() const override;
};