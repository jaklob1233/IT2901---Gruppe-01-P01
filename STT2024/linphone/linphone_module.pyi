from typing import overload


class Call:
    """
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
    """

    @property
    def callback(self) -> function:
        """
        Get callback function

        Setter:
            None

        Getter:
            function: Call state change Callback function
        """

    @property
    def callStatus(self) -> str:
        """
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
        """

    def acceptCall(self) -> bool:
        """
        Accept a call

        Parameters:
            None

        Returns:
            bool: True if call was accepted, False otherwise
        """

    def setCallback(self, callback_func: function) -> None:
        """
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
        """

class Core:
    """
    The Core class is a wrapper for the Linphone::Core object in its C++ library. The Core is limited in capability and it only has implemented logic for calls and sound sources.

        Constructor:
            config_path (str): Path to config file. Optional
        
        Properties:
            hasActiveCalls -> bool: Check if there are any active calls
            activeCallCount -> int: Count of active calls
            soundSources -> list[str]: List of all sound sources
            sipAddress -> str: Cores Sip Address
            on -> bool: If the core is turned on or not

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
            loadConfigFromXML (str) -> bool:
                Loads Linphone config from XML string
                Returns True if successful, False otherwise.
            getConfigAsXML (None) -> str:
                Gets Linphone config as XML
                Returns XML config as string

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
    """

    @overload
    def __init__(self) -> None:
        """
        Creates a Core object without a config.

        Parameters:
            None

        Returns:
            A new Core object.
        """
        
    @overload
    def __init__(self, config_path: str) -> None:
        """
        Creates a Core object with a config

        Parameters:
            config_path (str): path to config - optional

        Returns:
            A new Core Object
        """

    @property
    def hasActiveCalls(self) -> bool:
        """
        Check if Core has active calls.

        Setter:
            None

        Getter:
            bool: True if there is any active calls, False otherwise.
        """

    @property
    def activeCallCount(self) -> int:
        """
        Get active call count.

        Setter:
            None

        Getter:
            int: Count of the active calls.
        """

    @property
    def soundSources(self) -> list[str]:
        """
        Get all sound sources the Core has access to.

        Setter:
            None

        Getter:
            list[str]: list of all sound sources
        """

    @property
    def sipAddress(self) -> str:
        """
        Get or set the sipAddress
            
        Setter:
            address (string): SIP address to use

        Getter:
            string: current SIP address of Core
        """

    @property
    def on(self) -> bool:
        """
        Check if Core is on or not

        Setter:
            None

        Getter:
            bool: True if Core is on, False otherwise
        """

    def start(self) -> bool:
        """
        Start the core

        Parameters:
            None
        
        Returns:
            bool: True if Core started, False otherwise.
        """

    def stop(self) -> bool:
        """
        Stop the core

        Parameters:
            None
        
        Returns:
            bool: True if Core stopped, False otherwise
        """

    def run(self) -> None:
        """
        Start thread for listening after incoming calls.
            
        Parameters:
            None
        
        Returns:
            None
        """
    def stopRun(self) -> None:
        """
        Stops thread listening after incoming calls.

        Parameters:
            None
        
        Returns:
            None
        """
    def getCall(self) -> Call:
        """
        Get first active call

        Parameters:
            None

        Returns:
            Call: First active Call object
        """
    def setSoundSource(self, sound_source: str) -> None:
        """
        Set sound source
            
        Parameters:
            sound_source (str): sound source to set active. Use soundSources property to get list of sound sources
        
        Returns:
            None
        """

    def loadConfigFromXML(self, config_path: str) -> bool:
        """
        Loads Linphone config from XML string

        Parameters:
            config_string (str): XML config string

        Returns:
            bool: True if load was successful, False otherwise
        """

    def getConfigAsXML(self) -> str:
        """
        Gets config as XML

        Parameters:
            None
        
        Returns:
            str: Config as xml string
        """