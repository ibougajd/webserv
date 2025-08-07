#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <map>

#define PORT 8080
#define BUFFER_SIZE 4096

// === MIME TYPE DETECTION ===
std::string get_mime_type(const std::string& path) {
    size_t dot = path.find_last_of(".");
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot + 1);
    static std::map<std::string, std::string> mime = {
        {"html", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"gif", "image/gif"},
        {"svg", "image/svg+xml"},
        {"txt", "text/plain"},
        {"json", "application/json"}
    };

    if (mime.count(ext))
        return mime[ext];
    return "application/octet-stream";
}

// === LOAD FILE (TEXT/BINARY) ===
bool load_file_binary(const std::string& path, std::string& data) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << file.rdbuf();
    data = ss.str();
    return true;
}

// === SEND 200 RESPONSE ===
void send_response(int client_socket, const std::string& content, const std::string& mime_type) {
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Length: " << content.size() << "\r\n"
             << "Content-Type: " << mime_type << "\r\n"
             << "Connection: close\r\n\r\n";

    std::string header = response.str();
    send(client_socket, header.c_str(), header.size(), 0);
    send(client_socket, content.c_str(), content.size(), 0);
}

// === SEND 404 RESPONSE ===
void send_404(int client_socket) {
    std::string content = "<h1>404 Not Found</h1>";
    std::ostringstream response;
    response << "HTTP/1.1 404 Not Found\r\n"
             << "Content-Length: " << content.size() << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Connection: close\r\n\r\n"
             << content;

    std::string response_str = response.str();
    send(client_socket, response_str.c_str(), response_str.size(), 0);
}

// === EXTRACT FILE PATH FROM REQUEST ===
std::string get_requested_path(const std::string& request) {
    std::istringstream iss(request);
    std::string method, path;
    iss >> method >> path;

    if (method != "GET") return "";
    if (path == "/") path = "/index.html";
    return "./www" + path;
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // CREATE SOCKET
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        return 1;
    }

    // BIND ADDRESS
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // LISTEN
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server running at http://localhost:" << PORT << std::endl;

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // RECEIVE REQUEST
        char buffer[BUFFER_SIZE] = {0};
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            close(client_socket);
            continue;
        }

        std::string request(buffer);
        std::string path = get_requested_path(request);
        std::string content;
        std::string mime = get_mime_type(path);

        if (load_file_binary(path, content))
            send_response(client_socket, content, mime);
        else
            send_404(client_socket);

        close(client_socket);
    }

    close(server_fd);
    return 0;
}
