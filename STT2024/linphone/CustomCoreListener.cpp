#include "CustomCoreListener.hpp"

CustomCoreListener::CustomCoreListener(LinphoneCoreWrapper* lcw) {
    core = lcw;
}

void CustomCoreListener::onCallStateChanged(const std::shared_ptr<linphone::Core> &lc, const std::shared_ptr<linphone::Call> &call, linphone::Call::State cstate, const std::string &message) {    
    auto callWrapped = core->getCallWrapperFromCall(call);
    
    std::string callBackState;

    switch(cstate) {
        case linphone::Call::State::Idle: 
            callBackState = "Idle";
            break;
        case linphone::Call::State::IncomingReceived: 
            callBackState = "IncomingReceived";
            break;
        case linphone::Call::State::OutgoingInit:
            callBackState = "OutgoingInit";
            break;
        case linphone::Call::State::OutgoingProgress:
            callBackState = "OutgoingProgress";
            break;
        case linphone::Call::State::OutgoingRinging:
            callBackState = "OutgoingRinging";
            break;
        case linphone::Call::State::OutgoingEarlyMedia: 
            callBackState = "OutgoingEarlyMedia";
            break;
        case linphone::Call::State::Connected:
            callBackState = "Connected";
            break;
        case linphone::Call::State::StreamsRunning: 
            callBackState = "StreamsRunning";
            break;
        case linphone::Call::State::Pausing: 
            callBackState = "Pausing";
            break;
        case linphone::Call::State::Paused: 
            callBackState = "Paused";
            break;
        case linphone::Call::State::Resuming: 
            callBackState = "Resuming";
            break;
        case linphone::Call::State::Referred:
            callBackState = "Referred";
            break;
        case linphone::Call::State::Error:
            callBackState = "Error";
            break;
        case linphone::Call::State::End:
            callBackState = "End";
            break;
        case linphone::Call::State::PausedByRemote:
            callBackState = "PausedByRemote";
            break;
        case linphone::Call::State::UpdatedByRemote: 
            callBackState = "UpdatedByRemote";
            break;
        case linphone::Call::State::IncomingEarlyMedia:
            callBackState = "IncomingEarlyMedia";
            break;
        case linphone::Call::State::Updating: 
            callBackState = "Updating";
            break;
        case linphone::Call::State::Released: 
            callBackState = "Released";
            break;
        case linphone::Call::State::EarlyUpdatedByRemote: 
            callBackState = "EarlyUpdatedByRemote";
            break;
        case linphone::Call::State::EarlyUpdating: 
            callBackState = "EarlyUpdating";
            break;
        default: 
            callBackState = "Unknown State";
            break;
    }

    if (callWrapped) {
        callWrapped->triggerCallback(callBackState);
    }

    switch(cstate) {
        case linphone::Call::State::IncomingReceived:
            core->addCall(call);
            break;
        case linphone::Call::State::Released:
            core->removeCall(call);
            break;
        case linphone::Call::State::Error:
            std::cout << "Call error" << std::endl;
            break;
        default:
            break;
    }   
}