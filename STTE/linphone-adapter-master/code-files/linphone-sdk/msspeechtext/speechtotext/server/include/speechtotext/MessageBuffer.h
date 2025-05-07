//
// Created by norman.jansen on 05.12.2024.
//

#ifndef LINPHONE_SDK_SPEECHTOTEXT_MESSAGEBUFFER_H
#define LINPHONE_SDK_SPEECHTOTEXT_MESSAGEBUFFER_H

#include <vector>
#include <string>
#include <mutex>  // Include for std::mutex
#include <optional>

namespace org::openapitools::speechtotext::server::api {

    // Define the text_message struct
    struct text_message {
        long timestamp_start;  // Start time of the message
        long timestamp_end;    // End time of the message
        std::string text;      // The actual message content
    };

    class MessageBuffer {
    private:
        std::vector<text_message> buffer;  // Store text_message objects in the buffer
        std::mutex mtx;  // Mutex for thread safety

        // Private constructor ensures that no instances can be created outside of the class
        MessageBuffer() = default;

        // Delete copy constructor and assignment operator to prevent copies
        MessageBuffer(const MessageBuffer &) = delete;
        MessageBuffer &operator=(const MessageBuffer &) = delete;

    public:
        // Static method to get the singleton instance
        static MessageBuffer &getInstance() {
            static MessageBuffer instance;  // Local static variable ensures the instance is created only once
            return instance;
        }

        // Add a new text_message to the buffer
        void add_message(const text_message &message) {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            buffer.push_back(message);
        }

        // Get the next text_message from the buffer
        std::optional<text_message> get_message() {
            std::lock_guard<std::mutex> lock(mtx);
            if (buffer.empty()) {
                return std::nullopt;  // Return an empty optional if no messages are left
            }
            text_message firstMessage = buffer.front();
            buffer.erase(buffer.begin());
            return firstMessage;
        }

        // Optionally get the entire buffer (to inspect or manipulate it)
        const std::vector<text_message> &getBuffer() {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            return buffer;
        }
    };
}

#endif // LINPHONE_SDK_SPEECHTOTEXT_MESSAGEBUFFER_H
