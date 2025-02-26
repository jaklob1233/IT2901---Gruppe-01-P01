# audio-samples

## Name
audio-samples

## Description
This project collects python code for executing audio experiments in IST-201. 
Further, it contains audio samples and tools for pre-processing these samples.

## Installation
create virtual environment (once)
python3 -m venv env

activate virtual environment (everytime WSL-Debian is restarted)
source env/bin/activate

## Further development

## Usage

## Corpora
### CRM
CRM (coordinated response measure) is a data set of audio samples which can be used for future experiments in the field, because samples are available in different languages.

### ITU
Small multilingual corpus (20 languages, 16 speakers per language, 1 file per speaker) from ITU (to be complemented with a more complete English database).

### ABC-MRT
abcmrt is the corpus for ABC-MRT16 - an objective measure of intelligibility (English corpus) as a complement to ESTOI (speech intelligibility), PESQ/POLQA (speech quality).
ABC-MRT16 does work with this corpus only and cannot be applied to other audio samples.

## Tools
### bin-to-wav-converter

bin-to-wav-converter is a java program that is used to convert bin files from CRM data set to WAV files.
BinToWav.java can be run directly in JVM.

    java BinToWav.java

### SpeechVoltmeter (sv56)

Sv56 from Software Tool Library (STL) from ITU for signal pre-processing can be used to normalise the speech level of the audio samples (i.e. the input of speech coder). Generally speech codecs are evaluated using three different levels corresponding to low, nominal and high levels (-36 dBov, -26 dBov, -16 dBov).
dBov (overload) is similar to dBfs (full scale) but estimated on a mean estimated value of speech envelop. The nominal level (-26dBov) ensures that there is no saturation of speech.
sv56demo.c can be run from CLion. Default value for level is -26 dBov. Running with parameter e.g. "-lev -36" or "-lev -16" will convert to low or high levels respectively.  

## Authors and acknowledgment
Norman JANSEN, Fraunhofer FKIE, 2024
