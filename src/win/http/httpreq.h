/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <string>
#include <vector>
#include <map>
#include "navitab/logger.h"

namespace navitab {

class HttpReq {
public:
    HttpReq(std::unique_ptr<logging::Logger> &l);
    virtual ~HttpReq();

    bool feedData(char *fragment, int n); // returns true if request headers have all been received
    bool hasError() const; // returns true if headers could not be parsed
    bool keepAlive() const; // returns true if headers included 'Connection: keep-alive'

    bool getQueryString(const char *q, std::string &val);
    std::string getUrl() const;

private:
    enum { kMethod, kUrl, kVersion, kHeader, kComplete, kError };

    void feedMethod(char *&req, int &n);
    void feedUrl(char *&req, int &n);
    void feedVersion(char *&req, int &n);
    void feedHeader(char *&req, int &n);

    void extractQueryStrings();

private:
    std::unique_ptr<logging::Logger> &LOG;
    int feedState = kMethod;
    std::string working;
    std::string method;
    std::string url;
    std::string version;
    std::map<std::string, std::string> queryStrings;
    std::vector<std::string> headers;
};

} // namespace navitab
