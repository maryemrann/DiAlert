import sys
import joblib
import pandas as pd

# Load model and encoders
try:
    model = joblib.load("model.pkl")
    encoders = joblib.load("encoders.pkl")
except Exception as e:
    print(f"error,Failed to load model or encoders: {e}")
    sys.exit(1)

FEATURE_ORDER = [
    'gender', 'age', 'hypertension', 'heart_disease',
    'smoking_history', 'bmi', 'HbA1c_level', 'blood_glucose_level'
]

gender_map = {'male': 'Male', 'female': 'Female', 'other': 'Other'}
smoking_map = {
    'formerly_smoked': 'former',
    'never_smoked': 'never',
    'smokes': 'current',
    'unknown': 'No Info'
}

def normalize(text):
    """Lowercase, strip, and replace spaces with underscores."""
    return text.strip().lower().replace(" ", "_")

def encode_features(df, encoders):
    for column in df.select_dtypes(include='object').columns:
        if column in encoders:
            df[column] = encoders[column].transform(df[column])
        else:
            print(f"error,No encoder for {column}")
            sys.exit(1)
    return df

try:
    gender_input = normalize(sys.argv[1])
    age = float(sys.argv[2])
    hypertension = int(sys.argv[3])
    heart_disease = int(sys.argv[4])
    smoking_input = normalize(sys.argv[5])
    bmi = float(sys.argv[6])
    hba1c = float(sys.argv[7])
    glucose = float(sys.argv[8])

    gender = gender_map[gender_input]
    smoking = smoking_map[smoking_input]

except (IndexError, ValueError, KeyError):
    print("error,Invalid input format or value")
    sys.exit(1)

user_data = {
    'gender': gender,
    'age': age,
    'hypertension': hypertension,
    'heart_disease': heart_disease,
    'smoking_history': smoking,
    'bmi': bmi,
    'HbA1c_level': hba1c,
    'blood_glucose_level': glucose
}

input_df = pd.DataFrame([user_data], columns=FEATURE_ORDER)
input_df = encode_features(input_df, encoders)

prediction = model.predict(input_df)[0]
probabilities = model.predict_proba(input_df)[0]
positive_prob = probabilities[1]
percent = positive_prob * 100

if percent < 30:
    risk_label = 'Low risk of Diabetes'
elif percent < 60:
    risk_label = 'Moderate risk of Diabetes'
else:
    risk_label = 'High risk of Diabetes'

# **Clean final output only**
print(f"{percent:.2f},{risk_label}")
sys.stdout.flush()
