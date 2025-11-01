/**
 * @file Server.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Main server component for handling incoming Git connections.
 * @details Server manages TCP connections, spawns protocol handlers for each client,
 * and provides the main entry point for the SVCS server application.
 *
 * @russian
 * @brief Основной серверный компонент для обработки входящих Git-соединений.
 * @details Server управляет TCP-соединениями, запускает обработчики протоколов для каждого клиента
 * и предоставляет основную точку входа для серверного приложения SVCS.
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "RemoteProtocol.hxx"

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <unordered_set>
#include <mutex>

using boost::asio::ip::tcp;

/**
 * @english
 * @class Server
 * @brief Main TCP server for SVCS remote operations.
 * @details Listens on specified port, accepts client connections, and spawns
 * RemoteProtocol instances to handle push/pull operations. Supports multiple
 * concurrent clients through thread pooling.
 *
 * @russian
 * @class Server
 * @brief Основной TCP-сервер для удаленных операций SVCS.
 * @details Прослушивает указанный порт, принимает клиентские соединения и запускает
 * экземпляры RemoteProtocol для обработки операций push/pull. Поддерживает несколько
 * параллельных клиентов через пул потоков.
 */
class Server {
public:
    /**
     * @english
     * @brief Constructs a Server with specified dependencies.
     * @param io_context ASIO I/O context for asynchronous operations.
     * @param port TCP port to listen on (default: 9418 - Git protocol port).
     * @param repo_manager Shared pointer to repository manager.
     * @param event_bus Shared pointer to event bus for notifications.
     *
     * @russian
     * @brief Конструирует Server с указанными зависимостями.
     * @param io_context ASIO I/O контекст для асинхронных операций.
     * @param port TCP-порт для прослушивания (по умолчанию: 9418 - порт Git протокола).
     * @param repo_manager Общий указатель на менеджер репозиториев.
     * @param event_bus Общий указатель на шину событий для уведомлений.
     */
    Server(boost::asio::io_context& io_context,
           unsigned short port,
           std::shared_ptr<RepositoryManager> repo_manager,
           std::shared_ptr<ISubject> event_bus);

    /**
     * @english
     * @brief Destructor - stops the server and cleans up resources.
     *
     * @russian
     * @brief Деструктор - останавливает сервер и освобождает ресурсы.
     */
    ~Server();

    /**
     * @english
     * @brief Starts the server and begins accepting connections.
     * @return bool True if server started successfully, false otherwise.
     *
     * @russian
     * @brief Запускает сервер и начинает принимать соединения.
     * @return bool True если сервер запущен успешно, false в противном случае.
     */
    bool start();

    /**
     * @english
     * @brief Stops the server gracefully.
     * @details Stops accepting new connections and allows existing connections to complete.
     *
     * @russian
     * @brief Останавливает сервер корректно.
     * @details Прекращает прием новых соединений и позволяет существующим соединениям завершиться.
     */
    void stop();

    /**
     * @english
     * @brief Checks if the server is currently running.
     * @return bool True if server is running, false otherwise.
     *
     * @russian
     * @brief Проверяет, запущен ли сервер в данный момент.
     * @return bool True если сервер запущен, false в противном случае.
     */
    bool isRunning() const;

    /**
     * @english
     * @brief Gets the port number the server is listening on.
     * @return unsigned short Port number.
     *
     * @russian
     * @brief Получает номер порта, который прослушивает сервер.
     * @return unsigned short Номер порта.
     */
    unsigned short getPort() const;

    /**
     * @english
     * @brief Gets the number of currently active client connections.
     * @return size_t Number of active connections.
     *
     * @russian
     * @brief Получает количество текущих активных клиентских соединений.
     * @return size_t Количество активных соединений.
     */
    size_t getActiveConnections() const;

private:
    /**
     * @english
     * @brief Begins asynchronous acceptance of new client connections.
     *
     * @russian
     * @brief Начинает асинхронный прием новых клиентских соединений.
     */
    void startAccept();

    /**
     * @english
     * @brief Handles a successfully accepted client connection.
     * @param socket Shared pointer to the accepted TCP socket.
     * @param error ASIO error code indicating acceptance result.
     *
     * @russian
     * @brief Обрабатывает успешно принятое клиентское соединение.
     * @param socket Общий указатель на принятый TCP-сокет.
     * @param error Код ошибки ASIO, указывающий результат принятия.
     */
    void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);

    /**
     * @english
     * @brief Spawns a new thread to handle client communication.
     * @param socket Shared pointer to the client TCP socket.
     *
     * @russian
     * @brief Запускает новый поток для обработки клиентской коммуникации.
     * @param socket Общий указатель на клиентский TCP-сокет.
     */
    void spawnClientHandler(std::shared_ptr<tcp::socket> socket);

    /**
     * @english
     * @brief Handles client communication in a separate thread.
     * @param socket Shared pointer to the client TCP socket.
     *
     * @russian
     * @brief Обрабатывает клиентскую коммуникацию в отдельном потоке.
     * @param socket Общий указатель на клиентский TCP-сокет.
     */
    void handleClient(std::shared_ptr<tcp::socket> socket);

    /**
     * @english
     * @brief Determines the service type (upload-pack/receive-pack) from client request.
     * @param socket Client socket to read the request from.
     * @return std::string Service type or empty string on error.
     *
     * @russian
     * @brief Определяет тип сервиса (upload-pack/receive-pack) из запроса клиента.
     * @param socket Клиентский сокет для чтения запроса.
     * @return std::string Тип сервиса или пустая строка при ошибке.
     */
    std::string determineServiceType(std::shared_ptr<tcp::socket> socket);

    /**
     * @english
     * @brief Executes the appropriate protocol handler based on service type.
     * @param socket Client socket for communication.
     * @param service_type Type of service requested ("upload-pack" or "receive-pack").
     * @return bool True if protocol completed successfully, false otherwise.
     *
     * @russian
     * @brief Выполняет соответствующий обработчик протокола на основе типа сервиса.
     * @param socket Клиентский сокет для коммуникации.
     * @param service_type Тип запрошенного сервиса ("upload-pack" или "receive-pack").
     * @return bool True если протокол завершен успешно, false в противном случае.
     */
    bool executeProtocol(std::shared_ptr<tcp::socket> socket, const std::string& service_type);

    /**
     * @english
     * @brief Removes a client connection from active connections tracking.
     * @param socket Socket to remove from tracking.
     *
     * @russian
     * @brief Удаляет клиентское соединение из отслеживания активных соединений.
     * @param socket Сокет для удаления из отслеживания.
     */
    void removeClientConnection(std::shared_ptr<tcp::socket> socket);

    /**
     * @english
     * @brief Sends a server notification via event bus.
     * @param message Notification message.
     *
     * @russian
     * @brief Отправляет серверное уведомление через шину событий.
     * @param message Сообщение уведомления.
     */
    void notifyServerEvent(const std::string& message) const;

    /**
     * @english
     * @brief Sends a client connection notification via event bus.
     * @param message Connection event message.
     *
     * @russian
     * @brief Отправляет уведомление о клиентском соединении через шину событий.
     * @param message Сообщение о событии соединения.
     */
    void notifyClientConnection(const std::string& message) const;

    /**
     * @english
     * @brief Sends an error notification via event bus.
     * @param message Error message.
     *
     * @russian
     * @brief Отправляет уведомление об ошибке через шину событий.
     * @param message Сообщение об ошибке.
     */
    void notifyServerError(const std::string& message) const;

private:
    /**
     * @english
     * @brief ASIO I/O context for asynchronous operations.
     *
     * @russian
     * @brief ASIO I/O контекст для асинхронных операций.
     */
    boost::asio::io_context& io_context_;

    /**
     * @english
     * @brief TCP acceptor for incoming connections.
     *
     * @russian
     * @brief TCP акцептор для входящих соединений.
     */
    tcp::acceptor acceptor_;

    /**
     * @english
     * @brief Repository manager for object and reference operations.
     *
     * @russian
     * @brief Менеджер репозиториев для операций с объектами и ссылками.
     */
    std::shared_ptr<RepositoryManager> repository_manager_;

    /**
     * @english
     * @brief Event bus for system notifications.
     *
     * @russian
     * @brief Шина событий для системных уведомлений.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Atomic flag indicating server running state.
     *
     * @russian
     * @brief Атомарный флаг, указывающий состояние работы сервера.
     */
    std::atomic<bool> is_running_;

    /**
     * @english
     * @brief Set of active client connections for tracking.
     *
     * @russian
     * @brief Набор активных клиентских соединений для отслеживания.
     */
    std::unordered_set<std::shared_ptr<tcp::socket>> active_connections_;

    /**
     * @english
     * @brief Mutex for protecting active connections set.
     *
     * @russian
     * @brief Мьютекс для защиты набора активных соединений.
     */
    mutable std::mutex connections_mutex_;
};