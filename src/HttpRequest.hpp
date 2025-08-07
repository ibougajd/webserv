#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <sstream>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
};

HttpRequest parseHttpRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream stream(raw);
    std::string line;

    if (std::getline(stream, line)) {
        std::istringstream lstream(line);
        lstream >> req.method >> req.path >> req.version;
    }

    while (std::getline(stream, line) && line != "\r") {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            key.erase(key.find_last_not_of(" \t\r") + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t\r") + 1);
            req.headers[key] = val;
        }
    }

    return req;
}

#endif
