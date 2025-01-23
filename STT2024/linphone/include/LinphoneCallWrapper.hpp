#ifndef LINPHONE_CALL_WRAPPER_H
#define LINPHONE_CALL_WRAPPER_H

#include <linphone++/linphone.hh>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <string>
#include <iostream>

namespace py = pybind11;

class LinphoneCallWrapper {
public:
    LinphoneCallWrapper(const std::shared_ptr<linphone::Call> c);
    ~LinphoneCallWrapper();
    void getStream();
    bool acceptCall();
    std::string callStatus();
    void setCallBackFunction(const std::function<void(const std::string&)>& cb);
    std::function<void(const std::string&)> getCallback();
    void triggerCallback(const std::string& state);
    void hello();

    std::shared_ptr<linphone::Call> call;
    std::function<void(const std::string&)> callback;
};

#endif