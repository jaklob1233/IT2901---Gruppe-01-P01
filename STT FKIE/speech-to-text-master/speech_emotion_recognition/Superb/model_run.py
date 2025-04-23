import psutil
import os
import time
import torch
import librosa
import re
from transformers import Wav2Vec2ForSequenceClassification, Wav2Vec2FeatureExtractor
from RealTime import realtime_dynamic_test


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



# Emotion labels mapping
label_mapping = {
    '01': 'Neutral', '03': 'Happy', '04': 'Sad', '05': 'Angry'
}

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
    """Loads the Wav2Vec2 model and feature extractor and times the loading process."""
    start_load_time = time.time()  # Start time for model loading

    feature_extractor = Wav2Vec2FeatureExtractor.from_pretrained(model_path)
    model = Wav2Vec2ForSequenceClassification.from_pretrained(model_path, state_dict=torch.load(f"{model_path}/pytorch_model.bin", map_location="cpu"))

    end_load_time = time.time()  # End time for model loading
    model_load_time = end_load_time - start_load_time  # Calculate model load time

    return feature_extractor, model, model_load_time


# Prediction function (detecting emotions)
def predict_vec_emotion(audio_file, feature_extractor, model):
    """Predicts the emotion and confidence scores for an audio file."""
    y, sr = librosa.load(audio_file, sr=16000, mono=True)

    start_pred_time = time.time()  # Start time for prediction

    inputs = feature_extractor(y, sampling_rate=16000, return_tensors="pt", padding=True)
    with torch.no_grad():
        logits = model(**inputs).logits

    end_pred_time = time.time()  # End time for prediction

    # Calculate time for this prediction
    prediction_time = end_pred_time - start_pred_time

    probs = torch.nn.functional.softmax(logits, dim=-1)
    id2label = model.config.id2label

    emotion_probs = {id2label[i].capitalize(): float(probs[0][i]) for i in range(len(id2label))}
    predicted_emotion = max(emotion_probs, key=emotion_probs.get)

    return predicted_emotion, emotion_probs, prediction_time


# Process all audio files in the test directory and collect results
def process_audio_files(model, feature_extractor, test_dir):
    """Process each audio file and return predictions and individual prediction times."""
    results = []
    total_prediction_time = 0
    num_files = 0

    # Loop through all audio files in the test directory
    for file_name in os.listdir(test_dir):
        if file_name.endswith(".wav") and hasEmotion(file_name):
            audio_file = os.path.join(test_dir, file_name)
            predicted_emotion, emotion_probs, prediction_time = realtime_dynamic_test(predict_vec_emotion,audio_file, feature_extractor, model, None)
            # predicted_emotion, emotion_probs, prediction_time = predict_vec_emotion(audio_file, feature_extractor, model)
            
 
            # Add the time for this prediction
            total_prediction_time += prediction_time
            num_files += 1

            results.append({
                "file_name": file_name,
                "predicted_emotion": predicted_emotion,
                "emotion_probs": emotion_probs,
                "prediction_time": prediction_time
            })

    # Calculate average prediction time per file
    avg_prediction_time = total_prediction_time / num_files if num_files > 0 else 0

    return results, avg_prediction_time


# Check if emotion exists in the file name (SAVEE and TESS dataset handling)
def hasEmotion(file_name):
    # Check for SAVEE dataset (expecting the pattern: DC_d02 or something similar)
    match = re.match(r"^[A-Z]{2}_(d|a|sa|h|n|f|su)\d{2}", file_name)
    if match:
        emotion_code = match.group(1)
        if savee_label_mapping.get(emotion_code, None) in label_mapping.values():
            return True
        else:
            print(f"Unknown emotion code in SAVEE dataset: {emotion_code}")
            return False

    # Check for TESS dataset (expecting the pattern: YAF_someword_emotion)
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
            print(f"Unknown emotion code in TESS dataset: {emotion_code}")
            return False

    return False

# Main function to run emotion recognition
def run_emotion_recognition(model_path, test_dir):
    feature_extractor, model, model_load_time = load_model(model_path)
    
    results, avg_prediction_time = process_audio_files(model, feature_extractor, test_dir)

    # Print results and times
    for result in results:
        print(f"File: {result['file_name']}")
        print(f"Predicted Emotion: {result['predicted_emotion']}")
        print(f"Confidence Scores: {result['emotion_probs']}")
        print(f"Prediction Time: {result['prediction_time']} seconds")
        print("-" * 40)

    print("Done")
    
    # Return results, model load time, and avg prediction time
    return results, avg_prediction_time, model_load_time

# Wrapper for model testing with resource monitoring
def run_models_with_monitoring(model_path, test_dir):

    print("Starting Emotion Recognition Test...")
    # Run the emotion recognition function with resource monitoring
    model_results, performance_metrics = monitor_resources(run_emotion_recognition, model_path, test_dir)

    # Return both model results and performance metrics separately
    return model_results, performance_metrics

if __name__ == "__main__":
    test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
    model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Superb"
    
    model_results, performance_metrics = run_models_with_monitoring(model_path, test_dir)

    # Print the results
    print("\nModel Performance Metrics:")
    print(performance_metrics)

    print("\nModel Results:")
    for result in model_results:
        print(result)

# def run_emotion_recognition(model_path, test_dir):
#     feature_extractor, model = load_model(model_path)
#     results = process_audio_files(model, feature_extractor, test_dir)

#     for result in results:
#         print(f"File: {result['file_name']}")
#         print(f"Predicted Emotion: {result['predicted_emotion']}")
#         print(f"Confidence Scores: {result['emotion_probs']}")
#         print("-" * 40)
#     print("Done")
#     return results

# if __name__ == "__main__":
#     test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
#     model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Superb"
#     _ = run_emotion_recognition(model_path, test_dir)