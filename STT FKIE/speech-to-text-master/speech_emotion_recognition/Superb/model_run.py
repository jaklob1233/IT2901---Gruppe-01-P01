import psutil
import os
import time
import torch
import librosa
import re
from transformers import Wav2Vec2ForSequenceClassification, Wav2Vec2FeatureExtractor
from RealTime import realtime_dynamic_test


# Load model function
def load_model(model_path):
    """Loads the Wav2Vec2 model and feature extractor and times the loading process."""
    start_load_time = time.time()  # Start time for model loading

    feature_extractor = Wav2Vec2FeatureExtractor.from_pretrained(model_path)
    model = Wav2Vec2ForSequenceClassification.from_pretrained(model_path, state_dict=torch.load(f"{model_path}/pytorch_model.bin", map_location="cpu"))

    end_load_time = time.time()  # End time for model loading
    model_load_time = end_load_time - start_load_time  # Calculate model load time

    return feature_extractor, model, model_load_time


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

# Main function to run emotion recognition
def run_emotion_recognition(model_path, test_dir):
    feature_extractor, model, model_load_time = load_model(model_path)
    
    results, avg_prediction_time = process_audio_files(model, feature_extractor, test_dir)
    
    # Return results, model load time, and avg prediction time
    return results, avg_prediction_time, model_load_time

if __name__ == "__main__":
    # test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
    # model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Superb"
    
    model_results, performance_metrics = run_models_with_monitoring(model_path, test_dir)

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