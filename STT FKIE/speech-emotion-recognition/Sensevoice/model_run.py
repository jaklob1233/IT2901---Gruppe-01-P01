import os
import re
import time
import psutil
import torchaudio
from Sensevoice.model import SenseVoiceSmall
from RealTime import realtime_dynamic_test

# Emotion mappings
tess_label_mapping = {
    "angry": "Angry",
    "disgust": "Disgust",
    "fear": "Fearful",
    "happy": "Happy",
    "sad": "Sad",
    "ps": "Surprised",
    "neutral": "Neutral"
}

savee_label_mapping = {
    "a": "Angry",
    "d": "Disgust",
    "f": "Fearful",
    "h": "Happy",
    "sa": "Sad",
    "su": "Surprised",
    "n": "Neutral"
}

CREMA_label_mapping = {
    "ANG": "Angry",
    "DIS": "Disgust",
    "FEA": "Fearful",
    "HAP": "Happy",
    "SAD": "Sad",
    "NEU": "Neutral"
}


label_mapping = {
    '01': 'Neutral', '02': 'Happy', '04': 'Sad', '05': 'Angry', '06': 'Fearful', '07': 'Disgust', '08': 'Surprised'
}

TARGET_SAMPLE_RATE = 16000  # Ensure all audio files match this sample rate


def monitor_resources(func, *args, **kwargs):
    # Get the current process
    pid = os.getpid()
    process = psutil.Process(pid)

    # Measure initial resource usage
    cpu_before = process.cpu_percent(interval=1.0)  # Measure CPU usage before
    memory_before = process.memory_info().rss / (1024 ** 2)  # MB (initial memory usage)

    # Sleep for a brief moment for accurate CPU reading
    time.sleep(1)  # Increased sleep time for better accuracy

    # Record start time for overall execution
    start_time = time.time()

    # Run the function to get model results and performance metrics
    model_results, avg_prediction_time, model_load_time = func(*args, **kwargs)

    # Measure resources after the function execution
    time.sleep(1)  # Optional: gives a bit more time for accurate CPU readings
    cpu_after = process.cpu_percent(interval=1.0)  # Measure CPU usage after function
    memory_after = process.memory_info().rss / (1024 ** 2)  # MB (final memory usage)
    end_time = time.time()  # End time for overall execution

    # Calculate the total execution time, CPU usage, and memory usage
    execution_time = end_time - start_time
    cpu_usage = max(0, cpu_after - cpu_before)  # Ensure CPU usage is not negative
    memory_usage = memory_after - memory_before

    # Prepare the performance metrics dictionary
    performance_metrics = {
        "execution_time": execution_time,
        "cpu_usage": cpu_usage,
        "memory_usage": memory_usage,
        "average_prediction_time": avg_prediction_time,  # Include the average time per prediction
        "model_load_time": model_load_time  # Include model load time
    }

    return model_results, performance_metrics

# Load model function
def load_model(model_path):
    """Load the SenseVoiceSmall model."""
    start_load_time = time.time()
    model, kwargs = SenseVoiceSmall.from_pretrained(model=f"{model_path}/model/models/iic/SenseVoiceSmall", device="cuda:0")
    model.eval()  # Set model to evaluation mode
    end_load_time = time.time()

    model_load_time = end_load_time - start_load_time  # Model load time
    return model, kwargs, model_load_time

# Predict emotion function
def predict_sense_emotion(audio_file, model, kwargs):
    """Predict the emotion of a given audio file."""
    waveform, sample_rate = torchaudio.load(audio_file)

    # Resample if necessary
    if sample_rate != TARGET_SAMPLE_RATE:
        resampler = torchaudio.transforms.Resample(orig_freq=sample_rate, new_freq=TARGET_SAMPLE_RATE)
        waveform = resampler(waveform)
    start_pred_time = time.time()  # Start time for prediction

    # Perform inference
    res, _ = model.inference(
        data_in=waveform,
        language="auto",  # Adjust language if needed
        use_itn=False,
        ban_emo_unk=True,
        **kwargs,
    )
    end_pred_time = time.time()  # End time for prediction

    prediction_time = end_pred_time - start_pred_time

    emotion_probs = dict(res[0]["possible_emotions"])
    dominant_emotion = max(emotion_probs, key=emotion_probs.get)

    return dominant_emotion, emotion_probs, prediction_time

# Process all audio files in the directory
def process_audio_files(model, kwargs, test_dir):
    results = []
    total_prediction_time = 0
    num_files = 0

    # Loop through all audio files in the test directory
    for file_name in os.listdir(test_dir):
        if file_name.endswith(".wav") and hasEmotion(file_name):  # Only process valid emotion files
            audio_file = os.path.join(test_dir, file_name)

            # Measure prediction time
            start_pred_time = time.time()
            # predicted_emotion, emotion_probs, prediction_time = predict_sense_emotion( audio_file, model, kwargs)
            predicted_emotion, emotion_probs, prediction_time = realtime_dynamic_test(predict_sense_emotion, audio_file , None, model, kwargs)

            # predicted_emotion, emotion_probs, prediction_time = predict_sense_emotion(audio_file, model, kwargs)
            end_pred_time = time.time()
            
            prediction_time = end_pred_time - start_pred_time
            total_prediction_time += prediction_time
            num_files += 1
            print(predicted_emotion)

            results.append({
                "file_name": file_name,
                "predicted_emotion": predicted_emotion,
                "emotion_probs": emotion_probs,
                "prediction_time": prediction_time,
            })

    avg_prediction_time = total_prediction_time / num_files if num_files > 0 else 0
    return results, avg_prediction_time

# Check if emotion exists in the file name (SAVEE and TESS dataset handling)
def hasEmotion(file_name):
    match = re.match(r"^[A-Z]{2}_(d|a|sa|h|n|f|su)\d{2}", file_name)
    if match:
        emotion_code = match.group(1)
        if savee_label_mapping.get(emotion_code, None) in label_mapping.values():
            return True
        else:
            print(f"Unknown emotion code in SAVEE dataset: {emotion_code}")
            return False

    match = re.match(r"^(YAF|OAF)_[a-zA-Z]+_([a-z]+)\.wav$", file_name)
    if match:
        emotion_code = match.group(2)
        if tess_label_mapping.get(emotion_code, None) in label_mapping.values():
            return True
        else:
            print(f"Unknown emotion code in TESS dataset: {emotion_code}")
            return False
        
        # Check for CREMA dataset (expecting the pattern: YAF_someword_emotion)
    match = re.match(r"^[a-zA-Z0-9]+_[a-zA-Z0-9]+_([a-zA-Z]{3})_[a-zA-Z0-9]+\.wav$", file_name)
    if match:
        emotion_code = match.group(1)
        if CREMA_label_mapping.get(emotion_code, None) in label_mapping.values():
            return True
        else:
            print(f"Unknown emotion code in CREMA dataset: {emotion_code}")
            return False

    return False

    return False

# Run the emotion recognition and collect results
def run_emotion_recognition(model_path, test_dir):
    model, kwargs, model_load_time = load_model(model_path)
    results, avg_prediction_time = process_audio_files(model, kwargs, test_dir)

    for result in results:
        print(f"File: {result['file_name']}")
        print(f"Predicted Emotion: {result['predicted_emotion']}")
        print(f"Confidence Scores: {result['emotion_probs']}")
        print(f"Prediction Time: {result['prediction_time']} seconds")
        print("-" * 40)

    print("Done")
    return results, avg_prediction_time, model_load_time

# Wrapper function with resource monitoring
def run_models_with_monitoring(model_path, test_dir):
    print("Starting Emotion Recognition Test...")
    model_results, performance_metrics = monitor_resources(run_emotion_recognition, model_path, test_dir)

    # Return both model results and performance metrics separately
    return model_results, performance_metrics

if __name__ == "__main__":
    test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
    model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Sensevoice"
    
    model_results, performance_metrics = run_models_with_monitoring(model_path, test_dir)

    # Print the results
    print("\nModel Performance Metrics:")
    print(performance_metrics)

    print("\nModel Results:")
    for result in model_results:
        print(result)
