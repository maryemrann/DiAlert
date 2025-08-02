

from flask import Flask, request, jsonify
import joblib
import numpy as np
import pandas as pd

app = Flask(__name__)

# Load model and encoders
model = joblib.load("model.pkl")
encoders = joblib.load("encoders.pkl")

# Define the order of features expected
FEATURE_ORDER = [
    'gender',
    'age',
    'hypertension',
    'heart_disease',
    'smoking_history',
    'bmi',
    'HbA1c_level',
    'blood_glucose_level'
]

@app.route('/')
def home():
    return " Diabetes Prediction API is up and running!"

@app.route('/predict', methods=['POST'])
def predict():
    data = request.get_json()

    # Build a DataFrame from input
    input_df = pd.DataFrame([data], columns=FEATURE_ORDER)

    # Encode categorical columns using saved encoders
    for column in input_df.select_dtypes(include='object').columns:
        if column in encoders:
            input_df[column] = encoders[column].transform(input_df[column])

    # Predict using model
    prediction = model.predict(input_df)[0]

    return jsonify({'prediction': int(prediction)})

if __name__ == '__main__':
    app.run(debug=True)

