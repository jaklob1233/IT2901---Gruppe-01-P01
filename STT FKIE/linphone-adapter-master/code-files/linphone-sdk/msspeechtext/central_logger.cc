#include "central_logger.h"
#include "mediastreamer2/mscommon.h"

using json = nlohmann::json;

CentralLogger* CentralLogger::instance = nullptr;

CentralLogger::CentralLogger() : curl(nullptr), headers(nullptr) {
    curl = curl_easy_init();
    if (!curl) {
        ms_error("Failed to initialize CentralLogger (CURL)");
        exit(1);
    }

    // Set HTTP headers
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

CentralLogger* CentralLogger::getInstance() {
    if (!instance) {
        instance = new CentralLogger();
    }
    return instance;
}

CentralLogger::~CentralLogger() {
    if (curl) {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
    if (headers) {
        curl_slist_free_all(headers);
        headers = nullptr;
    }
}

std::string CentralLogger::loggingEventTypeToString(LoggingEventType type) {
    switch (type) {
        case LoggingEventType::Triggered: return "triggered";
        case LoggingEventType::Sent: return "sent";
        case LoggingEventType::Received: return "received";
        case LoggingEventType::Processed: return "processed";
        default: return "unknown";
    }
}

void CentralLogger::sendLog(
        const std::string &vectorUrl,
        const std::string &loggingSystemID,
        LoggingEventType loggingEventType,
        long timestamp,
        const std::string &sourceSystemID,
        const std::vector<std::string> &destinationSystemIDs,
        const std::string& messageID,
        const std::string &subject,
        const std::string &textMessage
) {
    if (!curl) {
        ms_error("CentralLogger is not initialized!");
        return;
    }

    // Create JSON log object
    json logEntry = {
            {"textMessageLog", {
                    {"loggingSystemID", loggingSystemID},
                    {"loggingEventType", loggingEventTypeToString(loggingEventType)},
                    {"timestamp", timestamp},
                    {"sourceSystemID", sourceSystemID},
                    {"destinationSystemIDs", destinationSystemIDs},
                    {"messageID", messageID},
                    {"subject", subject},
                    {"text", textMessage}
            }}
    };

    // Convert JSON to string
    std::string logData = logEntry.dump();

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, vectorUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, logData.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, logData.size());

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        ms_error("Failed to send log! Error: %s", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200) {
            ms_message("Log sent successfully!");
        } else {
            ms_error("Failed to send log! HTTP Code: %lon", response_code);
        }
    }
}

long CentralLogger::current_epoch_ms() {
    struct timespec ts = {0};  // Zero-initialize to prevent Clang-Tidy warnings
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}