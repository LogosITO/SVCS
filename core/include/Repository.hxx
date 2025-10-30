/**
 * @file Repository.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Manages the entire version control repository, acts as a Subject for event logging,
 * and controls all major VCS operations.
 * @details This class is responsible for initializing the repository structure,
 * managing the ObjectStorage, and coordinating all file system and object database interactions.
 * It implements the ISubject interface to broadcast events about its operations.
 *
 * @russian
 * @brief Управляет всем репозиторием системы контроля версий, действует как Subject для логирования событий
 * и контролирует все основные операции СКВ.
 * @details Этот класс отвечает за инициализацию структуры репозитория,
 * управление ObjectStorage и координацию всех взаимодействий с файловой системой и базой данных объектов.
 * Он реализует интерфейс ISubject для широковещательной передачи событий о своих операциях.
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "ObjectStorage.hxx"
#include "Index.hxx"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @english
 * @class Repository
 * @brief Manages the version control directory structure and core operations.
 * @details Implements ISubject to serve as an event publisher for the core VCS operations.
 *
 * @russian
 * @class Repository
 * @brief Управляет структурой директорий системы контроля версий и основными операциями.
 * @details Реализует ISubject для работы в качестве издателя событий для основных операций СКВ.
 */
class Repository : public ISubject {
private:
    /**
     * @english
     * @brief The root directory path of the repository (where the .svcs directory lives).
     *
     * @russian
     * @brief Корневой путь директории репозитория (где находится директория .svcs).
     */
    const std::filesystem::path root_path;

    /**
     * @english
     * @brief Unique pointer to the ObjectStorage instance, responsible for managing the object database.
     *
     * @russian
     * @brief Уникальный указатель на экземпляр ObjectStorage, отвечающий за управление базой данных объектов.
     */
    std::unique_ptr<ObjectStorage> objects;

    /**
     * @english
     * @brief Collection of weak pointers to registered observers, inherited from ISubject's concept.
     * @note This member is part of the concrete Subject implementation.
     *
     * @russian
     * @brief Коллекция слабых указателей на зарегистрированных наблюдателей, унаследованная от концепции ISubject.
     * @note Этот член является частью конкретной реализации Subject.
     */
    std::vector<std::weak_ptr<IObserver>> observers;

    /**
     * @english
     * @brief Unique pointer to the Index instance, responsible for managing the staging area.
     *
     * @russian
     * @brief Уникальный указатель на экземпляр Index, отвечающий за управление областью подготовки.
     */
    std::unique_ptr<Index> index;

public:
    /**
     * @english
     * @brief Virtual destructor.
     * @details Ensures correct cleanup of managed resources (ObjectStorage).
     *
     * @russian
     * @brief Виртуальный деструктор.
     * @details Обеспечивает корректную очистку управляемых ресурсов (ObjectStorage).
     */
    ~Repository() override;

    /**
     * @english
     * @brief Constructs the repository manager.
     * @param rpath The root directory of the repository.
     *
     * @russian
     * @brief Конструирует менеджер репозитория.
     * @param rpath Корневая директория репозитория.
     */
    explicit Repository(std::filesystem::path  rpath);

    /**
     * @english
     * @brief Initializes the repository's internal dependencies.
     * @details This typically involves instantiating and setting up internal components like ObjectStorage.
     *
     * @russian
     * @brief Инициализирует внутренние зависимости репозитория.
     * @details Обычно это включает создание и настройку внутренних компонентов, таких как ObjectStorage.
     */
    void initializeDependencies();

    /**
     * @english
     * @brief Initializes the basic repository structure (.svcs, objects, refs) on the disk.
     * @return bool True if initialization succeeded, false otherwise.
     *
     * @russian
     * @brief Инициализирует базовую структуру репозитория (.svcs, objects, refs) на диске.
     * @return bool True, если инициализация прошла успешно, false в противном случае.
     */
    bool init();

    /**
     * @english
     * @brief Checks if the repository structure is fully initialized and valid.
     * @return true if all necessary directories and files exist.
     *
     * @russian
     * @brief Проверяет, полностью ли инициализирована и действительна структура репозитория.
     * @return true, если все необходимые директории и файлы существуют.
     */
    [[nodiscard]] bool is_initialized() const;

    /**
     * @english
     * @brief Gets the root path of the repository.
     * @return const std::filesystem::path& The repository root path.
     *
     * @russian
     * @brief Получает корневой путь репозитория.
     * @return const std::filesystem::path& Корневой путь репозитория.
     */
    [[nodiscard]] const std::filesystem::path& getPath() const { return root_path; }

    /**
     * @english
     * @brief Gets the raw pointer to the managed ObjectStorage instance for database interaction.
     * @return ObjectStorage* Pointer to the managed ObjectStorage instance.
     *
     * @russian
     * @brief Получает сырой указатель на управляемый экземпляр ObjectStorage для взаимодействия с базой данных.
     * @return ObjectStorage* Указатель на управляемый экземпляр ObjectStorage.
     */
    [[nodiscard]] ObjectStorage* getObjectStorage() const { return objects.get(); }

    /**
     * @english
     * @brief Attaches an observer to receive events.
     * @param observer Shared pointer to the observer instance.
     *
     * @russian
     * @brief Прикрепляет наблюдателя для получения событий.
     * @param observer Общий указатель на экземпляр наблюдателя.
     */
    void attach(std::shared_ptr<IObserver> observer) override;

    /**
     * @english
     * @brief Detaches a registered observer.
     * @param observer Shared pointer to the observer instance to detach.
     *
     * @russian
     * @brief Отсоединяет зарегистрированного наблюдателя.
     * @param observer Общий указатель на экземпляр наблюдателя для отсоединения.
     */
    void detach(std::shared_ptr<IObserver> observer) override;

    /**
     * @english
     * @brief Notifies all attached observers about an event.
     * @param event The constant reference to the event data structure.
     *
     * @russian
     * @brief Уведомляет всех прикрепленных наблюдателей о событии.
     * @param event Константная ссылка на структуру данных события.
     */
    void notify(const Event& event) const override;

    /**
     * @english
     * @brief Statically searches for an existing repository starting from a given path and moving up.
     * @param start_path The directory path to begin the search from (defaults to ".").
     * @return A shared pointer to the found Repository instance, or nullptr if none is found.
     *
     * @russian
     * @brief Статически ищет существующий репозиторий, начиная с заданного пути и поднимаясь вверх.
     * @param start_path Путь директории, с которого начинать поиск (по умолчанию ".").
     * @return Общий указатель на найденный экземпляр Repository или nullptr, если ничего не найдено.
     */
    static std::shared_ptr<Repository> findRepository(const std::string& start_path = ".");

    /**
     * @english
     * @brief Statically checks if a given path is the root of an SVCS repository.
     * @param path The path to check.
     * @return true if the path contains the required repository marker files/directories.
     *
     * @russian
     * @brief Статически проверяет, является ли заданный путь корнем репозитория SVCS.
     * @param path Путь для проверки.
     * @return true, если путь содержит необходимые маркерные файлы/директории репозитория.
     */
    static bool isRepository(const std::string& path);

    /**
     * @english
     * @brief Stages a file by hashing its content, storing it in the object database,
     * and updating the staging index.
     * @param path The path to the file to be staged (relative to the repository root).
     * @return bool True if the file was successfully staged, false otherwise (e.g., file ignored, permission error).
     * @throws std::runtime_error if file system or object storage operations fail.
     *
     * @russian
     * @brief Подготавливает файл, хешируя его содержимое, сохраняя его в базе данных объектов
     * и обновляя индекс подготовки.
     * @param path Путь к файлу для подготовки (относительно корня репозитория).
     * @return bool True, если файл успешно подготовлен, false в противном случае (например, файл проигнорирован, ошибка прав доступа).
     * @throws std::runtime_error если операции файловой системы или хранилища объектов завершаются неудачно.
     */
    bool stageFile(const std::string& path); 
};