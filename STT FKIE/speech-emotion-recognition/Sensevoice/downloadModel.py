import os
import torchaudio
from funasr import AutoModel
from funasr.utils.postprocess_utils import rich_transcription_postprocess

# Set the cache directory where the model will be downloaded and stored
os.environ["MODELSCOPE_CACHE"] = "/home/dev/IT2901-Gruppe-01/IT2901---Gruppe-01-P01/STT FKIE/speech-emotion-recognition/Sensevoicemodel"

# Define model to download
model_name = "iic/SenseVoiceSmall"

# Download and save the model
model = AutoModel(
    model=model_name,
    vad_model="iic/speech_fsmn_vad_zh-cn-16k-common-pytorch",
    vad_kwargs={"max_single_segment_time": 30000},
    trust_remote_code=True
)

# # Path to your input audio file
audio_path = "/home/dev/IT2901-Gruppe-01/IT2901---Gruppe-01-P01/STT FKIE/speech-to-text-master/test/samples/A_eng_f1.wav"

# # Load the audio file
waveform, sample_rate = torchaudio.load(audio_path)

# # Ensure correct sample rate
# target_sample_rate = 16000
# if sample_rate != target_sample_rate:
#     waveform = torchaudio.transforms.Resample(sample_rate, target_sample_rate)(waveform)

# Run inference
res = model.generate(
    input=audio_path,
    cache={},
    language="auto",
    use_itn=True,
    batch_size_s=60,
    merge_vad=True,
    merge_length_s=15,
)

# Post-process and print the output
if "text" in res[0]:
    text = rich_transcription_postprocess(res[0]["text"])
    print(text)
else:
    print("Error: 'text' key is missing in the model output.")
    print("Full response:", res)



# import re
# import torchaudio
# from funasr import AutoModel
# from model import SenseVoiceSmall
# from funasr.utils.postprocess_utils import rich_transcription_postprocess

# # Define local model paths
# local_model_path = "/home/dev/IT2901-Gruppe-01/IT2901---Gruppe-01-P01/STT FKIE/speech-emotion-recognition/Sensevoicemodel/models/iic/SenseVoiceSmall"
# local_vad_model_path = "/home/dev/IT2901-Gruppe-01/IT2901---Gruppe-01-P01/STT FKIE/speech-emotion-recognition/Sensevoicemodel/Sensevoicemodel/models/iic/speech_fsmn_vad_zh-cn-16k-common-pytorch"

# m, kwargs = SenseVoiceSmall.from_pretrained(model=local_model_path, device="cuda:0")
# m.eval()


# # Path to your input audio file
# # audio_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples/03-01-08-02-02-02-06.wav"
# audio_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples/SAVEE/ALL/JK_d05.wav"
# # audio_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples/AudioSamples/hehe-quie-funny-that-101soundboards.mp3"


# # Load the audio file
# waveform, sample_rate = torchaudio.load(audio_path)


# # Ensure correct sample rate
# target_sample_rate = 16000
# if sample_rate != target_sample_rate:
#     print(f"Original Sample Rate: {sample_rate}, Resampling to {target_sample_rate}...")
#     resampler = torchaudio.transforms.Resample(orig_freq=sample_rate, new_freq=target_sample_rate)
#     waveform = resampler(waveform)
#     sample_rate = target_sample_rate  # Update sample rate after resampling
# print(waveform, sample_rate)


# res, meta_data = m.inference(
#     data_in=waveform,
#     language="auto", # "zh", "en", "yue", "ja", "ko", "nospeech"
#     use_itn=False,
#     ban_emo_unk=True,
#     **kwargs,
# )

# print(res)
# print(meta_data)