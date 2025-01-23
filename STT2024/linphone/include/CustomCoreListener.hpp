// CustomCoreListener.h
#ifndef CUSTOM_CORE_LISTENER_H
#define CUSTOM_CORE_LISTENER_H

#include "LinphoneCoreWrapper.hpp"
#include <linphone++/linphone.hh>
#include <string>

class CustomCoreListener : public linphone::CoreListener {
public:
    CustomCoreListener(LinphoneCoreWrapper* lcw);
    virtual ~CustomCoreListener() {}

    void onCallStateChanged(const std::shared_ptr<linphone::Core> &lc, const std::shared_ptr<linphone::Call> &call, linphone::Call::State cstate, const std::string &message) override;

private:
    LinphoneCoreWrapper* core;
};

#endif // CUSTOM_CORE_LISTENER_H
