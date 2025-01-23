import whisper

import wave
from vosk import Model, KaldiRecognizer, SetLogLevel

import re
from Levenshtein import distance as levenshtein_distance
import json
from time import time
import psutil
import functools


import numpy as np
import pandas as pd

import matplotlib.pyplot as plt
import seaborn as sns

SetLogLevel(0)

def preprocess(s):
    s = s.lower()
    s = re.sub(r'[^\w\s]', '', s)
    return s

def calculate_accuracy_and_error(output, correct):
    processed_output = preprocess(output)
    processed_correct = preprocess(correct)
    
    dist = levenshtein_distance(processed_output, processed_correct)
    
    accuracy = (1 - dist / max(len(processed_output), len(processed_correct))) * 100
    error_rate = 100 - accuracy
    
    return accuracy, error_rate

test_files = [f"84-121550-000{i}.wav" for i in range(10)]
test_correct = []

with open("libre_text_test.txt", "r+") as f:
    test_correct = f.readlines()

# create models
whisper_model_tiny = whisper.load_model("tiny")
whisper_model_base = whisper.load_model("base")
vosk_model_tiny = Model(lang="en-us")
vosk_model_base = Model("vosk_model_us_en_medium")

# test the models
iterations = 1

whisper_results_tiny = np.empty((iterations, len(test_files)), dtype=str)
whisper_results_base = np.empty((iterations, len(test_files)), dtype=str)
vosk_results_tiny = np.empty((iterations, len(test_files)), dtype=str)
vosk_results_base = np.empty((iterations, len(test_files)), dtype=str)
speed_tiny = np.empty((iterations, len(test_files), 2), dtype=float)
speed_base = np.empty((iterations, len(test_files), 2), dtype=float)

measurements = []

def profile_resource_usage(func):
    """
    Decorator that profiles CPU and RAM usage of the function.
    """
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # Get the process handle for the current process
        p = psutil.Process()

        # Measure CPU times and memory usage before function execution
        cpu_times_before = p.cpu_times()
        memory_before = p.memory_info().rss

        result = func(*args, **kwargs)  # Execute the function

        # Measure CPU times and memory usage after function execution
        cpu_times_after = p.cpu_times()
        memory_after = p.memory_info().rss

        # Calculate differences
        cpu_time_used = (cpu_times_after.user - cpu_times_before.user) + \
                        (cpu_times_after.system - cpu_times_before.system)
        memory_used = memory_after - memory_before

        # Append measurements to the array
        measurements.append({
            'function': func.__name__,
            'cpu_time': cpu_time_used, 
            'memory_used': memory_used
            })

        return result

    return wrapper

@profile_resource_usage
def whisper_tiny_transcribe(audio):
    return whisper_model_tiny.transcribe(audio)["text"]

@profile_resource_usage
def whisper_base_transcribe(audio):
    return whisper_model_base.transcribe(audio)["text"]

@profile_resource_usage
def vosk_tiny_transcribe(wf):
    rec_tiny = KaldiRecognizer(vosk_model_tiny, wf.getframerate())

    rec_tiny.SetWords(True)
    rec_tiny.SetPartialWords(True)

    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        rec_tiny.AcceptWaveform(data)

    return json.loads(rec_tiny.FinalResult())["text"]

@profile_resource_usage
def vosk_base_transcribe(wf):
    rec_base = KaldiRecognizer(vosk_model_base, wf.getframerate())

    rec_base.SetWords(True)
    rec_base.SetPartialWords(True)

    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        rec_base.AcceptWaveform(data)
    return json.loads(rec_base.FinalResult())["text"]

for i in range(iterations):
    for j, audio in enumerate(test_files):
        # tiny
        whisper_tiny_start_time = time()
        whisper_results_tiny[i,j] = whisper_tiny_transcribe(audio)
        whisper_tiny_end_time = time()
        
        # base
        whisper_base_start_time = time()
        whisper_results_base[i,j] = whisper_base_transcribe(audio)
        whisper_base_end_time = time()

        
        # tiny
        wf = wave.open(audio)
        vosk_tiny_start_time = time()
        vosk_results_tiny[i,j] = vosk_tiny_transcribe(wf)
        vosk_tiny_end_time = time()

        # base
        wf = wave.open(audio)
        vosk_base_start_time = time()
        vosk_results_base[i,j] = vosk_base_transcribe(wf)
        vosk_base_end_time = time()

        speed_tiny[i,j] = [whisper_tiny_end_time - whisper_tiny_start_time, vosk_tiny_end_time - vosk_tiny_start_time]
        speed_base[i,j] = [whisper_base_end_time - whisper_base_start_time, vosk_tiny_end_time - vosk_tiny_start_time]

        pass

print(measurements)
exit()

# correct
accuracy_tiny = []
error_tiny = []
accuracy_base = []
error_base = []

for i, (whisper_result, vosk_result) in enumerate(zip(whisper_results_tiny, vosk_results_tiny)):
    whisper_a, whisper_e = calculate_accuracy_and_error(whisper_result, test_correct[i])
    vosk_a, vosk_e = calculate_accuracy_and_error(vosk_result, test_correct[i])
    accuracy_tiny.append([whisper_a, vosk_a])
    error_tiny.append([whisper_e, vosk_e])

for i, (whisper_result, vosk_result) in enumerate(zip(whisper_results_base, vosk_results_base)):
    whisper_a, whisper_e = calculate_accuracy_and_error(whisper_result, test_correct[i])
    vosk_a, vosk_e = calculate_accuracy_and_error(vosk_result, test_correct[i])
    accuracy_base.append([whisper_a, vosk_a])
    error_base.append([whisper_e, vosk_e])

accuracy_tiny = np.array(accuracy_tiny)
error_tiny = np.array(error_tiny)
speed_tiny = np.array(speed_tiny)

accuracy_base = np.array(accuracy_base)
error_base = np.array(error_base)
speed_base = np.array(speed_base)

df_accuracy_tiny = pd.DataFrame(accuracy_tiny, columns=["Whisper", "Vosk"])
df_error_tiny = pd.DataFrame(error_tiny, columns=["Whisper", "Vosk"])
df_speed_tiny = pd.DataFrame(speed_tiny, columns=["Whisper", "Vosk"])

df_accuracy_base = pd.DataFrame(accuracy_base, columns=["Whisper", "Vosk"])
df_error_base = pd.DataFrame(error_base, columns=["Whisper", "Vosk"])
df_speed_base = pd.DataFrame(speed_base, columns=["Whisper", "Vosk"])

fig = plt.figure(constrained_layout=True, figsize=(15, 10))
fig.suptitle("Test Whisper VS Vosk")

subfigs = fig.subfigures(nrows=2, ncols=1)

subfigs[0].suptitle("Tiny model")
tiny_axs = subfigs[0].subplots(nrows=1, ncols=3)
tiny_axs[0].set_title("Accuracy")
tiny_axs[0].set_xlabel("Test")
tiny_axs[0].set_ylabel("%")
sns.lineplot(data=df_accuracy_tiny.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=tiny_axs[0])
tiny_axs[1].set_title("Error rate")
tiny_axs[1].set_xlabel("Test")
tiny_axs[1].set_ylabel("%")
sns.lineplot(data=df_error_tiny.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=tiny_axs[1])
tiny_axs[2].set_title("Speed")
tiny_axs[2].set_xlabel("Test")
tiny_axs[2].set_ylabel("Time [s]")
sns.lineplot(data=df_speed_tiny.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=tiny_axs[2])

subfigs[1].suptitle("Base model")
base_axs = subfigs[1].subplots(nrows=1, ncols=3)
base_axs[0].set_title("Accuracy")
base_axs[0].set_xlabel("Test")
base_axs[0].set_ylabel("%")
sns.lineplot(data=df_accuracy_base.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=base_axs[0])
base_axs[1].set_title("Error rate")
base_axs[1].set_xlabel("Test")
base_axs[1].set_ylabel("%")
sns.lineplot(data=df_error_base.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=base_axs[1])
base_axs[2].set_title("Speed")
base_axs[2].set_xlabel("Test")
base_axs[2].set_ylabel("Time [s]")
sns.lineplot(data=df_speed_base.reset_index().melt(id_vars=['index'], value_vars=['Whisper', 'Vosk']), x="index", y="value", hue="variable", ax=base_axs[2])

fig.savefig("output2.png")
# print(f"whisper: {whisper_mean[0]}, {whisper_mean[1]}")
# print(f"vosk: {vosk_accuracy}, {vosk_error_rate}")