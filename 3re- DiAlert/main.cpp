#include <iostream>     // For input/output (cin, cout)
#include <string>       // To work with strings
#include <sstream>      // To convert between strings and numbers
#include <algorithm>    // For functions like transform, find
#include <cctype>       // For checking character types (like isdigit)
#include <vector>       // For using dynamic arrays (lists)
#include <memory>       // For smart pointers (used in system command)
#include <stdexcept>    // For throwing and catching errors
#include <array>        // For fixed-size arrays (used in system output)
#include <fstream>      // To write to files (like result.txt)
#include <filesystem>   // To handle file paths (for opening HTML)
using namespace std;

// Base class for Input reading and validation

class InputReader {
protected:
    string trim(const string& s) const {
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        if (start == string::npos || end == string::npos) return "";
        return s.substr(start, end - start + 1);
    }

    virtual bool validate(const string& input) const = 0;
    virtual void printPrompt() const = 0;

public:
    string read() {
        string input;
        while (true) {
            printPrompt();
            getline(cin, input);
            input = trim(input);
            if (validate(input)) return input;
            cout << "Invalid input. Please try again.\n";
        }
    }
};

class FloatReader : public InputReader {
protected:
    bool validate(const string& s) const override {
        istringstream iss(s);
        float f;
        iss >> noskipws >> f;
        return iss.eof() && !iss.fail();
    }
    void printPrompt() const override {
        cout << prompt;
    }

private:
    string prompt;

public:
    FloatReader(const string& p) : prompt(p) {}

    float readFloat() {
        string s = read();
        return stof(s);
    }
};

class BinaryReader : public InputReader {
protected:
    bool validate(const string& s) const override {
        return (s == "0" || s == "1");
    }
    void printPrompt() const override {
        cout << prompt;
    }

private:
    string prompt;

public:
    BinaryReader(const string& p) : prompt(p) {}

    int readBinary() {
        string s = read();
        return stoi(s);
    }
};

class GenderReader : public InputReader {
protected:
    bool validate(const string& s) const override {
        string lower = s;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        vector<string> valid = {"male", "female", "other"};
        return find(valid.begin(), valid.end(), lower) != valid.end();
    }
    void printPrompt() const override {
        cout << "Enter gender (male/female/other): ";
    }

public:
    string readGender() {
        string s = read();
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
};

class SmokingStatusReader : public InputReader {
protected:
    bool validate(const string& s) const override {
        string lower = s;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        vector<string> valid = {
            "formerly smoked",
            "never smoked",
            "smokes",
            "unknown"
        };
        return find(valid.begin(), valid.end(), lower) != valid.end();
    }
    void printPrompt() const override {
        cout << "Enter smoking status (formerly smoked/never smoked/smokes/unknown): ";
    }

public:
    string readSmokingStatus() {
        string s = read();
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
};

class PredictionRunner {
protected:
    string exec(const char* cmd) const {
        array<char, 128> buffer;
        string result;
        unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw runtime_error("popen() failed!");
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

public:
    virtual ~PredictionRunner() = default;

    virtual string runPrediction(const string& cmd) const {
        return exec(cmd.c_str());
    }

    virtual pair<string, string> parseOutput(const string& output) const {
        size_t comma_pos = output.find(',');
        if (comma_pos == string::npos)
            throw runtime_error("Invalid prediction output format");

        string percent = output.substr(0, comma_pos);
        string label = output.substr(comma_pos + 1);
        label.erase(label.find_last_not_of(" \n\r\t") + 1);
        return {percent, label};
    }
};

class DiabetesPrediction : public PredictionRunner {
public:
    string getRecommendations(const string& label, int hypertension, int heart_disease, float bmi, float glucose, float hba1c) const {
        stringstream recommendations;
        string lowerLabel = label;
        transform(lowerLabel.begin(), lowerLabel.end(), lowerLabel.begin(), ::tolower);

        if (lowerLabel.find("high") != string::npos) {
            recommendations << "- High risk detected. Please consult an endocrinologist or primary care doctor soon.\n";
            recommendations << "- Prioritize a low-carb, high-fiber diet and avoid sugary foods.\n";
            recommendations << "- Engage in daily physical activity (walking, yoga, or cardio).\n";
            recommendations << "- Monitor your glucose levels regularly and track symptoms.\n";
            recommendations << "- Manage stress and get adequate sleep (7-9 hours).\n";
        } else if (lowerLabel.find("moderate") != string::npos) {
            recommendations << "- Moderate risk detected. This is a good time to take preventive action.\n";
            recommendations << "- Make dietary changes: reduce intake of processed sugars and high-GI foods.\n";
            recommendations << "- Aim for at least 150 minutes of moderate exercise weekly.\n";
            recommendations << "- Schedule routine check-ups and consider testing HbA1c every few months.\n";
            recommendations << "- Maintain a healthy weight and quit smoking if applicable.\n";
        } else if (lowerLabel.find("low") != string::npos) {
            recommendations << "- Low diabetes risk. Keep up the good habits!\n";
            recommendations << "- Continue eating a balanced diet with plenty of fruits and vegetables.\n";
            recommendations << "- Stay active and maintain regular health screenings.\n";
            recommendations << "- Avoid smoking and excessive consumption of sugary drinks.\n";
        } else {
            recommendations << "- Unable to determine recommendation due to unclear prediction result.\n";
        }

        if (hypertension == 1)
            recommendations << "- Hypertension detected: Reduce salt intake, manage stress, and monitor BP regularly.\n";

        if (heart_disease == 1)
            recommendations << "- Heart disease present: Follow a heart-friendly diet, reduce cholesterol, and consult a cardiologist routinely.\n";

        if (bmi > 25.0)
            recommendations << "- High BMI: Consider healthy weight loss through diet and exercise.\n";
        else if (bmi < 18.5)
            recommendations << "- Low BMI: Ensure you're getting enough nutrition to maintain a healthy weight.\n";

        if (glucose > 140.0)
            recommendations << "- Elevated blood glucose: Reduce sugar intake and monitor glucose regularly.\n";
        else if (glucose < 70.0)
            recommendations << "- Low blood glucose: Ensure proper carbohydrate intake to avoid hypoglycemia.\n";

        if (hba1c >= 6.5)
            recommendations << "- HbA1c indicates diabetes: Maintain strict glucose control and consider medication.\n";
        else if (hba1c >= 5.7)
            recommendations << "- HbA1c in prediabetic range: Take preventive actions now to reduce diabetes risk.\n";
        else
            recommendations << "- HbA1c is in normal range: Maintain current healthy habits.\n";

        return recommendations.str();
    }

    void printRecommendations(const string& label, int hypertension, int heart_disease, float bmi, float glucose, float hba1c) const {
        cout << getRecommendations(label, hypertension, heart_disease, bmi, glucose, hba1c);
    }

    vector<string> parseRecommendations(const string& recs) const {
        vector<string> recommendations;
        istringstream iss(recs);
        string line;
        
        while (getline(iss, line)) {
            if (!line.empty() && line[0] == '-') {
                string cleaned = line.substr(2);
                // Remove any existing quotes that might break JSON
                cleaned.erase(remove(cleaned.begin(), cleaned.end(), '\"'), cleaned.end());
                recommendations.push_back(cleaned);
            }
        }
        
        return recommendations;
    }
};

int main() {
    string percent, label;

    try {
        GenderReader genderReader;
        FloatReader ageReader("Enter age: ");
        BinaryReader hypertensionReader("Enter hypertension (0 = No, 1 = Yes): ");
        BinaryReader heartDiseaseReader("Enter heart disease (0 = No, 1 = Yes): ");
        SmokingStatusReader smokingReader;
        FloatReader bmiReader("Enter BMI: ");
        FloatReader hba1cReader("Enter HbA1c Level: ");
        FloatReader glucoseReader("Enter Blood Glucose Level: ");

        string gender = genderReader.readGender();
        float age = ageReader.readFloat();
        int hypertension = hypertensionReader.readBinary();
        int heart_disease = heartDiseaseReader.readBinary();
        string smoking_status = smokingReader.readSmokingStatus();
        float bmi = bmiReader.readFloat();
        float hba1c = hba1cReader.readFloat();
        float glucose = glucoseReader.readFloat();

        // Replace spaces with underscores for command line
        for (auto& c : smoking_status) {
            if (c == ' ') c = '_';
        }

        string cmd = "python predict.py " + gender + " " +
                     to_string(age) + " " + to_string(hypertension) + " " +
                     to_string(heart_disease) + " " + smoking_status + " " +
                     to_string(bmi) + " " + to_string(hba1c) + " " +
                     to_string(glucose);

        cout << "\n[Running prediction...]\n";

        DiabetesPrediction predictor;
        string output = predictor.runPrediction(cmd);

        tie(percent, label) = predictor.parseOutput(output);

        cout << "Diabetes Risk Probability: " << percent << "%\n";
        cout << "Prediction: " << label << "\n\n";

        // Get and print recommendations
        string allRecommendations = predictor.getRecommendations(label, hypertension, heart_disease, bmi, glucose, hba1c);
        cout << allRecommendations << endl;

        // Parse recommendations for JSON output
        vector<string> recommendations = predictor.parseRecommendations(allRecommendations);

        // Write prediction results to file for HTML to read
      // In your main() function, after getting all the input and predictions:

ofstream outFile("result.txt");
if (outFile.is_open()) {
    outFile << "{\n";
    outFile << "  \"gender\": \"" << gender << "\",\n";
    outFile << "  \"age\": " << age << ",\n";
    outFile << "  \"hypertension\": " << hypertension << ",\n";
    outFile << "  \"heart_disease\": " << heart_disease << ",\n";
    outFile << "  \"smoking_status\": \"" << smoking_status << "\",\n";
    outFile << "  \"bmi\": " << bmi << ",\n";
    outFile << "  \"hba1c\": " << hba1c << ",\n";
    outFile << "  \"glucose\": " << glucose << ",\n";
    outFile << "  \"probability\": \"" << percent << "\",\n";
    outFile << "  \"risk_level\": \"" << label << "\",\n";
    outFile << "  \"recommendations\": [\n";
    
    for (size_t i = 0; i < recommendations.size(); ++i) {
        outFile << "    \"" << recommendations[i] << "\"";
        if (i != recommendations.size() - 1) outFile << ",";
        outFile << "\n";
    }
    
    outFile << "  ]\n";
    outFile << "}\n";
    outFile.close();
}
         else {
            cerr << "Could not write to result.txt\n";
            return 1;
        }

        // Open the HTML page in the browser (Windows)
        filesystem::path html_path = filesystem::current_path() / "result.html";
        string command = "start \"\" \"" + html_path.string() + "\"";
        system(command.c_str());

    } catch (const exception& ex) {
        cerr << "An error occurred: " << ex.what() << endl;
        return 1;
    }

    return 0;
}