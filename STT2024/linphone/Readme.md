# Linphone python library

Custom linphone library created using `pybind11` and `liblinphone-dev 4.4`.

## How it works

This is a custom python module that lets you create a valid VoIP client that can be called. The purpose of it is to be able to extract the live audio and transcribe it in real-time.

## How to install/use it

### Single script

To build the library from a single line, you can run `./install.sh` in the source directory which will install all the needed dependecies, compile the code, and copy the resulting binary to the correct directories.

### Manual build

To build the module manually, you need to use cmake. The library depends on `pybind11` and `liblinphone-dev 4.4`. After installing those, you can build the application by running `cmake --build build` in the `linphone` directory. When the code has compiled, you can copy the resulting `.so` file to the `linphone` directory and the source directory.

## Linphone module API

### Core

The Core class is a wrapper for the Linphone::Core object in its C++ library. The Core is limited in capability and it only has implemented logic for calls and sound sources.

#### Constructor
config_path (str): Path to config file. Optional

#### Properties
*hasActiveCalls* -> bool: Check if there are any active calls

*activeCallCount* -> int: Count of active calls

*soundSources* -> list[str]: List of all sound sources

*sipAddress* -> str: Cores Sip Address

*on* -> bool: If the core is turned on or not

#### Functions
*start* (None) -> bool: 
    Starts the Core. 
    Returns True if Core stared, False otherwise. 

*stop* (None) -> bool:
    Stops the Core. 
    Returns True if Core stopped, False otherwise.

*run* (None) -> None: 
    Starts thread listening for calls.
    Returns None

*stopRun* (None) -> None: 
    Stops thread listening for calls.
    Returns None
*getCall* (None) -> Call: 
    Returns the first active Call object.

*setSoundSource* (str) -> None:
    Sets the active sound source
    Returns None 

*loadConfigFromXML* (str) -> bool:
    Loads Linphone config from XML string
    Returns True if successful, False otherwise.

*getConfigAsXML* (None) -> str:
    Gets Linphone config as XML
    Returns XML config as string

#### Basic usage
```python
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
```

### Call

The Call class is a wrapper for the Linphone::Call object in its C++ library. The Call class is limited in its capability as it only lets you define a state change callback, and accept a call. A Call object is only created by the Core class, and thus cannot be created by a user.

#### Constructor

None

#### Properties

*callback* (function): Gets call state change callback function

*callStatus* (string): Gets call status

#### Functions
*acceptCall* (None) -> bool:
    Accept the call
    Returns True if call was accepted, False otherwise

*setCallback* (function) -> None: 
    Sets the state change callback function
    See setCallback documentation for how the callback function should be structured and works
    Returns None

## Linphone Python class

### Linphone

The linphone python class, situated in `linphone.py` is an implementation of the Linphone Python module. It lets you set a transcriber, a mqtt broker and automatically accepts calls and processes the audio when running.

#### Constructor

transcriber (TranscriberBaseModel): The transcriber to use, defaults to None
mqtt (MQTTClient): MQTT client to use, defaults to None
text_to_speech (TextToSpeech): TextToSpeech to use, defaults to None
configPath (str): Path to linphone config

#### Functions

*run* (None) -> None:
    Starts the core, and listening thread for linphone and makes it able to handle incoming calls.

## How to use it

Here is a simple intro on how to use it
1. import the library with `import linphone_module`
2. Create a core with `core = linphone_module.Core()`
    - You can either pass in a config path or choose to use the default config
3. Start the core with `core.start()`
    - It returns a boolean value if the core was able to start or not
4. Start iterating with  `core.iterate()`
    - This should be placed inside a while loop. The iterate function is what enables the core to accept calls
5. Check if calls have come in with `core.hasActiveCalls`
    - Incoming calls are stored in a FIFO-queue and the property is `True` when the queue is not empty.
6. Accept the calls with `call = core.getCall()`
    - This removes the first item in the FIFO-queue and returns a `Call object`. This object can only be created by the `Core`.
7. Add Callback to the call with `call.setCallback(callback)`
    - The callback function handles the state change of the calls. For example when the call is active and when it has ended.
8. Accept the call with `call.acceptCall()`
    - This opens the line of communication between the caller and the recipient.
9. When you are stopping the process, stop the core before exiting with `core.stop()`
    - This is done to ensure no data loss

## Audio out

To get the audio out we use pulseaudio and create a sink to capture the audio with.
- Create the sink with: `pactl load-module module-null-sink sink_name=LinphoneAudioOut sink_properties=device.description=Linphone_Audio_Out`.
- The sink with that description will be automatically loaded.