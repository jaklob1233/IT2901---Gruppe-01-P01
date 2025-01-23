#include "LinphoneCoreWrapper.hpp"
#include "CustomCoreListener.hpp"

LinphoneCoreWrapper::LinphoneCoreWrapper() {
    auto factory = linphone::Factory::get();
    auto listener = std::make_shared<CustomCoreListener>(this);
    core = factory->createCore("", "", nullptr);
    core->addListener(listener);
    linphone::LoggingService::get()->setLogLevel(linphone::LogLevel::Fatal);
}

LinphoneCoreWrapper::LinphoneCoreWrapper(const std::string &configPath) {
    auto factory = linphone::Factory::get();
    auto listener = std::make_shared<CustomCoreListener>(this);
    auto config = factory->createConfigWithFactory(configPath, "");
    core = factory->createCoreWithConfig(config, nullptr);
    core->addListener(listener);
    linphone::LoggingService::get()->setLogLevel(linphone::LogLevel::Fatal);
}

LinphoneCoreWrapper::~LinphoneCoreWrapper() {}

bool LinphoneCoreWrapper::loadXmlConfigFromString(const std::string& configString) {
    int result = core->getConfig()->loadFromXmlString(configString);
    if (result == 0) return true;
    return false;
}

std::string LinphoneCoreWrapper::getConfigAsXML() {
    return core->getConfig()->dumpAsXml();
}

bool LinphoneCoreWrapper::start() {
    if (on) 
        return false;

    int status = core->start();
    if (status == 0) {
        on = true;
        return true;
    }

    return false;
}

bool LinphoneCoreWrapper::stop() {
    if(!on) {
        return false;
    }

    core->stop();
    on = false;
    return true;
}

bool LinphoneCoreWrapper::hasActiveCalls() {
    return !calls.empty();
}

int LinphoneCoreWrapper::activeCallCount() {
    return calls.size();
}

py::object LinphoneCoreWrapper::getCall() {
    if (calls.empty())
        return py::none();
    auto item = calls.front().get();
    calls.pop();
    return py::cast(item);
}

void LinphoneCoreWrapper::addCall(const std::shared_ptr<linphone::Call> incoming_call) {
    auto call = std::make_shared<LinphoneCallWrapper>(incoming_call); 
    calls.push(call);
    callMap[incoming_call.get()] = call;
}

std::shared_ptr<LinphoneCallWrapper> LinphoneCoreWrapper::getCallWrapperFromCall(std::shared_ptr<linphone::Call> call) {
    return callMap[call.get()];
}

bool LinphoneCoreWrapper::getOn() const {
    return on;
}

void LinphoneCoreWrapper::removeCall(const std::shared_ptr<linphone::Call> exiting_call) {
    callMap.erase(exiting_call.get());
}

std::list<std::string> LinphoneCoreWrapper::getSoundDeviceList() {
    return core->getSoundDevicesList();
}

void LinphoneCoreWrapper::setSoundSource(const std::string& device) {
    core->setMediaDevice(device);
}

std::string LinphoneCoreWrapper::getSipAddress() {
    return core->getConfig()->getString("sip", "contact", "");
}

void LinphoneCoreWrapper::setSipAddress(const std::string& address) {
    core->getConfig()->setString("sip", "contact", address);
}

void LinphoneCoreWrapper::runFunc(std::shared_ptr<linphone::Core> core) {
    while (running.load()) {
        core->iterate();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void LinphoneCoreWrapper::run() {
    running = true;
    runThread = std::thread(&LinphoneCoreWrapper::runFunc, this, core);
}

void LinphoneCoreWrapper::stopRun() {
    if (running) {
        running = false;
        if (runThread.joinable()) {
            runThread.join();
        }
    }
}

