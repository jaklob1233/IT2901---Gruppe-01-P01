//
// Created by norman.jansen on 05.12.2024.
//

#ifndef LINPHONE_SDK_MQTTTRANSPORT_MESSAGEBUFFER_H
#define LINPHONE_SDK_MQTTTRANSPORT_MESSAGEBUFFER_H

#include <vector>
#include <string>
#include <mutex>  // Include for std::mutex
#include <optional>

namespace org::openapitools::mqtttransport::server::api {

    // Define the mqtt_message struct
    struct mqtt_message {
        std::string sender_id;
        std::string receiver_id;
        std::string message_id;
        std::string message;  // The actual message content
        long timestamp;
    };

    class MessageBuffer {
    private:
        std::vector<mqtt_message> buffer;  // Store mqtt_message objects in the buffer
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

        // Add a new mqtt_message to the buffer
        void add_message(const mqtt_message &message) {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            buffer.push_back(message);
        }

        // Get the next mqtt_message from the buffer
        std::optional<mqtt_message> get_message() {
            std::lock_guard<std::mutex> lock(mtx);
            if (buffer.empty()) {
                return std::nullopt;  // Return an empty optional if no messages are left
            }
            mqtt_message firstMessage = buffer.front();
            buffer.erase(buffer.begin());
            return firstMessage;
        }

        // Optionally get the entire buffer (to inspect or manipulate it)
        const std::vector<mqtt_message> &getBuffer() {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            return buffer;
        }
    };
}

#endif // LINPHONE_SDK_MQTTTRANSPORT_MESSAGEBUFFER_H
