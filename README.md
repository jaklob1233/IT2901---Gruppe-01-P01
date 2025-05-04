# Speech-to-text with emotions
## Overview
This project extends the existing speech-to-text (STT) communication system developed for the Norwegian Defence Research Establishment (FFI), in collaboration with Fraunhofer Institute for Communication, Information Processing and Ergonomics FKIE, by integrating Speech Emotion Recognition (SER) capabilities. The enhanced system transcribes spoken audio into text and simultaneously detects the speaker's emotional state, transmitting both to the recipient. This provides richer context in text-based communication, particularly valuable in operational and military settings where emotional nuance can impact decision-making and prioritization.

**Note:** All changes and new functionality are implemented solely within the `speech-to-text-master` component. No modifications have been made to the other system components (`mqtt-transport-master`, `linphone-adapter-master`, `text-to-speech-master`). Each of these has its own README for setup and usage instructions.

## System architecture
The system is organized into four main components:

 - **Speech to text:** Handles both the transcription of audio to text and the detection of emotions from speech.

 - **MQTT transport:** Manages message transport using the MQTT protocol.

 - **Linphone adapter:** Interfaces with Linphone for audio capture and VoIP functionality.

 - **Text to speech:** Converts received text (and optionally emotion) back into speech.

Each component runs in its own Docker container and communicates via defined interfaces and webhooks. For more details on running and configuring each part, refer to their respective READMEs in their respective directories.

## Speech emotion recognition integration
### Motivation
Previously, the STT system enabled efficient, bandwidth-saving communication by sending transcribed text instead of audio. However, this approach omitted the emotional cues present in speech, which are critical for understanding urgency, stress, or intent-especially in military operations. The integration of SER addresses this gap by appending emotional metadata to each transcribed message, allowing recipients to better interpret the sender’s state and respond appropriately

### Scope of work
 - **System and component integration:** \
The integration of SER is strictly limited to the speech-to-text-master directory. No changes have been made to the MQTT transport, Linphone adapter, or Text to speech modules.

 - **SER model implementation:**\
Two SER models have been integrated:

    - **SenseVoice-Small:** Recognizes seven emotions (anger, disgust, sadness, fear, happiness, surprise, neutral) and is optimized for real-time, offline inference.

    - **Superb (Wav2Vec2-based):** Recognizes four emotions (angry, sad, happy, neutral) and is included for comparison and flexibility.

 - **Modular design:** \
Both models are implemented as subclasses of a common abstraction, allowing easy switching and future extension. Model selection is configurable.

 - **Concurrent processing:** \
The system processes audio in real time, running transcription and emotion detection in parallel for minimal latency.

## Functional overview
- **Input:** \
Audio is captured via Linphone and forwarded to the Speech to text module.

- **Processing:**
    - Audio is divided into manageable chunks.

    - Each chunk is transcribed to text (using Vosk or Whisper).

    - SER models (Supero or SenseVoice) analyze the same chunk for emotional content.

    - Results (text, emotion label, and confidence score) are bundled.

- **Output:** \
The combined transcription and emotion data are published via MQTT for delivery to recipients.

- **Reception:** \
On the receiving side, messages can be played back using text to speech, including the detected emotion.

## Key features
 - **Offline, real-Time emotion recognition:** \
All SER processing is performed locally, ensuring operational security and independence from cloud services.

 - **Multi-Model support:** \
The system supports both SenseVoice and Superb, allowing for flexibility in deployment and future fine-tuning by FFI.

 - **Configurable and extensible:** \
The architecture allows easy integration of additional SER models or replacement of existing ones.

 - **Minimal system impact:** \
The SER integration is designed to run efficiently on standard laptops (≥16GB RAM, Intel i5 14th gen or equivalent), with low CPU and memory overhead.

## Usage and integration
 - **Running the system:** \
Please refer to the README files in each component directory for detailed setup, configuration, and usage instructions.

 - **Configuration:** \
The SER model to be used is specified in the configuration file within `speech-to-text-master`. The system can be switched between SenseVoice and Superb as needed.

 - **Testing:** \
The system has been tested with standard emotion datasets (TESS, SAVEE, CREMA-D) for accuracy, confidence, and performance. See the project report for detailed evaluation results.

## Limitations and future work
 - **Model limitations:** 

    - Accuracy may decrease in highly noisy environments or with non-English speech (though SenseVoice is designed for multilingual support).

    - Some emotions (e.g., calm, surprise) may be underrepresented depending on the model and dataset.

 - **Further fine-tuning:** \
The current models are suitable for prototyping and demonstration. For deployment in operational environments, further fine-tuning with domain-specific data is recommended.

## Contact
This project is developed for FFI in collaboration with FKIE and NATO IST-201. For questions, further integration, or model fine-tuning, please contact the project team or FFI representatives.

## Acknowledgments
 - Norwegian Defence Research Establishment (FFI)
 - Fraunhofer FKIE and NATO IST-201 research group
 - Open source contributors to SenseVoice, Wav2Vec2, and related libraries

## Contributors
- [Ådne Aspheim](https://github.com/AadneAspheim)
- [Ådne Børresen](https://github.com/adneborresen)
- [Sepanta Jamshid Ganjei](https://github.com/sepantaganjei)
- [Adrian Haugen](https://github.com/AdrianDHaugen)
- [Gustav Hovde](https://github.com/G-dubbbz)
- [Jakob Kleiven Meltzer](https://github.com/jaklob1233)
- [Elin Arna Sigthorsdottir](https://github.com/ElinArnaSig)