# Vosk and Whisper benchmarking

How to set it up
1. Install the required libraries in requirements.txt
2. Download whisper tiny and base models from [here](https://github.com/openai/whisper/discussions/1463) and add them to this path:
	- Linux: ~/.cache/whisper/
	- Windows: C:\Users[Username]\.cache\whisper
3. Download Vosk Medium LGraph model from [here](https://alphacephei.com/vosk/models) and extract it to a folder called `vosk_model_us_en_medium`.
4. Run the benchmarks with `python model\ test/main_test.py`
