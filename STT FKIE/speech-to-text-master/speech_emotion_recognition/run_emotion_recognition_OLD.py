import os
import librosa
import torch
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from collections import defaultdict
from sklearn.metrics import confusion_matrix, accuracy_score, roc_curve, auc
from transformers import Wav2Vec2FeatureExtractor, Wav2Vec2ForSequenceClassification
from sklearn.preprocessing import label_binarize
from sklearn.metrics import classification_report, f1_score, precision_score, recall_score


# Load the model

# test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Wiam"
# weights_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/Wiam/pytorch_model.bin"
# model_path = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam"
weights_path = f"{model_path}/pytorch_model.bin"

feature_extractor = Wav2Vec2FeatureExtractor.from_pretrained(model_path)
model = Wav2Vec2ForSequenceClassification.from_pretrained(model_path, state_dict=torch.load(weights_path, map_location="cpu"))

# Emotion labels mapping
label_mapping = {
    '01': 'Neutral', '02': 'Calm', '03': 'Happy', '04': 'Sad',
    '05': 'Angry', '06': 'Fearful', '07': 'Disgust', '08': 'Surprised'
}

def predict_emotion(audio_file):
    """ Predicts the emotion and confidence scores for an audio file. """
    y, sr = librosa.load(audio_file, sr=16000, mono=True)
    inputs = feature_extractor(y, sampling_rate=16000, return_tensors="pt", padding=True)

    with torch.no_grad():
        logits = model(**inputs).logits

    probs = torch.nn.functional.softmax(logits, dim=-1)
    id2label = model.config.id2label

    # Ensure correct key formatting
    emotion_probs = {id2label[i].capitalize(): float(probs[0][i]) for i in range(len(id2label))}
    predicted_emotion = max(emotion_probs, key=emotion_probs.get)

    return predicted_emotion, emotion_probs


def evaluate_model(test_dir):
    """ Evaluates model performance and plots confidence distributions. """
    all_true_emotions = []
    all_predicted_emotions = []
    all_predicted_confidences = defaultdict(list)
    all_predicted_probs = []
    intensity_split = {'normal': {'correct': 0, 'total': 0}, 'strong': {'correct': 0, 'total': 0}}

    for file_name in os.listdir(test_dir):
        if file_name.endswith(".wav"):
            parts = file_name.split("-")
            true_emotion = parts[2]
            intensity = parts[3]  # Extract intensity (01 = normal, 02 = strong)

            audio_file = os.path.join(test_dir, file_name)
            predicted_emotion, emotion_probs = predict_emotion(audio_file)

            if true_emotion in label_mapping:
                true_label = label_mapping[true_emotion]
                all_true_emotions.append(true_label)
                all_predicted_emotions.append(predicted_emotion)

                category = 'strong' if intensity == '02' else 'normal'
                if label_mapping[true_emotion] == predicted_emotion.capitalize():
                    intensity_split[category]['correct'] += 1
                intensity_split[category]['total'] += 1

                # Store confidence scores
                if predicted_emotion in emotion_probs:
                    all_predicted_confidences[predicted_emotion].append(emotion_probs[predicted_emotion])
                else:
                    print(f"Warning: Missing confidence for '{predicted_emotion}' in {file_name}")

                # Collect probabilities for ROC curve
                all_predicted_probs.append([emotion_probs.get(label, 0) for label in label_mapping.values()])

    # Convert predicted probabilities to numpy array
    all_predicted_probs = np.array(all_predicted_probs)

    # Print confidence values for debugging
    print("\nPredicted Confidence Scores (Raw Values):")
    for emotion, scores in all_predicted_confidences.items():
        print(f"{emotion}: {scores}")

    # Calculate accuracy, precision, recall, and F1-score
    accuracy = accuracy_score(all_true_emotions, all_predicted_emotions)
    precision = precision_score(all_true_emotions, all_predicted_emotions, average='weighted')
    recall = recall_score(all_true_emotions, all_predicted_emotions, average='weighted')
    f1 = f1_score(all_true_emotions, all_predicted_emotions, average='weighted')

    print(f"\nModel Performance Metrics:")
    print(f"Accuracy: {accuracy * 100:.2f}%")
    print(f"Precision: {precision * 100:.2f}%")
    print(f"Recall: {recall * 100:.2f}%")
    print(f"F1-Score: {f1 * 100:.2f}%\n")

    # Generate classification report with dynamic labels
    unique_labels = sorted(set(all_true_emotions + all_predicted_emotions))
    label_names = [label for label in label_mapping.values() if label in unique_labels]

    print("Detailed Classification Report:")
    print(classification_report(all_true_emotions, all_predicted_emotions, target_names=label_names))

    # Call plotting functions
    plot_accuracy_overview(accuracy, intensity_split)
    plot_confusion_matrix(all_true_emotions, all_predicted_emotions)
    plot_average_confidence_per_emotion(all_predicted_confidences)

def plot_accuracy_overview(accuracy, intensity_split):
    categories = ['Overall', 'Normal Intensity', 'Strong Intensity']
    accuracies = [
        accuracy * 100,
        (intensity_split['normal']['correct'] / intensity_split['normal']['total']) * 100 if intensity_split['normal']['total'] > 0 else 0,
        (intensity_split['strong']['correct'] / intensity_split['strong']['total']) * 100 if intensity_split['strong']['total'] > 0 else 0
    ]
    
    plt.figure(figsize=(8, 5))
    plt.bar(categories, accuracies, color=['blue', 'green', 'red'])
    plt.ylim(0, 100)
    plt.ylabel('Accuracy (%)')
    plt.title('Model Accuracy Overview by Intensity')
    plt.show()

def plot_confusion_matrix(true_labels, predicted_labels):
    """ Plots a confusion matrix. """
    labels = list(label_mapping.values())
    cm = confusion_matrix(true_labels, predicted_labels, labels=labels)

    plt.figure(figsize=(8, 6))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', xticklabels=labels, yticklabels=labels)
    plt.xlabel('Predicted')
    plt.ylabel('True')
    plt.title('Confusion Matrix')
    plt.show()

def plot_average_confidence_per_emotion(all_predicted_confidences):
    """ Plots average model confidence for each predicted emotion. """
    emotions = list(all_predicted_confidences.keys())
    average_confidences = [np.mean(all_predicted_confidences[emotion]) if all_predicted_confidences[emotion] else 0 for emotion in emotions]

    # Print confidence values
    print("\nAverage Confidence Scores per Emotion:")
    for emotion, avg_conf in zip(emotions, average_confidences):
        print(f"{emotion}: {avg_conf:.4f}")

    plt.figure(figsize=(10, 6))
    plt.plot(emotions, average_confidences, marker='o', linestyle='-', color='b', label="Average Confidence")
    plt.title("Average Model Confidence for Each Emotion")
    plt.xlabel("Emotion")
    plt.ylabel("Average Confidence (Max Softmax Probability)")
    plt.xticks(rotation=45, ha='right')
    plt.grid(True)
    plt.legend()
    plt.show()

def plot_roc_curve(all_true_emotions, all_predicted_probs):
    """ Plots an ROC curve for emotion classification. """
    labels = list(label_mapping.values())
    y_true = label_binarize(all_true_emotions, classes=labels)

    plt.figure(figsize=(10, 6))

    for i, label in enumerate(labels):
        fpr, tpr, _ = roc_curve(y_true[:, i], all_predicted_probs[:, i])
        roc_auc = auc(fpr, tpr)
        plt.plot(fpr, tpr, label=f'{label} (AUC = {roc_auc:.2f})')

    plt.plot([0, 1], [0, 1], color='navy', linestyle='--')
    plt.xlabel('False Positive Rate')
    plt.ylabel('True Positive Rate')
    plt.title('Receiver Operating Characteristic (ROC) Curve')
    plt.legend(loc='best')
    plt.show()

# Run the evaluation
test_dir = "C:/Users/elina/Desktop/NTNU/Bachelor/Model/Wave2Vec-Ehcalabres-wiam/AudioSamples"
evaluate_model(test_dir)
