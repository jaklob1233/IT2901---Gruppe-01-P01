import whisper

model = whisper.load_model("tiny")
result = model.transcribe("test.wav")
print(result["text"])