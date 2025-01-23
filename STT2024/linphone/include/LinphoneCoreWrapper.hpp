// LinphoneCoreWrapper.h
#ifndef LINPHONE_CORE_WRAPPER_H
#define LINPHONE_CORE_WRAPPER_H

#include <pybind11/pybind11.h>
#include <linphone++/linphone.hh>
#include <queue>
#include <memory>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include "LinphoneCallWrapper.hpp"

namespace py = pybind11;

// Forward declaration of CustomCoreListener to resolve circular dependency
class CustomCoreListener;

class LinphoneCoreWrapper {
public:
    LinphoneCoreWrapper();
    LinphoneCoreWrapper(const std::string &configPath);
    ~LinphoneCoreWrapper();

    void addCall(const std::shared_ptr<linphone::Call> incoming_call);
    void removeCall(const std::shared_ptr<linphone::Call> exiting_call);
    bool start();
    bool stop();
    bool hasActiveCalls();
    py::object getCall();
    bool getOn() const;
    int activeCallCount();
    std::shared_ptr<LinphoneCallWrapper> getCallWrapperFromCall(std::shared_ptr<linphone::Call> call);
    std::list<std::string> getSoundDeviceList();
    void setSoundSource(const std::string& device);
    std::string getSipAddress();
    void setSipAddress(const std::string& address);
    void run();
    void stopRun();
    bool loadXmlConfigFromString(const std::string& configString);
    std::string getConfigAsXML();
private:
    std::shared_ptr<linphone::Core> core;
    std::queue<std::shared_ptr<LinphoneCallWrapper>> calls;
    bool on = false;
    std::map<linphone::Call*, std::shared_ptr<LinphoneCallWrapper>> callMap;
    std::atomic<bool> running = false;
    void runFunc(std::shared_ptr<linphone::Core> core);
    std::thread runThread;
};

#endif // LINPHONE_CORE_WRAPPER_H
