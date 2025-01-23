#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <linphone++/linphone.hh>
#include <queue>
#include <iostream>
#include <unistd.h>
#include "LinphoneCoreWrapper.hpp"
#include "LinphoneCallWrapper.hpp"
#include "CustomCoreListener.hpp"

PYBIND11_MODULE(linphone_module, m) {
    m.doc() = R"()";
    py::class_<LinphoneCoreWrapper>(m, "Core", R"(
        The Core class is a wrapper for the Linphone::Core object in its C++ library. The Core is limited in capability and it only has implemented logic for calls and sound sources.

        Constructor:
            config_path (str): Path to config file. Optional
        
        Properties:
            hasActiveCalls (bool): Check if there are any active calls
            activeCallCount (int): Count of active calls
            soundSources (string[]): List of all sound sources
            sipAddress (string): Cores Sip Address
            on (bool): If the core is turned on or not

        Functions:
            start (None) -> bool: 
                Starts the Core. 
                Returns True if Core stared, False otherwise. 
            stop (None) -> bool:
                Stops the Core. 
                Returns True if Core stopped, False otherwise.
            run (None) -> None: 
                Starts thread listening for calls.
                Returns None
            stopRun (None) -> None: 
                Stops thread listening for calls.
                Returns None
            getCall (None) -> Call: 
                Returns the first active Call object.
            setSoundSource (str) -> None:
                Sets the active sound source
                Returns None 

        Basic usage:
            config_path = "Path_to_config"
            core = Core(config_path)

            try:
                # starting the core
                core.start()
                core.run()

                while True:
                    if core.hasActiveCalls:
                        call = core.getCall()
                        # handle call
                    time.sleep(0.1) # add sleep to free resources

            except KeyboardInterrupt:
                # stopping the core
                core.stopRun()
                core.stop()

        )") 
        .def(py::init<>(), R"(
            Creates a Core object without a config.

            Returns:
                A new Core object.
        )")
        .def(py::init<const std::string &>(), py::arg("config_path"), R"(
            Creates a Core object with a config

            Returns:
                A new Core Object
        )")
        .def("start", &LinphoneCoreWrapper::start, R"(
            Start the core

            Parameters:
                None
            
            Returns:
                bool: True if Core started, False otherwise.
            )")
        .def("stop", &LinphoneCoreWrapper::stop, R"(
            Stop the core

            Parameters:
                None
            
            Returns:
                bool: True if Core stopped, False otherwise
            )")
        .def("getCall", &LinphoneCoreWrapper::getCall, R"(
            Get first active call

            Parameters:
                None

            Returns:
                Call: First active Call object
            )")
        .def("run", &LinphoneCoreWrapper::run, R"(
            Start thread for listening after incoming calls.
            
            Parameters:
                None
            
            Returns:
                None
            )")
        .def("stopRun", &LinphoneCoreWrapper::stopRun, R"(
            Stops thread listening after incoming calls.

            Parameters:
                None
            
            Returns:
                None
            )")
        .def("setSoundSource", &LinphoneCoreWrapper::setSoundSource, py::arg("sound_source"), R"(
            Set sound source
            
            Parameters:
                sound_source (str): sound source to set active. Use soundSources property to get list of sound sources
            
            Returns:
                None
            )")
        .def("loadConfigFromXML", &LinphoneCoreWrapper::loadXmlConfigFromString, py::arg("config_string"), R"(
            Loads Linphone config from XML string

            Parameters:
                config_string (str): XML config string

            Returns:
                bool: True if load was successful, False otherwise
        )")
        .def("getConfigAsXML", &LinphoneCoreWrapper::getConfigAsXML, R"(
            Gets config as XML

            Parameters:
                None
            
            Returns:
                str: Config as xml string
        )")
        .def_property("hasActiveCalls", &LinphoneCoreWrapper::hasActiveCalls, nullptr, R"(
            Check if Core has active calls.

            Setter:
                None

            Getter:
                bool: True if there is any active calls, False otherwise.
        )")
        .def_property("activeCallCount", &LinphoneCoreWrapper::activeCallCount, nullptr, R"(
            Get active call count.

            Setter:
                None

            Getter:
                int: Count of the active calls.
        )")
        .def_property("soundSources", &LinphoneCoreWrapper::getSoundDeviceList, nullptr, R"(
            Get all sound sources the Core has access to.

            Setter:
                None

            Getter:
                string[]: list of all sound sources
        )")
        .def_property("sipAddress", &LinphoneCoreWrapper::getSipAddress, &LinphoneCoreWrapper::setSipAddress, R"(
            Get or set the sipAddress
            
            Setter:
                address (string): SIP address to use

            Getter:
                string: current SIP address of Core
        )")
        .def_property("on", &LinphoneCoreWrapper::getOn, nullptr, R"(
            Check if Core is on or not

            Setter:
                None

            Getter:
                bool: True if Core is on, False otherwise
        )");
    py::class_<LinphoneCallWrapper>(m, "Call", R"(
        The Call class is a wrapper for the Linphone::Call object in its C++ library. The Call class is limited in its capability as it only lets you define a state change callback, and accept a call. A Call object is only created by the Core class, and thus cannot be created by a user.

        Constructor:
            None

        Properties:
            callback (function): Gets call state change callback function
            callStatus (string): Gets call status

        Functions:
            acceptCall (None) -> bool:
                Accept the call
                Returns True if call was accepted, False otherwise
            setCallback (function) -> None: 
                Sets the state change callback function
                See setCallback documentation for how the callback function should be structured and works
                Returns None
    )")
        .def("acceptCall", &LinphoneCallWrapper::acceptCall, R"(
            Accept a call

            Parameters:
                None

            Returns:
                bool: True if call was accepted, False otherwise
        )")
        .def("setCallback", &LinphoneCallWrapper::setCallBackFunction, py::arg("callback_function"), R"(
            Sets the call state change callback function

            Parameters:
                callback (function): The callback function
            
            Returns:
                None

            Callback function structure:
                The Callback function takes in the new state of the call. Therefore you should structure the callback function like this:
                function callback(state: str):
                    # handle state here

                See callStatus docs for all possible states.
            
            )")
        .def_property("callback", &LinphoneCallWrapper::getCallback, nullptr, R"(
            Get callback function

            Setter:
                None

            Getter:
                function: Call state change Callback function
        )")
        .def_property("callState", &LinphoneCallWrapper::callStatus, nullptr, R"(
            Get call state

            Setter:
                None
            
            Getter:
                string: Call state

            Possible call states:
                Idle,
                IncomingReceived,
                OutgoingInit,
                OutgoingProgress,
                OutgoingRinging,
                OutgoingEarlyMedia,
                Connected,
                StreamsRunning,
                Pausing,
                Paused,
                Resuming,
                Referred,
                Error,
                End,
                PausedByRemote,
                UpdatedByRemote,
                IncomingEarlyMedia,
                Updating,
                Released,
                EarlyUpdatedByRemote,
                EarlyUpdating,
                Unknown State
        )");
}