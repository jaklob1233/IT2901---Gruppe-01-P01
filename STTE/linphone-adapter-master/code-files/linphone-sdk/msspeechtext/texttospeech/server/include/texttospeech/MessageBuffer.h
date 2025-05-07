//
// Created by norman.jansen on 05.12.2024.
//

#ifndef LINPHONE_SDK_TEXTTOSPEECH_MESSAGEBUFFER_H
#define LINPHONE_SDK_TEXTTOSPEECH_MESSAGEBUFFER_H

#include <vector>
#include <string>
#include <mutex>  // Include for std::mutex
#include <optional>

namespace org::openapitools::texttospeech::server::api {

    // Define the audio_post struct
    struct audio_post {
        int sample_rate;
        int length;
        long timestamp;
        std::string message_id;
        std::string byte_array;  // Represents audio data as a string
    };

    class MessageBuffer {
    private:
        std::vector<audio_post> buffer;  // Store audio posts in a vector
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

        // Add a new audio_post to the buffer
        void add_message(const audio_post &message) {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            buffer.push_back(message);
        }

        // Get the next audio_post from the buffer
        std::optional<audio_post> get_message() {
            std::lock_guard<std::mutex> lock(mtx);
            if (buffer.empty()) {
                return std::nullopt;  // Return an empty optional if no messages are left
            }
            audio_post firstMessage = buffer.front();
            buffer.erase(buffer.begin());
            return firstMessage;
        }

        // Optionally get the entire buffer (to inspect or manipulate it)
        const std::vector<audio_post> &getBuffer() {
            std::lock_guard<std::mutex> lock(mtx);  // Lock the mutex during this operation
            return buffer;
        }
    };
}

#endif // LINPHONE_SDK_TEXTTOSPEECH_MESSAGEBUFFER_H
