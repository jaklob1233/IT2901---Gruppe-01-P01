import wave
from vosk import Model, KaldiRecognizer

model = Model("vosk_model_us_en_medium")

wf = wave.open("84-121550-0000.wav")

rec = KaldiRecognizer(model, wf.getframerate())

rec.SetWords(True)
rec.SetPartialWords(True)

while True:
    data = wf.readframes(4000)
    if len(data) == 0:
        break
    rec.AcceptWaveform(data)

print(rec.FinalResult())
