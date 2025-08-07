#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

struct HttpResponse {
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string toString() const {
        std::ostringstream res;
        res << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            res << it->first << ": " << it->second << "\r\n";
        }
        res << "\r\n" << body;
        return res.str();
    }
};

#endif
