#!/usr/bin/env bash
# download models
[ -d models ] || mkdir models
[ -d models/vosk_english_medium ] || mkdir models/vosk_english_medium
[ -d models/whisper_english_base ] || mkdir models/whisper_english_base
[ -d models/whisper_english_tiny ] || mkdir models/whisper_english_tiny

# vosk
curl https://alphacephei.com/vosk/models/vosk-model-en-us-0.22-lgraph.zip -o vosk_medium.zip
vosk_model_name=$(unzip -qql vosk_medium.zip | head -n1 | tr -s ' ' | cut -d' ' -f5-)
unzip vosk_medium.zip
cp -a $vosk_model_name/. models/vosk_english_medium
rm -rf $vosk_model_name
rm vosk_medium.zip

# whisper
curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/vocab.bpe -o models/whisper_english_base/vocab.pbe
curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/vocab.bpe -o models/whisper_english_tiny/vocab.pbe
curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/encoder.json -o models/whisper_english_base/encoder.json
curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/encoder.json -o models/whisper_english_tiny/encoder.json
curl https://openaipublic.azureedge.net/main/whisper/models/d3dd57d32accea0b295c96e26691aa14d8822fac7d9d27d5dc00b4ca2826dd03/tiny.en.pt -o models/whisper_english_tiny/tiny.en.pt
curl https://openaipublic.azureedge.net/main/whisper/models/65147644a518d12f04e32d6f3b26facc3f8dd46e5390956a9424a650c0ce22b9/tiny.pt -o models/whisper_english_tiny/tiny.pt
curl https://openaipublic.azureedge.net/main/whisper/models/25a8566e1d0c1e2231d1c762132cd20e0f96a85d16145c3a00adf5d1ac670ead/base.en.pt -o models/whisper_english_base/base.en.pt
curl https://openaipublic.azureedge.net/main/whisper/models/ed3a0b6b1c0edf879ad9b11b1af5a0e6ab5db9205f891f668f8b0e6c6326e34e/base.pt -o models/whisper_english_base/base.pt
