// Task 1 - Temperature monitoring
//
// A weather station records one integer temperature (in degrees Celsius) per day.
// The records are kept as a sorted (non-decreasing) array and searched with
// binary search, including first/last occurrence lookup.

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#define MAX_DATA 365

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// ---------------------------------------------------------------------------
// Input helpers
// ---------------------------------------------------------------------------

// Prints a prompt and reads one line. Returns false on end of input.
static bool readLine(const string& prompt, string& line) {
    cout << prompt;
    if (!std::getline(cin, line)) {
        cout << endl;
        return false;
    }
    return true;
}

// Strict integer parsing: the whole string (ignoring surrounding spaces) must be one int.
static bool parseInt(const string& text, int& value) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    size_t end = text.find_last_not_of(" \t\r\n");
    if (begin == string::npos) return false;
    try {
        size_t pos = 0;
        long long v = std::stoll(text.substr(begin, end - begin + 1), &pos);
        if (pos != end - begin + 1) return false;
        if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max()) return false;
        value = static_cast<int>(v);
        return true;
    } catch (...) {
        return false;
    }
}

// Parses two integers separated by whitespace, e.g. "20 30".
static bool parseTwoInts(const string& text, int& a, int& b) {
    size_t begin = text.find_first_not_of(" \t\r\n");
    if (begin == string::npos) return false;
    size_t split = text.find_first_of(" \t", begin);
    if (split == string::npos) return false;
    return parseInt(text.substr(begin, split - begin), a) && parseInt(text.substr(split), b);
}

// ---------------------------------------------------------------------------
// Binary search
// ---------------------------------------------------------------------------

// Index of the first element that is >= key (data.size() if there is none).
static int firstNotLess(const vector<int>& data, int key) {
    int low = 0, high = static_cast<int>(data.size());
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (data[mid] < key) low = mid + 1;
        else high = mid;
    }
    return low;
}

// Index of the first element that is > key (data.size() if there is none).
static int firstGreater(const vector<int>& data, int key) {
    int low = 0, high = static_cast<int>(data.size());
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (data[mid] <= key) low = mid + 1;
        else high = mid;
    }
    return low;
}

// First occurrence of key, or -1.
static int findFirst(const vector<int>& data, int key) {
    int idx = firstNotLess(data, key);
    return (idx < static_cast<int>(data.size()) && data[idx] == key) ? idx : -1;
}

// Last occurrence of key, or -1.
static int findLast(const vector<int>& data, int key) {
    int idx = firstGreater(data, key) - 1;
    return (idx >= 0 && data[idx] == key) ? idx : -1;
}

// ---------------------------------------------------------------------------
// Program state and menu actions
// ---------------------------------------------------------------------------

static vector<int> temps;

static bool readCount(int& count) {
    string line;
    if (!readLine("Number Of Measurements (1-" + std::to_string(MAX_DATA) + "): ", line)) return false;
    if (!parseInt(line, count) || count < 1 || count > MAX_DATA) {
        cout << "Invalid Input" << endl;
        count = 0;
    }
    return true;
}

static std::mt19937 createGenerator() {
    std::random_device rd;
    if (rd.entropy() > 0) return std::mt19937(rd());
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return std::mt19937(static_cast<unsigned int>(seed));
}

// Returns false only when the input stream has ended.
static bool manualInput() {
    int count = 0;
    if (!readCount(count)) return false;
    if (count == 0) return true;

    vector<int> input;
    while (static_cast<int>(input.size()) < count) {
        string line;
        int value = 0;
        if (!readLine("Temperature " + std::to_string(input.size() + 1) + "/" + std::to_string(count) + ": ", line)) return false;
        if (!parseInt(line, value)) {
            cout << "Invalid Input" << endl;
            continue;
        }
        if (!input.empty() && value < input.back()) {
            cout << "Temperatures Must Be Entered In Non-Decreasing Order (Previous: " << input.back() << ")" << endl;
            continue;
        }
        input.push_back(value);
    }
    temps = input;
    cout << "Successfully Added " << temps.size() << " Measurements." << endl;
    return true;
}

static bool generateData() {
    int count = 0;
    if (!readCount(count)) return false;
    if (count == 0) return true;

    string line;
    int lower = 0, upper = 0;
    if (!readLine("Temperature Range (min max, separated by a space): ", line)) return false;
    if (!parseTwoInts(line, lower, upper) || lower > upper) {
        cout << "Invalid Input" << endl;
        return true;
    }

    auto gen = createGenerator();
    std::uniform_int_distribution<int> dist(lower, upper);
    vector<int> generated(count);
    for (int& t : generated) t = dist(gen);
    std::sort(generated.begin(), generated.end());
    temps = generated;
    cout << "Generated " << temps.size() << " Measurements." << endl;
    return true;
}

static bool searchRecord() {
    if (temps.empty()) {
        cout << "No Data Loaded." << endl;
        return true;
    }
    string line;
    int key = 0;
    if (!readLine("Temperature In Question: ", line)) return false;
    if (!parseInt(line, key)) {
        cout << "Invalid Input" << endl;
        return true;
    }
    int first = findFirst(temps, key);
    if (first == -1) {
        cout << "Temperature Not Found." << endl;
        return true;
    }
    int last = findLast(temps, key);
    if (first == last) {
        cout << "Temperature Found On Position: " << first << endl;
    } else {
        cout << "Temperature On First Position: " << first << endl;
        cout << "Temperature On Last Position: " << last << endl;
    }
    return true;
}

static bool searchInRange() {
    if (temps.empty()) {
        cout << "No Data Loaded." << endl;
        return true;
    }
    string line;
    int lower = 0, upper = 0;
    if (!readLine("Temperature Range (min max, separated by a space): ", line)) return false;
    if (!parseTwoInts(line, lower, upper) || lower > upper) {
        cout << "Invalid Input" << endl;
        return true;
    }
    int from = firstNotLess(temps, lower);
    int to = firstGreater(temps, upper);  // exclusive
    if (from >= to) {
        cout << "No Temperatures In Range." << endl;
        return true;
    }
    cout << "Temperatures In Range (Temperature, Position): ";
    for (int i = from; i < to; i++) {
        cout << "(" << temps[i] << ", " << i << ")" << (i + 1 < to ? ", " : "");
    }
    cout << endl;
    return true;
}

static void printData() {
    if (temps.empty()) {
        cout << "No Data Loaded." << endl;
        return;
    }
    for (size_t i = 0; i < temps.size(); i++) {
        cout << temps[i] << (i + 1 < temps.size() ? " " : "");
    }
    cout << endl;
}

static void clearData() {
    temps.clear();
    temps.shrink_to_fit();
    cout << "Data Was Successfully Cleared." << endl;
}

static void printMenu() {
    cout << "-----MENU-----" << endl;
    cout << "1. Manual Data Input" << endl;
    cout << "2. Generate Random Data" << endl;
    cout << "3. Search Specific Temperature" << endl;
    cout << "4. Search Temperatures In Range" << endl;
    cout << "5. Clear All Data" << endl;
    cout << "6. Print Data" << endl;
    cout << "0. Exit" << endl;
}

int main() {
    while (true) {
        printMenu();
        string line;
        int choice = 0;
        if (!readLine("Your Choice: ", line)) break;
        if (!parseInt(line, choice)) {
            cout << "Invalid Choice" << endl;
            continue;
        }
        if (choice == 0) break;

        bool inputAlive = true;
        switch (choice) {
            case 1: inputAlive = manualInput(); break;
            case 2: inputAlive = generateData(); break;
            case 3: inputAlive = searchRecord(); break;
            case 4: inputAlive = searchInRange(); break;
            case 5: clearData(); break;
            case 6: printData(); break;
            default: cout << "Invalid Choice" << endl; break;
        }
        if (!inputAlive) break;
    }
    return 0;
}
