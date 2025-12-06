/**
* @file NetworkUtils.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @brief Cross-platform network utilities
 */
#pragma once

#include <string>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <errno.h>
#endif

/**
 * @brief Platform abstraction layer
 * @details This namespace contains cross-platform wrappers for operating system
 *          functionality, allowing the code to work seamlessly on Windows,
 *          Linux, macOS, and other POSIX systems.
 *
 * @english
 * Platform abstraction layer
 * This namespace contains cross-platform wrappers for operating system
 * functionality, allowing the code to work seamlessly on Windows,
 * Linux, macOS, and other POSIX systems.
 *
 * @russian
 * Слой абстракции платформы
 * Это пространство имен содержит кросс-платформенные обертки для функциональности
 * операционной системы, позволяя коду работать без изменений на Windows,
 * Linux, macOS и других POSIX-системах.
 */
namespace svcs::platform {

#ifdef _WIN32
    typedef SOCKET SocketHandle;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#define SOCKET_ERROR_CODE WSAGetLastError()
#else
    typedef int SocketHandle;
#define INVALID_SOCKET_HANDLE (-1)
#define SOCKET_ERROR_CODE errno
#endif

    /**
     * @brief Initialize network subsystem (Windows only)
     * @return true if successful, false otherwise
     */
    bool initializeNetwork();

    /**
     * @brief Cleanup network subsystem (Windows only)
     */
    void cleanupNetwork();

    /**
     * @brief Create server socket
     * @param host Host address to bind to (empty string for any)
     * @param port Port to bind to
     * @return Socket handle or INVALID_SOCKET_HANDLE on error
     */
    SocketHandle createServerSocket(const std::string& host, uint16_t port);

    /**
     * @brief Close socket
     * @param sock Socket handle to close
     */
    void closeSocket(SocketHandle sock);

    /**
     * @brief Set socket non-blocking
     * @param sock Socket handle
     * @return true if successful, false otherwise
     */
    bool setSocketNonBlocking(SocketHandle sock);

    /**
     * @brief Get client IP from socket
     * @param sock Socket handle
     * @return Client IP address as string
     */
    std::string getClientIpFromSocket(SocketHandle sock);

    /**
     * @brief Accept a connection on a socket
     * @param serverSocket Server socket handle
     * @return Client socket handle or INVALID_SOCKET_HANDLE on error
     */
    SocketHandle acceptConnection(SocketHandle serverSocket);

}