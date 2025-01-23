#include "LinphoneCallWrapper.hpp"

// class LinphoneCallWrapper {
// public:
//     LinphoneCallWrapper(const std::shared_ptr<linphone::Call> c);
//     ~LinphoneCallWrapper();
//     void getStream();

// private:
//     std::shared_ptr<linphone::Call> call;
// };

LinphoneCallWrapper::LinphoneCallWrapper(const std::shared_ptr<linphone::Call> c) {
    call = c;
}

LinphoneCallWrapper::~LinphoneCallWrapper() {}

void LinphoneCallWrapper::getStream() {}

bool LinphoneCallWrapper::acceptCall() {
    bool result = (bool)(call->accept() + 1);
    return result;
}

void LinphoneCallWrapper::triggerCallback(const std::string& state) {
    // std::cout << state << std::endl;
    if (callback)
        callback(state);
    else
        std::cout << "No callback??" << std::endl;
    // callBack(state);
}

std::string LinphoneCallWrapper::callStatus() {
    switch(call->getState()) {
        case linphone::Call::State::Idle: return "Idle";
        case linphone::Call::State::IncomingReceived: return "IncomingReceived";
        case linphone::Call::State::OutgoingInit: return "OutgoingInit";
        case linphone::Call::State::OutgoingProgress: return "OutgoingProgress";
        case linphone::Call::State::OutgoingRinging: return "OutgoingRinging";
        case linphone::Call::State::OutgoingEarlyMedia: return "OutgoingEarlyMedia";
        case linphone::Call::State::Connected: return "Connected";
        case linphone::Call::State::StreamsRunning: return "StreamsRunning";
        case linphone::Call::State::Pausing: return "Pausing";
        case linphone::Call::State::Paused: return "Paused";
        case linphone::Call::State::Resuming: return "Resuming";
        case linphone::Call::State::Referred: return "Referred";
        case linphone::Call::State::Error: return "Error";
        case linphone::Call::State::End: return "End";
        case linphone::Call::State::PausedByRemote: return "PausedByRemote";
        case linphone::Call::State::UpdatedByRemote: return "UpdatedByRemote";
        case linphone::Call::State::IncomingEarlyMedia: return "IncomingEarlyMedia";
        case linphone::Call::State::Updating: return "Updating";
        case linphone::Call::State::Released: return "Released";
        case linphone::Call::State::EarlyUpdatedByRemote: return "EarlyUpdatedByRemote";
        case linphone::Call::State::EarlyUpdating: return "EarlyUpdating";
        default: return "Unknown State";
    }
}

void LinphoneCallWrapper::hello() {
    std::cout << "Hello" << std::endl;
    return;
}

void LinphoneCallWrapper::setCallBackFunction(const std::function<void(const std::string&)>& cb) {
    std::cout << "setting callback function" << std::endl;
    callback = cb;
}

std::function<void(const std::string&)> LinphoneCallWrapper::getCallback() {
    return callback;
}