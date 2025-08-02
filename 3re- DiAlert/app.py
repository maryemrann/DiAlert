import pandas as pd

data = pd.read_csv("diabetes_prediction_dataset.csv")

print(data.head())            # First 5 rows
print(data.info())            # Info about columns and data types
print(data.describe())        # Summary statistics
print(data.isnull().sum())    # Check missing values
print(data.columns)          # List of columns