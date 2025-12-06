#include "../include/NetworkUtils.hxx"
#include <system_error>
#include <sstream>
#include <iostream>

namespace svcs::platform {

#ifdef _WIN32
static bool networkInitialized = false;
static WSADATA wsaData;
#endif

bool initializeNetwork() {
#ifdef _WIN32
    if (!networkInitialized) {
        WORD version = MAKEWORD(2, 2);
        int result = WSAStartup(version, &wsaData);
        if (result != 0) {
            std::cerr << "Failed to initialize Winsock: " << result << std::endl;
            return false;
        }
        networkInitialized = true;
    }
#endif
    return true;
}

void cleanupNetwork() {
#ifdef _WIN32
    if (networkInitialized) {
        WSACleanup();
        networkInitialized = false;
    }
#endif
}

SocketHandle createServerSocket(const std::string& host, uint16_t port) {
    if (!initializeNetwork()) {
        return INVALID_SOCKET_HANDLE;
    }

    // Создаем сокет
    SocketHandle sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET_HANDLE) {
        return INVALID_SOCKET_HANDLE;
    }

    // Устанавливаем опцию REUSEADDR
    int opt = 1;
#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    // Настраиваем адрес
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (host.empty() || host == "0.0.0.0" || host == "any") {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        // Пробуем конвертировать как IP адрес
        if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
            // Не удалось как IP, пробуем как hostname
            // Используем более современный getaddrinfo вместо устаревшего gethostbyname
            addrinfo hints{}, *result = nullptr;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            if (getaddrinfo(host.c_str(), nullptr, &hints, &result) == 0 && result != nullptr) {
                sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(result->ai_addr);
                serverAddr.sin_addr = addr->sin_addr;
                freeaddrinfo(result);
            } else {
                closeSocket(sock);
                return INVALID_SOCKET_HANDLE;
            }
        }
    }

    // Привязываем сокет
    if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        closeSocket(sock);
        return INVALID_SOCKET_HANDLE;
    }

    // Начинаем слушать
    if (listen(sock, SOMAXCONN) == -1) {
        closeSocket(sock);
        return INVALID_SOCKET_HANDLE;
    }

    return sock;
}

void closeSocket(SocketHandle sock) {
    if (sock != INVALID_SOCKET_HANDLE) {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }
}

bool setSocketNonBlocking(SocketHandle sock) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(sock, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
}

std::string getClientIpFromSocket(SocketHandle sock) {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    if (getpeername(sock, (sockaddr*)&clientAddr, &clientLen) == 0) {
        char ip_str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &clientAddr.sin_addr, ip_str, sizeof(ip_str))) {
            return std::string(ip_str);
        }
    }
    return "";
}

SocketHandle acceptConnection(SocketHandle serverSocket) {
    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);

    SocketHandle clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
    return clientSocket;
}

}