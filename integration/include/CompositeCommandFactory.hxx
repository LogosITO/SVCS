/**
 * @file CompositeCommandFactory.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Composite factory that combines multiple command factories.
 * @details Integration component that delegates command creation to multiple
 * underlying factories in sequence. Provides unified interface for both
 * standard CLI commands and server administration commands.
 *
 * @russian
 * @brief Композитная фабрика, объединяющая несколько фабрик команд.
 * @details Интеграционный компонент, который делегирует создание команд
 * нескольким базовым фабрикам последовательно. Предоставляет унифицированный
 * интерфейс для стандартных CLI команд и команд администрирования сервера.
 */
#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <unordered_set>

/**
 * @english
 * @class CompositeCommandFactory
 * @brief Integration component for combining multiple command factories.
 * @details Serves as a bridge between different command subsystems (CLI, Server).
 * When creating a command, it tries each registered factory in order until
 * one successfully creates the command. This design allows clean separation
 * of concerns while providing a unified command interface to the application.
 *
 * @russian
 * @class CompositeCommandFactory
 * @brief Интеграционный компонент для объединения нескольких фабрик команд.
 * @details Служит мостом между различными подсистемами команд (CLI, Server).
 * При создании команды пробует каждую зарегистрированную фабрику по порядку,
 * пока одна из них не создаст команду успешно. Такая архитектура позволяет
 * чисто разделить ответственность, предоставляя единый интерфейс команд приложению.
 */
class CompositeCommandFactory {
public:
    /**
     * @english
     * @brief Constructs a CompositeCommandFactory with necessary dependencies.
     * @param event_bus Shared pointer to the event bus for command notifications.
     * @param repo_manager Shared pointer to the repository manager for repository operations.
     *
     * @russian
     * @brief Конструирует CompositeCommandFactory с необходимыми зависимостями.
     * @param event_bus Общий указатель на шину событий для уведомлений команд.
     * @param repo_manager Общий указатель на менеджер репозиториев для операций с репозиториями.
     */
    CompositeCommandFactory(std::shared_ptr<ISubject> event_bus,
                          std::shared_ptr<RepositoryManager> repo_manager);

    /**
     * @english
     * @brief Default destructor.
     *
     * @russian
     * @brief Деструктор по умолчанию.
     */
    ~CompositeCommandFactory() = default;

    /**
     * @english
     * @brief Deleted copy constructor.
     *
     * @russian
     * @brief Удаленный конструктор копирования.
     */
    CompositeCommandFactory(const CompositeCommandFactory&) = delete;

    /**
     * @english
     * @brief Deleted assignment operator.
     *
     * @russian
     * @brief Удаленный оператор присваивания.
     */
    CompositeCommandFactory& operator=(const CompositeCommandFactory&) = delete;

    /**
     * @english
     * @brief Registers a factory function for command creation.
     * @param factory Factory function that takes a command name and returns a command pointer.
     * @details Factories are called in the order they are registered until one returns a non-null command.
     * This allows prioritizing certain command types over others.
     *
     * @russian
     * @brief Регистрирует фабричную функцию для создания команд.
     * @param factory Фабричная функция, которая принимает имя команды и возвращает указатель на команду.
     * @details Фабрики вызываются в порядке их регистрации до тех пор, пока одна не вернет команду (не null).
     * Это позволяет приоритизировать определенные типы команд над другими.
     */
    void registerFactory(std::function<std::unique_ptr<ICommand>(const std::string&)> factory);

    /**
     * @english
     * @brief Creates a command by name using registered factories.
     * @param name The name of the command to create.
     * @return Unique pointer to the created command, or nullptr if no factory can create it.
     * @details Tries each registered factory in order until one successfully creates the command.
     * Returns the first non-null command found.
     *
     * @russian
     * @brief Создает команду по имени с использованием зарегистрированных фабрик.
     * @param name Имя команды для создания.
     * @return Уникальный указатель на созданную команду, или nullptr если ни одна фабрика не может ее создать.
     * @details Пробует каждую зарегистрированную фабрику по порядку, пока одна не создаст команду успешно.
     * Возвращает первую найденную команду (не null).
     */
    std::unique_ptr<ICommand> createCommand(const std::string& name) const;

    /**
     * @english
     * @brief Gets names of all available commands from all factories.
     * @return Vector of all command names that can be created by any registered factory.
     * @details Collects command names from all registered factories, removing duplicates.
     * Commands are returned in registration order (factory order), not alphabetical order.
     *
     * @russian
     * @brief Получает имена всех доступных команд из всех фабрик.
     * @return Вектор всех имен команд, которые могут быть созданы любой зарегистрированной фабрикой.
     * @details Собирает имена команд из всех зарегистрированных фабрик, удаляя дубликаты.
     * Команды возвращаются в порядке регистрации (порядок фабрик), не в алфавитном порядке.
     */
    std::vector<std::string> getAllCommandNames() const;

    /**
     * @english
     * @brief Gets the description for a command.
     * @param name The name of the command.
     * @return Command description, or error message if command not found.
     * @details Creates a temporary command instance to retrieve its description.
     * If command cannot be created, returns an appropriate error message.
     *
     * @russian
     * @brief Получает описание для команды.
     * @param name Имя команды.
     * @return Описание команды или сообщение об ошибке, если команда не найдена.
     * @details Создает временный экземпляр команды для получения ее описания.
     * Если команда не может быть создана, возвращает соответствующее сообщение об ошибке.
     */
    std::string getCommandDescription(const std::string& name) const;

    /**
     * @english
     * @brief Shows detailed help for a command.
     * @param name The name of the command.
     * @details Creates a temporary command instance and calls its showHelp method.
     * If command cannot be created, shows an error message.
     *
     * @russian
     * @brief Отображает подробную справку для команды.
     * @param name Имя команды.
     * @details Создает временный экземпляр команды и вызывает его метод showHelp.
     * Если команда не может быть создана, показывает сообщение об ошибке.
     */
    void showCommandHelp(const std::string& name) const;

    /**
     * @english
     * @brief Checks if a command with the given name exists.
     * @param name The name of the command to check.
     * @return True if any registered factory can create the command, false otherwise.
     * @details Tries to create the command to determine existence.
     *
     * @russian
     * @brief Проверяет, существует ли команда с заданным именем.
     * @param name Имя команды для проверки.
     * @return True если любая зарегистрированная фабрика может создать команду, false в противном случае.
     * @details Пытается создать команду для определения существования.
     */
    bool commandExists(const std::string& name) const;

    /**
     * @english
     * @brief Gets the number of registered factories.
     * @return Number of factory functions currently registered.
     *
     * @russian
     * @brief Получает количество зарегистрированных фабрик.
     * @return Количество фабричных функций, зарегистрированных в данный момент.
     */
    size_t getFactoryCount() const;

private:
    /**
     * @english
     * @brief Shared pointer to the event bus for command notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий для уведомлений команд.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Shared pointer to the repository manager for repository operations.
     *
     * @russian
     * @brief Общий указатель на менеджер репозиториев для операций с репозиториями.
     */
    std::shared_ptr<RepositoryManager> repo_manager_;

    /**
     * @english
     * @brief List of registered factory functions.
     * @details Maintains insertion order for factory priority.
     *
     * @russian
     * @brief Список зарегистрированных фабричных функций.
     * @details Сохраняет порядок вставки для приоритета фабрик.
     */
    std::vector<std::function<std::unique_ptr<ICommand>(const std::string&)>> factories_;
};