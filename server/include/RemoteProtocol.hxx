/**
 * @file RemoteProtocol.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the RemoteProtocol class for handling remote communication protocol.
 * @details The RemoteProtocol implements the custom protocol for push/pull operations
 * between SVCS clients and servers. It handles object negotiation, transfer, and
 * reference updates through a simple text-based protocol over stdin/stdout.
 *
 * @russian
 * @brief Объявление класса RemoteProtocol для обработки протокола удаленной связи.
 * @details RemoteProtocol реализует пользовательский протокол для операций push/pull
 * между клиентами и серверами SVCS. Он обрабатывает согласование объектов, передачу и
 * обновления ссылок через простой текстовый протокол по stdin/stdout.
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <iostream>

/**
 * @english
 * @class RemoteProtocol
 * @brief Handles the remote communication protocol for SVCS.
 * @details Manages the custom protocol for **push** and **pull** operations, including
 * object negotiation, data transfer, and reference updates between client and server.
 * The protocol is a simple text-based exchange over standard input/output.
 *
 * @russian
 * @class RemoteProtocol
 * @brief Обрабатывает протокол удаленной связи для SVCS.
 * @details Управляет пользовательским протоколом для операций **push** и **pull**, включая
 * согласование объектов, передачу данных и обновления ссылок между клиентом и сервером.
 * Протокол представляет собой простой текстовый обмен через стандартный ввод/вывод.
 */
class RemoteProtocol {
public:
    /**
     * @english
     * @brief Constructs a RemoteProtocol with necessary dependencies.
     * @param event_bus The shared pointer to the event bus for the notification system (@ref ISubject).
     * @param repository_manager The shared pointer to the repository manager for repository operations (@ref RepositoryManager).
     *
     * @russian
     * @brief Конструирует RemoteProtocol с необходимыми зависимостями.
     * @param event_bus Общий указатель на шину событий для системы уведомлений (@ref ISubject).
     * @param repository_manager Общий указатель на менеджер репозитория для операций с репозиторием (@ref RepositoryManager).
     */
    RemoteProtocol(std::shared_ptr<ISubject> event_bus,
                   std::shared_ptr<RepositoryManager> repository_manager);

    /**
     * @english
     * @brief Default destructor.
     *
     * @russian
     * @brief Деструктор по умолчанию.
     */
    ~RemoteProtocol() = default;

    /**
     * @english
     * @brief Handles the 'receive-pack' protocol for **push** operations.
     * @return bool True if the protocol completed successfully (all objects received and references updated), false otherwise.
     * @details This function processes client push requests. It manages the negotiation of objects,
     * receives the missing object data from the client, and finally attempts to apply the
     * requested reference updates (e.g., updating branches).
     *
     * @russian
     * @brief Обрабатывает протокол 'receive-pack' для операций **push**.
     * @return bool True если протокол завершен успешно (все объекты получены и ссылки обновлены), false в противном случае.
     * @details Эта функция обрабатывает запросы push от клиента. Она управляет согласованием объектов,
     * получает недостающие данные объектов от клиента и, наконец, пытается применить
     * запрошенные обновления ссылок (например, обновление веток).
     */
    bool handleReceivePack();

    /**
     * @english
     * @brief Handles the 'upload-pack' protocol for **pull** operations.
     * @return bool True if the protocol completed successfully (objects negotiated and sent), false otherwise.
     * @details This function processes client pull requests. It performs object negotiation by
     * determining which objects the client is missing based on its 'have' list, and then
     * packages and sends the missing object data to the client.
     *
     * @russian
     * @brief Обрабатывает протокол 'upload-pack' для операций **pull**.
     * @return bool True если протокол завершен успешно (объекты согласованы и отправлены), false в противном случае.
     * @details Эта функция обрабатывает запросы pull от клиента. Она выполняет согласование объектов путем
     * определения, каких объектов не хватает клиенту на основе его списка 'have', а затем
     * упаковывает и отправляет недостающие данные объектов клиенту.
     */
    bool handleUploadPack();

private:
    // Receive-pack (Push) Protocol Methods
    /**
     * @english
     * @brief Processes the initial negotiation phase for a push operation.
     * @return bool True on successful negotiation, false otherwise.
     *
     * @russian
     * @brief Обрабатывает начальную фазу согласования для операции push.
     * @return bool True при успешном согласовании, false в противном случае.
     */
    bool processPushNegotiation();

    /**
     * @english
     * @brief Receives and stores objects sent by the client.
     * @param wanted_objects A set of object hashes that the server is missing and the client needs to send.
     * @return bool True if all expected objects were received and written successfully, false otherwise.
     *
     * @russian
     * @brief Получает и сохраняет объекты, отправленные клиентом.
     * @param wanted_objects Набор хешей объектов, которых не хватает серверу и которые клиент должен отправить.
     * @return bool True если все ожидаемые объекты получены и записаны успешно, false в противном случае.
     */
    bool receiveObjects(const std::unordered_set<std::string>& wanted_objects);

    /**
     * @english
     * @brief Updates local references (e.g., branches, tags) based on client requests.
     * @param ref_updates A vector of pairs where each pair is `{ref_name, new_hash}`.
     * @return bool True if all reference updates were applied successfully, false otherwise.
     *
     * @russian
     * @brief Обновляет локальные ссылки (например, ветки, теги) на основе запросов клиента.
     * @param ref_updates Вектор пар, где каждая пара `{ref_name, new_hash}`.
     * @return bool True если все обновления ссылок применены успешно, false в противном случае.
     */
    bool updateReferences(const std::vector<std::pair<std::string, std::string>>& ref_updates);

    // Upload-pack (Pull) Protocol Methods
    /**
     * @english
     * @brief Processes the initial negotiation phase for a pull operation.
     * @return bool True on successful negotiation, false otherwise.
     *
     * @russian
     * @brief Обрабатывает начальную фазу согласования для операции pull.
     * @return bool True при успешном согласовании, false в противном случае.
     */
    bool processPullNegotiation();

    /**
     * @english
     * @brief Identifies which objects the client is missing.
     * @param client_has A set of object hashes that the client already has.
     * @return std::unordered_set<std::string> A set of object hashes that the server has, but the client is missing and needs.
     *
     * @russian
     * @brief Определяет, каких объектов не хватает клиенту.
     * @param client_has Набор хешей объектов, которые уже есть у клиента.
     * @return std::unordered_set<std::string> Набор хешей объектов, которые есть у сервера, но которых не хватает клиенту и которые ему нужны.
     */
    std::unordered_set<std::string> findMissingObjects(const std::unordered_set<std::string>& client_has);

    /**
     * @english
     * @brief Sends the requested missing objects to the client.
     * @param objects_to_send A set of object hashes to be sent to the client.
     * @return bool True if all specified objects were successfully sent, false otherwise.
     *
     * @russian
     * @brief Отправляет запрошенные недостающие объекты клиенту.
     * @param objects_to_send Набор хешей объектов для отправки клиенту.
     * @return bool True если все указанные объекты успешно отправлены, false в противном случае.
     */
    bool sendObjects(const std::unordered_set<std::string>& objects_to_send);

    // Object and Reference Management Methods
    /**
     * @english
     * @brief Checks if a given object hash exists in the repository.
     * @param object_hash The SHA-1 hash of the object.
     * @return bool True if the object exists, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли данный хеш объекта в репозитории.
     * @param object_hash Хеш SHA-1 объекта.
     * @return bool True если объект существует, false в противном случае.
     */
    bool objectExists(const std::string& object_hash) const;

    /**
     * @english
     * @brief Reads and retrieves the content of an object.
     * @param object_hash The SHA-1 hash of the object.
     * @return std::string The content of the object, or an empty string on failure.
     *
     * @russian
     * @brief Читает и извлекает содержимое объекта.
     * @param object_hash Хеш SHA-1 объекта.
     * @return std::string Содержимое объекта или пустая строка при ошибке.
     */
    std::string readObject(const std::string& object_hash) const;

    /**
     * @english
     * @brief Writes a new object to the repository.
     * @param object_hash The expected SHA-1 hash of the object (used for verification).
     * @param object_data The raw data of the object to be stored.
     * @return bool True if the object was written successfully, false otherwise.
     *
     * @russian
     * @brief Записывает новый объект в репозиторий.
     * @param object_hash Ожидаемый хеш SHA-1 объекта (используется для проверки).
     * @param object_data Необработанные данные объекта для хранения.
     * @return bool True если объект записан успешно, false в противном случае.
     */
    bool writeObject(const std::string& object_hash, const std::string& object_data);

    /**
     * @english
     * @brief Retrieves a set of all current reference names (e.g., branch names, tags) in the repository.
     * @return std::unordered_set<std::string> A set of all reference names.
     *
     * @russian
     * @brief Извлекает набор всех текущих имен ссылок (например, имен веток, тегов) в репозитории.
     * @return std::unordered_set<std::string> Набор всех имен ссылок.
     */
    std::unordered_set<std::string> getAllReferences() const;

    /**
     * @english
     * @brief Updates a specific reference to point to a new object hash.
     * @param ref_name The name of the reference (e.g., "refs/heads/master").
     * @param new_hash The new object hash (e.g., commit SHA-1) the reference should point to.
     * @return bool True if the reference was updated, false otherwise.
     *
     * @russian
     * @brief Обновляет конкретную ссылку, чтобы она указывала на новый хеш объекта.
     * @param ref_name Имя ссылки (например, "refs/heads/master").
     * @param new_hash Новый хеш объекта (например, коммит SHA-1), на который должна указывать ссылка.
     * @return bool True если ссылка обновлена, false в противном случае.
     */
    bool updateReference(const std::string& ref_name, const std::string& new_hash);

    // Protocol Communication Methods
    /**
     * @english
     * @brief Reads a single line from the standard input (stdin) for protocol data.
     * @param line Output parameter to store the read line.
     * @return bool True if a line was read successfully, false on EOF or read error.
     * @note This method handles the underlying I/O for the text-based protocol.
     *
     * @russian
     * @brief Читает одну строку из стандартного ввода (stdin) для данных протокола.
     * @param line Выходной параметр для хранения прочитанной строки.
     * @return bool True если строка прочитана успешно, false при EOF или ошибке чтения.
     * @note Этот метод обрабатывает базовый ввод/вывод для текстового протокола.
     */
    bool readLine(std::string& line) const;

    /**
     * @english
     * @brief Sends a single line of data to the standard output (stdout) for the protocol.
     * @param line The line of data to send.
     * @return bool True if the line was sent successfully, false otherwise.
     * @note This method handles the underlying I/O for the text-based protocol.
     *
     * @russian
     * @brief Отправляет одну строку данных в стандартный вывод (stdout) для протокола.
     * @param line Строка данных для отправки.
     * @return bool True если строка отправлена успешно, false в противном случае.
     * @note Этот метод обрабатывает базовый ввод/вывод для текстового протокола.
     */
    bool sendLine(const std::string& line) const;

    /**
     * @english
     * @brief Sends raw data (e.g., packed objects) to the standard output.
     * @param data The raw string data to send.
     * @return bool True if the data was sent successfully, false otherwise.
     *
     * @russian
     * @brief Отправляет необработанные данные (например, упакованные объекты) в стандартный вывод.
     * @param data Необработанные строковые данные для отправки.
     * @return bool True если данные отправлены успешно, false в противном случае.
     */
    bool sendData(const std::string& data) const;

    /**
     * @english
     * @brief Sends an error message through the protocol channel.
     * @param error The error message string.
     * @return bool True if the error message was sent successfully, false otherwise.
     *
     * @russian
     * @brief Отправляет сообщение об ошибке через канал протокола.
     * @param error Строка сообщения об ошибке.
     * @return bool True если сообщение об ошибке отправлено успешно, false в противном случае.
     */
    bool sendError(const std::string& error) const;

    // Validation Methods
    /**
     * @english
     * @brief Validates if a string is a correctly formatted object hash (e.g., 40 hex characters).
     * @param hash The string to validate.
     * @return bool True if the hash is valid, false otherwise.
     *
     * @russian
     * @brief Проверяет, является ли строка правильно отформатированным хешем объекта (например, 40 шестнадцатеричных символов).
     * @param hash Строка для проверки.
     * @return bool True если хеш валиден, false в противном случае.
     */
    bool isValidObjectHash(const std::string& hash) const;

    /**
     * @english
     * @brief Validates if a string is a correctly formatted and safe reference name.
     * @param ref_name The string to validate (e.g., "refs/heads/master").
     * @return bool True if the reference name is valid, false otherwise.
     *
     * @russian
     * @brief Проверяет, является ли строка правильно отформатированным и безопасным именем ссылки.
     * @param ref_name Строка для проверки (например, "refs/heads/master").
     * @return bool True если имя ссылки валидно, false в противном случае.
     */
    bool isValidReference(const std::string& ref_name) const;

    // Notification Methods
    /**
     * @english
     * @brief Sends a debug notification message via the event bus.
     * @param message The debug message.
     *
     * @russian
     * @brief Отправляет отладочное уведомительное сообщение через шину событий.
     * @param message Отладочное сообщение.
     */
    void notifyDebug(const std::string& message) const;

    /**
     * @english
     * @brief Sends an informational notification message via the event bus.
     * @param message The informational message.
     *
     * @russian
     * @brief Отправляет информационное уведомительное сообщение через шину событий.
     * @param message Информационное сообщение.
     */
    void notifyInfo(const std::string& message) const;

    /**
     * @english
     * @brief Sends an error notification message via the event bus.
     * @param message The error message.
     *
     * @russian
     * @brief Отправляет уведомительное сообщение об ошибке через шину событий.
     * @param message Сообщение об ошибке.
     */
    void notifyError(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification indicating the start of a protocol operation (push/pull).
     * @param message Details about the protocol start (e.g., "Starting receive-pack").
     *
     * @russian
     * @brief Отправляет уведомление, указывающее на начало операции протокола (push/pull).
     * @param message Детали о начале протокола (например, "Starting receive-pack").
     */
    void notifyProtocolStart(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification indicating the successful completion of a protocol.
     * @param message Details about the success.
     *
     * @russian
     * @brief Отправляет уведомление, указывающее на успешное завершение протокола.
     * @param message Детали об успехе.
     */
    void notifyProtocolSuccess(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification indicating a protocol failure.
     * @param message Details about the error.
     *
     * @russian
     * @brief Отправляет уведомление, указывающее на сбой протокола.
     * @param message Детали об ошибке.
     */
    void notifyProtocolError(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification related to network sending activities.
     * @param message Details about the data sent over the network/stream.
     *
     * @russian
     * @brief Отправляет уведомление, связанное с сетевыми действиями отправки.
     * @param message Детали о данных, отправленных по сети/потоку.
     */
    void notifyNetworkSend(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification related to network receiving activities.
     * @param message Details about the data received over the network/stream.
     *
     * @russian
     * @brief Отправляет уведомление, связанное с сетевыми действиями получения.
     * @param message Детали о данных, полученных по сети/потоку.
     */
    void notifyNetworkReceive(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification about an object being transferred.
     * @param message Details about the object transfer (e.g., hash, size, direction).
     *
     * @russian
     * @brief Отправляет уведомление о передаче объекта.
     * @param message Детали о передаче объекта (например, хеш, размер, направление).
     */
    void notifyObjectTransfer(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification about a reference (branch/tag) being updated.
     * @param message Details about the reference update (e.g., "master: old_hash -> new_hash").
     *
     * @russian
     * @brief Отправляет уведомление об обновлении ссылки (ветки/тега).
     * @param message Детали об обновлении ссылки (например, "master: old_hash -> new_hash").
     */
    void notifyReferenceUpdate(const std::string& message) const;

    /**
     * @english
     * @brief Sends a notification related to the object negotiation phase.
     * @param message Details about the negotiation process (e.g., "Client HAS X", "Server ACKs Y").
     *
     * @russian
     * @brief Отправляет уведомление, связанное с фазой согласования объектов.
     * @param message Детали о процессе согласования (например, "Client HAS X", "Server ACKs Y").
     */
    void notifyNegotiation(const std::string& message) const;

    /**
     * @english
     * @brief Sends an error message to the client through the protocol channel and logs a protocol error.
     * @param error The error message string to send to the client.
     * @return bool True if the error message was sent successfully, false otherwise.
     * @details This is a utility method that combines `sendError` (for client) and `notifyProtocolError` (for logging).
     *
     * @russian
     * @brief Отправляет сообщение об ошибке клиенту через канал протокола и логирует ошибку протокола.
     * @param error Строка сообщения об ошибке для отправки клиенту.
     * @return bool True если сообщение об ошибке отправлено успешно, false в противном случае.
     * @details Это служебный метод, который объединяет `sendError` (для клиента) и `notifyProtocolError` (для логирования).
     */
    bool sendProtocolError(const std::string& error) const;

private:
    /**
     * @english
     * @brief Shared pointer to the event bus for notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий для уведомлений.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Shared pointer to the repository manager for object and reference operations.
     *
     * @russian
     * @brief Общий указатель на менеджер репозитория для операций с объектами и ссылками.
     */
    std::shared_ptr<RepositoryManager> repository_manager_;
};