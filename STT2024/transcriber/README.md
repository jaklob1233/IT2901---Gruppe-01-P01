# Transciber

Text-To-Speech generator using `pyttsx3`, and Voice-to-text transcription using `vosk` or `whisper`.

## Installation

In the source folder there is a script `install.sh` you can run to install all the needed dependencies. For offline support you also need to run the script `install_packages.sh`. This will install all needed models for both the Vosk, and the Whisper transcription services.

## How to use

### Text-to-speech

The TextToSpeech class is very easy to use. Simple instantiate an object, and use its speak() method for the class to use your systems default text-to-speech model to playback to your systems default audio output device.

#### Properties

*engine* -> pyttsx3.engine: The pyttsx3 engine
*rate* -> int: Speed the model speaks at. Defaults to 150.

#### Functions

*speak* (text) -> None:
  Uses the engine to playback the text in the parameter to your systems default output device
  Returns none

### Transcription

To use the transcriber, simply instantiate a Transcriber object with a string that specifies which model to use (see possible transcription models in config.ini)

#### Constructor

model (str): Name of transcription model to use.

#### Functions

*acceptData* (bytes) -> None:
  Stores the data in the chosen transcriber model
  
*getResults* (None) -> str:
  Gets the result from the transcriber model using the data stored in the transcriber

*clearData* (None) -> None:
  Empties the data in the transcriber model

## Basic usage


```python
import pyaudio
import wave
import sys


# Values for audio recording
chunk = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100
RECORD_SECONDS = 3

p = pyaudio.PyAudio()

stream = p.open(format = FORMAT,
    channels = CHANNELS,
    rate = RATE,
    input = True,
    frames_per_buffer = chunk)

# Record audio and store it in data
all = []
for i in range(0, RATE / chunk * RECORD_SECONDS):
    data = stream.read(chunk)
    all.append(data)
stream.close()
p.terminate()
data = ''.join(all)

transcriber = Transcriber("vosk")

transcriber.acceptData(data) # Accepts the data from the recording
transcribed_data = transcriber.getResults() # Returns the transcribed data
transcriber.clearData() # Empties the transcriber

tts = TextToSpeech()
tts.speak(transcribed_data)
```
