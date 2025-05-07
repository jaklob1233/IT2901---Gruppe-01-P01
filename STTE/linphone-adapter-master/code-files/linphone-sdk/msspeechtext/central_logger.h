#pragma once  // Prevents multiple inclusions

#include <iostream>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

class CentralLogger {
public:
    enum class LoggingEventType { Triggered, Sent, Received, Processed };

    static CentralLogger* getInstance();
    void sendLog(const std::string& vectorUrl,
                 const std::string& loggingSystemID,
                 LoggingEventType loggingEventType,
                 long timestamp,
                 const std::string& sourceSystemID,
                 const std::vector<std::string>& destinationSystemIDs,
                 const std::string& messageID,
                 const std::string& subject,
                 const std::string& textMessage);

    static long current_epoch_ms();

private:
    static CentralLogger* instance;
    CURL* curl;
    struct curl_slist* headers;
    CentralLogger();
    ~CentralLogger();
    std::string loggingEventTypeToString(LoggingEventType type);
};

extern CentralLogger *logger;  // Declare the logger