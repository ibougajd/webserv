#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <vector>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

#define PORT 8080

// === Load static files ===
std::string loadFile(const std::string& path) {
    std::ifstream file(path.c_str());
    std::stringstream buffer;
    if (file)
        buffer << file.rdbuf();
    return buffer.str();
}

// === Detect MIME type ===
std::string getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    if (path.find(".gif") != std::string::npos) return "image/gif";
    return "text/plain";
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server running on http://localhost:" << PORT << "\n";

    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));
        recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        std::string rawRequest(buffer);
        HttpRequest request = parseHttpRequest(rawRequest);

        if (request.path == "/")
            request.path = "/index.html";

        std::string body = loadFile("www" + request.path);

        HttpResponse response;
        if (body.empty()) {
            response.statusCode = 404;
            response.statusMessage = "Not Found";
            body = "<h1>404 Not Found</h1>";
        } else {
            response.statusCode = 200;
            response.statusMessage = "OK";
        }

        response.body = body;
        response.headers["Content-Type"] = getMimeType(request.path);

        std::ostringstream lenStream;
        lenStream << body.size();
        response.headers["Content-Length"] = lenStream.str();

        std::string fullResponse = response.toString();
        send(client_fd, fullResponse.c_str(), fullResponse.size(), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
