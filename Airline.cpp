#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <cctype>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define MKDIR(dir) _mkdir(dir)
#define FILE_EXISTS(file) (_access(file, 0) != -1)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#define FILE_EXISTS(file) (access(file, F_OK) != -1)
#endif

using namespace std;

string toLower(string s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), 
              [](unsigned char c){ return tolower(c); });
    return result;
}

bool equalsIgnoreCase(const string& a, const string& b) {
    return toLower(a) == toLower(b);
}

bool containsIgnoreCase(const string& haystack, const string& needle) {
    return toLower(haystack).find(toLower(needle)) != string::npos;
}

bool isNumeric(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

void printHeader(const string& title) {
    cout << "\n" << string(80, '-') << "\n";
    cout << "  " << title << "\n";
    cout << string(80, '-') << "\n";
}

void printSubHeader(const string& title) {
    cout << "\n" << title << "\n";
    cout << string(title.length(), '-') << "\n";
}

void printMenuOption(int number, const string& option) {
    cout << "  [" << number << "] " << option << endl;
}

void printBackOption() {
    cout << "  [0] Back to previous menu" << endl;
}

void printSuccessMessage(const string& message) {
    cout << "\n  ✓ " << message << endl;
}

void printErrorMessage(const string& message) {
    cout << "\n  ! " << message << endl;
}

void printInfoMessage(const string& message) {
    cout << "\n  ! " << message << endl;
}

void printWarningMessage(const string& message) {
    cout << "\n  * " << message << endl;
}

void printTableHeader(const vector<pair<string, int>>& columns) {
    for (const auto& col : columns) {
        cout << setw(col.second) << left << col.first;
    }
    cout << endl;

    string separator;
    for (const auto& col : columns) {
        separator += string(col.second, '-');
    }
    cout << separator << endl;
}

void printTableRow(const vector<pair<string, int>>& values) {
    for (const auto& val : values) {
        cout << setw(val.second) << left << val.first;
    }
    cout << endl;
}

void printSeparator() {
    cout << string(80, '-') << endl;
}

void printPrompt(const string& prompt) {
    cout << prompt << " ";
}

int getValidIntegerInput(const string& prompt, int min, int max) {
    string input;
    int value;
    bool valid = false;
    
    while (!valid) {
        printPrompt(prompt);
        getline(cin, input);
        
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (input.empty()) {
            printErrorMessage("Input cannot be empty. Please try again.");
            continue;
        }
        
        if (!isNumeric(input)) {
            printErrorMessage("Invalid input. Please enter a number.");
            continue;
        }
        
        try {
            value = stoi(input);
            
            if (value < min || value > max) {
                printErrorMessage("Input must be between " + to_string(min) + " and " + to_string(max) + ". Please try again.");
                continue;
            }
            
            valid = true;
        } catch (const exception& e) {
            printErrorMessage("Invalid input. Please enter a valid number.");
        }
    }
    
    return value;
}

char getYesNoInput(const string& prompt) {
    string input;
    bool valid = false;
    char result;
    
    while (!valid) {
        printPrompt(prompt);
        getline(cin, input);
        
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (input.empty()) {
            printErrorMessage("Input cannot be empty. Please enter 'y' or 'n'.");
            continue;
        }
        
        if (input.length() != 1) {
            printErrorMessage("Invalid input. Please enter 'y' or 'n'.");
            continue;
        }
        
        result = tolower(input[0]);
        
        if (result != 'y' && result != 'n') {
            printErrorMessage("Invalid input. Please enter 'y' or 'n'.");
            continue;
        }
        
        valid = true;
    }
    
    return result;
}

class FileOperationException : public exception {
private:
    string message;
public:
    FileOperationException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class ValidationException : public exception {
private:
    string message;
public:
    ValidationException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class BookingException : public exception {
private:
    string message;
public:
    BookingException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class DatabaseManager {
private:
    DatabaseManager() {}

    static DatabaseManager* instance;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
    static DatabaseManager* getInstance() {
        if (instance == nullptr) {
            instance = new DatabaseManager();
        }
        return instance;
    }

    bool saveData(const string& filename, const string& data) {
        try {
            if (data.empty()) {
                return deleteFile(filename);
            }
            
            ofstream file(filename, ios::app);
            if (!file.is_open()) {
                throw FileOperationException("Failed to open file: " + filename);
            }
            file << data << endl;
            file.close();
            return true;
        } catch (const exception& e) {
            printErrorMessage("Error saving data: " + string(e.what()));
            return false;
        }
    }

    bool saveDataOverwrite(const string& filename, const string& data) {
        try {
            if (data.empty()) {
                return deleteFile(filename);
            }
            
            ofstream file(filename, ios::trunc);
            if (!file.is_open()) {
                throw FileOperationException("Failed to open file: " + filename);
            }
            file << data;
            file.close();
            return true;
        } catch (const exception& e) {
            printErrorMessage("Error saving data: " + string(e.what()));
            return false;
        }
    }

    string loadData(const string& filename) {
        try {
            ifstream file(filename);
            if (!file.is_open()) {
                return "";
            }
            
            stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            return buffer.str();
        } catch (const exception& e) {
            printErrorMessage("Error loading data: " + string(e.what()));
            return "";
        }
    }

    bool fileExists(const string& filename) {
        return FILE_EXISTS(filename.c_str());
    }

    bool deleteFile(const string& filename) {
        try {
            if (!fileExists(filename)) {
                return true;
            }
            
            if (remove(filename.c_str()) != 0) {
                throw FileOperationException("Failed to delete file: " + filename);
            }
            return true;
        } catch (const exception& e) {
            printErrorMessage("Error deleting file: " + string(e.what()));
            return false;
        }
    }

    ~DatabaseManager() {}
};

DatabaseManager* DatabaseManager::instance = nullptr;

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() {}
    virtual bool processPayment(double amount) = 0;
    virtual string getPaymentDetails() = 0;
};

class GCashPaymentStrategy : public PaymentStrategy {
private:
    string gcashNumber;

public:
    GCashPaymentStrategy(const string& number) : gcashNumber(number) {}

    bool processPayment(double amount) override {
        printInfoMessage("Processing GCash payment of $" + to_string(amount) + 
                        " using number " + gcashNumber + "...");
        
        return true;
    }

    string getPaymentDetails() override {
        return "GCash: " + gcashNumber;
    }
};

class CreditCardPaymentStrategy : public PaymentStrategy {
private:
    string cardNumber;
    string expiryDate;
    string cvv;

public:
    CreditCardPaymentStrategy(const string& number, const string& expiry, const string& cvv)
        : cardNumber(number), expiryDate(expiry), cvv(cvv) {}

    bool processPayment(double amount) override {
        printInfoMessage("Processing Credit Card payment of $" + to_string(amount) + 
                        " using card ending with " + cardNumber.substr(cardNumber.length() - 4) + "...");
        
        return true;
    }

    string getPaymentDetails() override {
        return "Credit Card: XXXX-XXXX-XXXX-" + cardNumber.substr(cardNumber.length() - 4);
    }
};

vector<class Flight> flights;
vector<class User*> users;
vector<class Reservation> reservations;
map<string, class WaitingList> waitingLists;
class User* currentUser = nullptr;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue..." << flush;

#ifdef _WIN32
    system("pause > nul");
#else
    cout << flush;
    system("read -n 1 -s");
#endif
}

bool createDirectory(const string& path) {
    int result = MKDIR(path.c_str());
    return (result == 0 || errno == EEXIST);
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    string months[] = {"January", "February", "March", "April", "May", "June", 
                      "July", "August", "September", "October", "November", "December"};

    stringstream ss;
    ss << months[ltm->tm_mon] << " " << ltm->tm_mday << ", " 
       << 1900 + ltm->tm_year << " – ";

    int hour = ltm->tm_hour;
    string ampm = "AM";
    if (hour >= 12) {
        ampm = "PM";
        if (hour > 12) hour -= 12;
    }
    if (hour == 0) hour = 12;

    ss << setfill('0') << setw(2) << hour << ":" 
       << setfill('0') << setw(2) << ltm->tm_min << " " << ampm;

    return ss.str();
}

string generateID(const string& prefix) {
    static map<string, int> counters;

    if (counters.find(prefix) == counters.end()) {
        counters[prefix] = 10000;
        
        DatabaseManager* dbManager = DatabaseManager::getInstance();
        string data;
        int highestID = 10000;
        
        if (prefix == "FL") {
            data = dbManager->loadData("flights.txt");
        } else if (prefix == "RES") {
            data = dbManager->loadData("reservations.txt");
        } else if (prefix == "USR") {
            data = dbManager->loadData("users.txt");
        }
        
        stringstream dataStream(data);
        string line;
        
        while (getline(dataStream, line)) {
            stringstream ss(line);
            string token;
            
            if (getline(ss, token, ',')) {
                if (token.substr(0, prefix.length()) == prefix) {
                    string numPart = token.substr(prefix.length());
                    try {
                        int idNum = stoi(numPart);
                        if (idNum > highestID) {
                            highestID = idNum;
                        }
                    } catch (const exception& e) {
                        continue;
                    }
                }
            }
        }
        
        counters[prefix] = highestID;
    }

    return prefix + to_string(++counters[prefix]);
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

class Flight {
private:
    string flightID;
    string airlineName;
    string planeID;
    int capacity;
    int availableSeats;
    string destination;
    string departureTime;
    string arrivalTime;
    string status;
    vector<vector<bool>> seatMap;
    int seatsPerRow;
    int totalColumns;

public:
    Flight() : seatsPerRow(3), totalColumns(7) {}

    Flight(const string& airlineName, const string& planeID, int capacity, 
           const string& destination, const string& departureTime, 
           const string& arrivalTime) 
        : airlineName(airlineName), planeID(planeID), capacity(capacity), 
          availableSeats(capacity), destination(destination), 
          departureTime(departureTime), arrivalTime(arrivalTime), status("On Time") {
        
        flightID = generateID("FL");
        
        calculateSeatLayout();
        
        initializeSeatMap();
    }

    void calculateSeatLayout() {
        if (capacity < 60) {
            seatsPerRow = 2;
            totalColumns = 5;
        }
        else if (capacity < 150) {
            seatsPerRow = 3;
            totalColumns = 7;
        }
        else {
            seatsPerRow = 5;
            totalColumns = 11;
        }
    }

    void initializeSeatMap() {
        int seatsPerFullRow = totalColumns - 1;
        int fullRows = capacity / seatsPerFullRow;
        
        int remainingSeats = capacity % seatsPerFullRow;
        
        int totalRows = fullRows + (remainingSeats > 0 ? 1 : 0);
        
        seatMap.resize(totalRows);
        
        for (int i = 0; i < totalRows; i++) {
            if (i == totalRows - 1 && remainingSeats > 0) {
                seatMap[i].resize(totalColumns, false);
                
                int seatsLeft = remainingSeats;
                int col = 0;
                
                while (col < seatsPerRow && seatsLeft > 0) {
                    seatMap[i][col] = false;
                    col++;
                    seatsLeft--;
                }
                
                col++;
                
                while (col < totalColumns && seatsLeft > 0) {
                    seatMap[i][col] = false;
                    col++;
                    seatsLeft--;
                }
                
                while (col < totalColumns) {
                    seatMap[i][col] = true;
                    col++;
                }
            } else {
                seatMap[i].resize(totalColumns, false);
                
                if (totalColumns == 5) {
                    seatMap[i][2] = true;
                } else if (totalColumns == 7) {
                    seatMap[i][3] = true;
                } else if (totalColumns == 11) {
                    seatMap[i][3] = true;
                    seatMap[i][8] = true;
                }
            }
        }
        
        int totalSeats = 0;
        for (size_t i = 0; i < seatMap.size(); i++) {
            for (size_t j = 0; j < seatMap[i].size(); j++) {
                if ((totalColumns == 5 && j == 2) || 
                    (totalColumns == 7 && j == 3) || 
                    (totalColumns == 11 && (j == 3 || j == 8))) {
                    continue;
                }
                
                if (!seatMap[i][j]) {
                    totalSeats++;
                }
            }
        }
        
        if (totalSeats > capacity) {
            int excessSeats = totalSeats - capacity;
            
            for (int i = seatMap.size() - 1; i >= 0 && excessSeats > 0; i--) {
                for (int j = seatMap[i].size() - 1; j >= 0 && excessSeats > 0; j--) {
                    if ((totalColumns == 5 && j == 2) || 
                        (totalColumns == 7 && j == 3) || 
                        (totalColumns == 11 && (j == 3 || j == 8))) {
                        continue;
                    }
                    
                    if (!seatMap[i][j]) {
                        seatMap[i][j] = true;
                        excessSeats--;
                    }
                }
            }
        }
    }

    string getFlightID() const { return flightID; }
    string getAirlineName() const { return airlineName; }
    string getPlaneID() const { return planeID; }
    int getCapacity() const { return capacity; }
    int getAvailableSeats() const { return availableSeats; }
    string getDestination() const { return destination; }
    string getDepartureTime() const { return departureTime; }
    string getArrivalTime() const { return arrivalTime; }
    string getStatus() const { return status; }

    void setAirlineName(const string& name) { airlineName = name; }
    void setPlaneID(const string& id) { planeID = id; }
    void setCapacity(int cap) { 
        capacity = cap; 
        calculateSeatLayout();
        initializeSeatMap();
    }
    void setDestination(const string& dest) { destination = dest; }
    void setDepartureTime(const string& time) { departureTime = time; }
    void setArrivalTime(const string& time) { arrivalTime = time; }
    void setStatus(const string& stat) { status = stat; }

    pair<int, int> seatNumberToIndices(const string& seatNumber) const {
        try {
            if (seatNumber.length() < 2) {
                throw ValidationException("Invalid seat number format");
            }
            
            int row;
            try {
                row = stoi(seatNumber.substr(0, seatNumber.length() - 1)) - 1;
            } catch (const invalid_argument&) {
                throw ValidationException("Invalid row number in seat");
            }
            
            char colLetter = toupper(seatNumber.back());
            
            int col;
            if (colLetter >= 'A' && colLetter <= 'Z') {
                col = colLetter - 'A';
                
                if (totalColumns == 5) {
                    if (col >= 2) col++;
                } else if (totalColumns == 7) {
                    if (col >= 3) col++;
                } else if (totalColumns == 11) {
                    if (col >= 3 && col < 7) col++;
                    else if (col >= 7) col += 2;
                }
            } else {
                throw ValidationException("Invalid column letter in seat");
            }
            
            return make_pair(row, col);
        } catch (const exception& e) {
            throw ValidationException(string("Error parsing seat number: ") + e.what());
        }
    }

    string indicesToSeatNumber(int row, int col) const {
        int adjustedCol = col;
        if (totalColumns == 5) {
            if (col > 2) adjustedCol--;
        } else if (totalColumns == 7) {
            if (col > 3) adjustedCol--;
        } else if (totalColumns == 11) {
            if (col > 3 && col <= 8) adjustedCol--;
            else if (col > 8) adjustedCol -= 2;
        }
        
        char colLetter = 'A' + adjustedCol;
        return to_string(row + 1) + colLetter;
    }

    bool isSeatAvailable(const string& seatNumber) const {
        try {
            pair<int, int> indices = seatNumberToIndices(seatNumber);
            int row = indices.first;
            int col = indices.second;
            
            if (row < 0 || row >= seatMap.size() || col < 0 || col >= totalColumns) {
                throw ValidationException("Seat number out of range");
            }
            
            if ((totalColumns == 5 && col == 2) || 
                (totalColumns == 7 && col == 3) || 
                (totalColumns == 11 && (col == 3 || col == 8))) {
                throw ValidationException("Cannot book an aisle");
            }
            
            return !seatMap[row][col];
        } catch (const exception& e) {
            printErrorMessage("Error checking seat availability: " + string(e.what()));
            return false;
        }
    }

    bool bookSeat(const string& seatNumber) {
        try {
            if (!isSeatAvailable(seatNumber)) {
                throw BookingException("Seat " + seatNumber + " is not available");
            }
            
            pair<int, int> indices = seatNumberToIndices(seatNumber);
            int row = indices.first;
            int col = indices.second;
            
            seatMap[row][col] = true;
            availableSeats--;
            
            return true;
        } catch (const exception& e) {
            printErrorMessage("Error booking seat: " + string(e.what()));
            return false;
        }
    }

    bool cancelSeat(const string& seatNumber) {
        try {
            pair<int, int> indices = seatNumberToIndices(seatNumber);
            int row = indices.first;
            int col = indices.second;
            
            if (row < 0 || row >= seatMap.size() || col < 0 || col >= totalColumns) {
                throw ValidationException("Seat number out of range");
            }
            
            if ((totalColumns == 5 && col == 2) || 
                (totalColumns == 7 && col == 3) || 
                (totalColumns == 11 && (col == 3 || col == 8))) {
                throw ValidationException("Cannot cancel an aisle");
            }
            
            if (!seatMap[row][col]) {
                throw BookingException("Seat " + seatNumber + " is already available");
            }
            
            seatMap[row][col] = false;
            availableSeats++;
            
            return true;
        } catch (const exception& e) {
            printErrorMessage("Error canceling seat: " + string(e.what()));
            return false;
        }
    }

    void displaySeatMap() const {
        printSubHeader("Seat Map for Flight " + flightID + " (" + airlineName + ")");
        
        cout << "  Destination: " << destination << "\n";
        cout << "  Available Seats: " << availableSeats << " out of " << capacity << "\n\n";
        
        cout << "    ";
        char seatLetter = 'A';
        for (int j = 0; j < totalColumns; j++) {
            if ((totalColumns == 5 && j == 2) || 
                (totalColumns == 7 && j == 3) || 
                (totalColumns == 11 && (j == 3 || j == 8))) {
                cout << "    ";
            } else {
                cout << seatLetter << "   ";
                seatLetter++;
            }
        }
        cout << "\n";
        
        for (size_t i = 0; i < seatMap.size(); i++) {
            cout << setw(2) << i + 1 << "  ";
            
            for (size_t j = 0; j < seatMap[i].size(); j++) {
                if ((totalColumns == 5 && j == 2) || 
                    (totalColumns == 7 && j == 3) || 
                    (totalColumns == 11 && (j == 3 || j == 8))) {
                    cout << "|   ";
                } else {
                    if (j < seatMap[i].size()) {
                        if (seatMap[i][j]) {
                            cout << "X   ";
                        } else {
                            cout << "O   ";
                        }
                    }
                }
            }
            cout << "\n";
        }
        
        cout << "\n";
        cout << "  O - Available  ";
        cout << "X - Occupied  ";
        cout << "| - Aisle\n";
    }

    string getFirstAvailableSeat() const {
        for (size_t i = 0; i < seatMap.size(); i++) {
            for (size_t j = 0; j < seatMap[i].size(); j++) {
                if ((totalColumns == 5 && j == 2) || 
                    (totalColumns == 7 && j == 3) || 
                    (totalColumns == 11 && (j == 3 || j == 8))) {
                    continue;
                }
                
                if (!seatMap[i][j]) {
                    return indicesToSeatNumber(i, j);
                }
            }
        }
        return "";
    }

    bool isFullyBooked() const {
        return availableSeats == 0;
    }

    void saveToFile() const {
        try {
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            
            stringstream ss;
            ss << flightID << ","
               << airlineName << ","
               << planeID << ","
               << capacity << ","
               << availableSeats << ","
               << destination << ","
               << departureTime << ","
               << arrivalTime << ","
               << status;
            
            dbManager->saveData("flights.txt", ss.str());
            
            stringstream seatSS;
            for (size_t i = 0; i < seatMap.size(); i++) {
                for (size_t j = 0; j < seatMap[i].size(); j++) {
                    seatSS << (seatMap[i][j] ? "1" : "0") << ",";
                }
                seatSS << "\n";
            }
            
            dbManager->saveDataOverwrite("seatmaps/" + flightID + ".txt", seatSS.str());
        } catch (const exception& e) {
            printErrorMessage("Error saving flight: " + string(e.what()));
        }
    }

    static void loadFlights() {
        try {
            flights.clear();
            
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            string fileContent = dbManager->loadData("flights.txt");
            
            if (fileContent.empty()) {
                return;
            }
            
            istringstream fileStream(fileContent);
            string line;
            
            while (getline(fileStream, line)) {
                if (line.empty()) {
                    continue;
                }
                
                vector<string> fields;
                string field;
                bool inQuotes = false;
                
                for (size_t i = 0; i < line.length(); i++) {
                    char c = line[i];
                    
                    if (c == '"') {
                        inQuotes = !inQuotes;
                    } else if (c == ',' && !inQuotes) {
                        fields.push_back(field);
                        field.clear();
                    } else {
                        field += c;
                    }
                }
                
                fields.push_back(field);
                
                if (fields.size() < 9) {
                    printErrorMessage("Invalid flight data format: " + line);
                    continue;
                }
                
                Flight flight;
                flight.flightID = fields[0];
                flight.airlineName = fields[1];
                flight.planeID = fields[2];
                
                try {
                    flight.capacity = stoi(fields[3]);
                    flight.availableSeats = stoi(fields[4]);
                } catch (const exception& e) {
                    printErrorMessage("Error parsing numeric flight data: " + string(e.what()));
                    continue;
                }
                
                flight.destination = fields[5];
                
                if (fields[6].find(" - ") == string::npos) {
                    flight.departureTime = "May 10, 2025 - 08:00 AM";
                } else {
                    flight.departureTime = fields[6];
                }
                
                if (fields[7].find("May 10, 2025") == string::npos) {
                    flight.arrivalTime = "May 10, 2025 - 10:00 AM";
                } else {
                    flight.arrivalTime = fields[7];
                }
                
                if (fields[8] == "may 10") {
                    flight.status = "On Time";
                } else {
                    flight.status = fields[8];
                }
                
                flight.calculateSeatLayout();
                
                flight.seatMap.clear();
                string seatData = dbManager->loadData("seatmaps/" + flight.flightID + ".txt");
                stringstream seatStream(seatData);
                string seatLine;
                
                while (getline(seatStream, seatLine)) {
                    stringstream seatSS(seatLine);
                    string seatToken;
                    vector<bool> row;
                    
                    while (getline(seatSS, seatToken, ',')) {
                        if (!seatToken.empty()) {
                            row.push_back(seatToken == "1");
                        }
                    }
                    
                    if (!row.empty()) {
                        flight.seatMap.push_back(row);
                    }
                }
                
                if (flight.seatMap.empty()) {
                    flight.initializeSeatMap();
                }
                
                flights.push_back(flight);
            }
        } catch (const exception& e) {
            printErrorMessage("Error loading flights: " + string(e.what()));
        }
    }

    static void saveAllFlights() {
        try {
            ofstream file("flights.txt", ios::trunc);
            file.close();
            
            for (const auto& flight : flights) {
                flight.saveToFile();
            }
        } catch (const exception& e) {
            printErrorMessage("Error saving all flights: " + string(e.what()));
        }
    }
};

class Reservation {
private:
    string reservationID;
    string passengerName;
    string flightID;
    string airlineName;
    string destination;
    string seatNumber;
    string status;
    string username;
    string paymentMethod;

public:
    Reservation() {}

    Reservation(const string& passengerName, const string& flightID, 
                const string& airlineName, const string& destination, 
                const string& seatNumber, const string& username,
                const string& paymentMethod = "")
        : passengerName(passengerName), flightID(flightID), 
          airlineName(airlineName), destination(destination), 
          seatNumber(seatNumber), status("Confirmed"), username(username),
          paymentMethod(paymentMethod) {
        
        reservationID = generateID("RES");
    }

    string getReservationID() const { return reservationID; }
    string getPassengerName() const { return passengerName; }
    string getFlightID() const { return flightID; }
    string getAirlineName() const { return airlineName; }
    string getDestination() const { return destination; }
    string getSeatNumber() const { return seatNumber; }
    string getStatus() const { return status; }
    string getUsername() const { return username; }
    string getPaymentMethod() const { return paymentMethod; }

    void saveToFile() const {
        try {
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            
            stringstream ss;
            ss << reservationID << ","
               << passengerName << ","
               << flightID << ","
               << airlineName << ","
               << destination << ","
               << seatNumber << ","
               << status << ","
               << username << ","
               << paymentMethod;
            
            dbManager->saveData("reservations.txt", ss.str());
        } catch (const exception& e) {
            printErrorMessage("Error saving reservation: " + string(e.what()));
        }
    }

    static void loadReservations() {
        try {
            reservations.clear();
            
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            string data = dbManager->loadData("reservations.txt");
            
            stringstream dataStream(data);
            string line;
            
            while (getline(dataStream, line)) {
                stringstream ss(line);
                string token;
                vector<string> tokens;
                
                while (getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() >= 8) {
                    Reservation reservation;
                    reservation.reservationID = tokens[0];
                    reservation.passengerName = tokens[1];
                    reservation.flightID = tokens[2];
                    reservation.airlineName = tokens[3];
                    reservation.destination = tokens[4];
                    reservation.seatNumber = tokens[5];
                    reservation.status = tokens[6];
                    reservation.username = tokens[7];
                    
                    if (tokens.size() >= 9) {
                        reservation.paymentMethod = tokens[8];
                    }
                    
                    reservations.push_back(reservation);
                }
            }
        } catch (const exception& e) {
            printErrorMessage("Error loading reservations: " + string(e.what()));
        }
    }

    static void saveAllReservations() {
        try {
            ofstream file("reservations.txt", ios::trunc);
            file.close();
            
            for (const auto& reservation : reservations) {
                reservation.saveToFile();
            }
        } catch (const exception& e) {
            printErrorMessage("Error saving all reservations: " + string(e.what()));
        }
    }
};

class WaitingList {
private:
    string flightID;
    vector<pair<string, string>> passengers;

public:
    WaitingList() {}

    WaitingList(const string& flightID) : flightID(flightID) {}

    void addPassenger(const string& username, const string& passengerName) {
        passengers.push_back(make_pair(username, passengerName));
    }

    bool removePassenger(const string& username) {
        for (auto it = passengers.begin(); it != passengers.end(); ++it) {
            if (it->first == username) {
                passengers.erase(it);
                return true;
            }
        }
        return false;
    }

    pair<string, string> getNextPassenger() const {
        if (passengers.empty()) {
            return make_pair("", "");
        }
        return passengers.front();
    }

    bool isEmpty() const {
        return passengers.empty();
    }

    void display() const {
        printSubHeader("Waiting List for Flight " + flightID);
        
        if (passengers.empty()) {
            printInfoMessage("No passengers in the waiting list.");
            return;
        }
        
        vector<pair<string, int>> columns = {
            {"No.", 5},
            {"Passenger Name", 25},
            {"Username", 20}
        };
        
        printTableHeader(columns);
        
        for (size_t i = 0; i < passengers.size(); i++) {
            cout << setw(5) << i + 1 
                 << setw(25) << passengers[i].second 
                 << setw(20) << passengers[i].first << "\n";
        }
    }

    void saveToFile() const {
        try {
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            
            if (passengers.empty()) {
                dbManager->deleteFile("waitinglists/" + flightID + ".txt");
                return;
            }
            
            stringstream ss;
            for (const auto& passenger : passengers) {
                ss << passenger.first << "," << passenger.second << "\n";
            }
            
            ofstream file("waitinglists/" + flightID + ".txt", ios::trunc);
            file.close();
            
            dbManager->saveData("waitinglists/" + flightID + ".txt", ss.str());
        } catch (const exception& e) {
            printErrorMessage("Error saving waiting list: " + string(e.what()));
        }
    }

    static void loadWaitingLists() {
        try {
            waitingLists.clear();
            
            for (const auto& flight : flights) {
                string flightID = flight.getFlightID();
                WaitingList waitingList(flightID);
                
                DatabaseManager* dbManager = DatabaseManager::getInstance();
                string data = dbManager->loadData("waitinglists/" + flightID + ".txt");
                
                stringstream dataStream(data);
                string line;
                
                while (getline(dataStream, line)) {
                    stringstream ss(line);
                    string username, passengerName;
                    
                    getline(ss, username, ',');
                    getline(ss, passengerName);
                    
                    waitingList.addPassenger(username, passengerName);
                }
                
                waitingLists[flightID] = waitingList;
            }
        } catch (const exception& e) {
            printErrorMessage("Error loading waiting lists: " + string(e.what()));
        }
    }

    static void saveAllWaitingLists() {
        try {
            for (auto& pair : waitingLists) {
                pair.second.saveToFile();
            }
        } catch (const exception& e) {
            printErrorMessage("Error saving all waiting lists: " + string(e.what()));
        }
    }
};

class Admin;
class Customer;

class User {
private:
    string username;
    string password;
    string name;
    bool isAdmin;

public:
    User() : isAdmin(false) {}

    User(const string& username, const string& password, const string& name, bool isAdmin)
        : username(username), password(password), name(name), isAdmin(isAdmin) {}

    virtual ~User() {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getName() const { return name; }
    bool getIsAdmin() const { return isAdmin; }

    void setUsername(const string& value) { username = value; }
    void setPassword(const string& value) { password = value; }
    void setName(const string& value) { name = value; }
    void setIsAdmin(bool value) { isAdmin = value; }

    virtual void displayMenu() = 0;

    virtual void saveToFile() const {
        try {
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            
            stringstream ss;
            ss << username << ","
               << password << ","
               << name << ","
               << (isAdmin ? "admin" : "customer");
            
            dbManager->saveData("users.txt", ss.str());
        } catch (const exception& e) {
            printErrorMessage("Error saving user: " + string(e.what()));
        }
    }

    static void loadUsers();
    static void saveAllUsers() {
        try {
            ofstream file("users.txt", ios::trunc);
            file.close();
            
            for (const auto& user : users) {
                user->saveToFile();
            }
        } catch (const exception& e) {
            printErrorMessage("Error saving all users: " + string(e.what()));
        }
    }

    static User* login(const string& username, const string& password) {
        for (auto& user : users) {
            if (user->getUsername() == username && user->getPassword() == password) {
                return user;
            }
        }
        return nullptr;
    }

    static bool usernameExists(const string& username) {
        for (const auto& user : users) {
            if (user->getUsername() == username) {
                return true;
            }
        }
        return false;
    }
};

class Admin : public User {
public:
    Admin() {
        setIsAdmin(true);
    }

    Admin(const string& username, const string& password, const string& name)
        : User(username, password, name, true) {}

    void displayMenu() override {
        int choice;
        do {
            clearScreen();
            printHeader("ADMIN DASHBOARD");
            
            cout << "  Welcome, " << getName() << "!" << endl;
            cout << "  " << getCurrentDateTime() << endl;
            
            printSeparator();
            
            printMenuOption(1, "Create Flight");
            printMenuOption(2, "Delete Flight");
            printMenuOption(3, "Reservations (View/Delete)");
            printMenuOption(4, "Flight Status (View/Edit)");
            printMenuOption(5, "View Seat Maps");
            printMenuOption(6, "Manage Waiting List");
            printMenuOption(7, "User Accounts");
            printMenuOption(8, "Logout");
            
            choice = getValidIntegerInput("Enter your choice:", 1, 8);
            
            switch (choice) {
                case 1:
                    createFlight();
                    break;
                case 2:
                    deleteFlight();
                    break;
                case 3:
                    manageReservations();
                    break;
                case 4:
                    manageFlightStatus();
                    break;
                case 5:
                    viewSeatMaps();
                    break;
                case 6:
                    manageWaitingList();
                    break;
                case 7:
                    manageUserAccounts();
                    break;
                case 8:
                    printInfoMessage("Logging out...");
                    break;
            }
        } while (choice != 8);
    }

    void createFlight() {
        clearScreen();
        printHeader("CREATE FLIGHT");
        
        string airlineName, planeID, destination, departureTime, arrivalTime;
        int capacity;
        
        try {
            bool validInput = false;
            while (!validInput) {
                printPrompt("Enter airline name (or 'b' to go back):");
                getline(cin, airlineName);
                if (airlineName == "b" || airlineName == "B") return;
                if (airlineName.empty()) {
                    printErrorMessage("Airline name cannot be empty. Please try again.");
                } else {
                    validInput = true;
                }
            }
            
            validInput = false;
            while (!validInput) {
                printPrompt("Enter plane number/ID:");
                getline(cin, planeID);
                if (planeID.empty()) {
                    printErrorMessage("Plane ID cannot be empty. Please try again.");
                } else {
                    validInput = true;
                }
            }
            
            string capacityStr;
            validInput = false;
            while (!validInput) {
                printPrompt("Enter airplane capacity:");
                getline(cin, capacityStr);
                
                capacityStr.erase(0, capacityStr.find_first_not_of(" \t\n\r\f\v"));
                capacityStr.erase(capacityStr.find_last_not_of(" \t\n\r\f\v") + 1);
                
                if (capacityStr.empty()) {
                    printErrorMessage("Capacity cannot be empty. Please try again.");
                    continue;
                }
                
                if (!isNumeric(capacityStr)) {
                    printErrorMessage("Invalid input. Please enter a number.");
                    continue;
                }
                
                try {
                    capacity = stoi(capacityStr);
                    if (capacity <= 0) {
                        printErrorMessage("Capacity must be greater than zero. Please try again.");
                        continue;
                    }
                    validInput = true;
                } catch (const exception& e) {
                    printErrorMessage("Invalid input. Please enter a valid number.");
                }
            }
            
            validInput = false;
            while (!validInput) {
                printPrompt("Enter flight destination (e.g., Manila to South Africa):");
                getline(cin, destination);
                if (destination.empty()) {
                    printErrorMessage("Destination cannot be empty. Please try again.");
                } else {
                    validInput = true;
                }
            }
            
            validInput = false;
            while (!validInput) {
                printPrompt("Enter flight departure time (e.g., May 10, 2025 - 08:00 AM):");
                getline(cin, departureTime);
                if (departureTime.empty()) {
                    printErrorMessage("Departure time cannot be empty. Please try again.");
                } else {
                    validInput = true;
                }
            }
            
            validInput = false;
            while (!validInput) {
                printPrompt("Enter arrival time (e.g., May 10, 2025 - 09:30 AM):");
                getline(cin, arrivalTime);
                if (arrivalTime.empty()) {
                    printErrorMessage("Arrival time cannot be empty. Please try again.");
                } else {
                    validInput = true;
                }
            }
            
            clearScreen();
            printSubHeader("Flight Summary");
            
            cout << "  Airline: " << airlineName << endl;
            cout << "  Plane ID: " << planeID << endl;
            cout << "  Capacity: " << capacity << " passengers" << endl;
            cout << "  Destination: " << destination << endl;
            cout << "  Departure: " << departureTime << endl;
            cout << "  Arrival: " << arrivalTime << endl;
            
            char confirm = getYesNoInput("\nConfirm flight creation (y/n):");
            
            if (confirm == 'y') {
                Flight flight(airlineName, planeID, capacity, destination, departureTime, arrivalTime);
                flights.push_back(flight);
                flight.saveToFile();
                
                WaitingList waitingList(flight.getFlightID());
                waitingLists[flight.getFlightID()] = waitingList;
                
                printSuccessMessage("Flight created successfully!");
            } else {
                printInfoMessage("Flight creation cancelled.");
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void deleteFlight() {
        clearScreen();
        printHeader("DELETE FLIGHT");
        
        if (flights.empty()) {
            Flight::loadFlights();
        }

        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            while (true) {
                clearScreen();
                printSubHeader("Available Flights");
                
                vector<pair<string, int>> columns = {
                    {"Flight ID", 15},
                    {"Airline", 25},
                    {"Destination", 25},
                    {"Departure Time", 30},
                    {"Arrival Time", 25}
                };
                
                printTableHeader(columns);
                
                for (const auto& flight : flights) {
                    vector<pair<string, int>> row = {
                        {flight.getFlightID(), 15},
                        {flight.getAirlineName(), 24},
                        {flight.getDestination(), 23},
                        {flight.getDepartureTime(), 28},
                        {flight.getArrivalTime(), 25}
                    };
                    
                    printTableRow(row);
                }

                string airlineName;
                bool validAirline = false;
                
                while (!validAirline) {
                    printPrompt("\nEnter airline name (or 'b' to go back):");
                    getline(cin, airlineName);

                    if (airlineName == "b" || airlineName == "B") {
                        return;
                    }
                    
                    vector<Flight> airlineFlights;
                    for (const auto& flight : flights) {
                        if (equalsIgnoreCase(flight.getAirlineName(), airlineName)) {
                            airlineFlights.push_back(flight);
                        }
                    }
                    
                    if (airlineFlights.empty()) {
                        printErrorMessage("No flights found for airline: " + airlineName + ". Please try again.");
                    } else {
                        validAirline = true;
                        clearScreen();
                        printSubHeader("Available flights for " + airlineName);
                        
                        vector<pair<string, int>> columns = {
                            {"Flight ID", 15},
                            {"Destination", 25},
                            {"Departure Time", 30},
                            {"Arrival Time", 25}
                        };
                        
                        printTableHeader(columns);
                        
                        for (const auto& flight : airlineFlights) {
                            vector<pair<string, int>> row = {
                                {flight.getFlightID(), 15},
                                {flight.getDestination(), 20},
                                {flight.getDepartureTime(), 28},
                                {flight.getArrivalTime(), 25}
                            };
                            
                            printTableRow(row);
                        }
                        
                        string flightID;
                        bool validFlightID = false;
                        
                        while (!validFlightID) {
                            printPrompt("\nEnter Flight ID to delete (or 'b' to go back):");
                            getline(cin, flightID);
                            
                            if (flightID.empty()) {
                                printErrorMessage("Flight ID cannot be empty. Please try again.");
                                continue;
                            } else if (flightID == "b" || flightID == "B") {
                                return;
                            }
                            
                            auto it = find_if(flights.begin(), flights.end(), 
                                             [&flightID](const Flight& f) { 
                                                 return equalsIgnoreCase(f.getFlightID(), flightID); 
                                             });
                            
                            if (it == flights.end()) {
                                printErrorMessage("Flight not found. Please try again.");
                            } else {
                                validFlightID = true;
                                
                                char confirm = getYesNoInput("\nConfirm delete (y/n):");
                                
                                if (confirm == 'y') {
                                    string actualFlightID = it->getFlightID();
                                    
                                    reservations.erase(
                                        remove_if(reservations.begin(), reservations.end(),
                                                 [&actualFlightID](const Reservation& r) { 
                                                     return equalsIgnoreCase(r.getFlightID(), actualFlightID); 
                                                 }),
                                        reservations.end());
                                    
                                    if (waitingLists.find(actualFlightID) != waitingLists.end()) {
                                        WaitingList& waitingList = waitingLists[actualFlightID];
                                        waitingList.saveToFile();
                                    }
                                    
                                    DatabaseManager* dbManager = DatabaseManager::getInstance();
                                    dbManager->deleteFile("seatmaps/" + actualFlightID + ".txt");
                                    
                                    dbManager->deleteFile("waitinglists/" + actualFlightID + ".txt");
                                    
                                    flights.erase(it);
                                    
                                    Flight::saveAllFlights();
                                    Reservation::saveAllReservations();
                                    WaitingList::saveAllWaitingLists();
                                    
                                    printSuccessMessage("Flight deleted successfully!");
                                } else {
                                    printInfoMessage("Deletion cancelled.");
                                }
                            }
                        }
                    }
                }
                pressEnterToContinue();
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void manageReservations() {
        clearScreen();
        printHeader("RESERVATIONS");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 15},
                {"Airline", 25},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 15},
                    {flights[i].getAirlineName(), 24},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex = getValidIntegerInput("\nEnter flight number to view reservations:", 0, flights.size());

            if (flightIndex == 0){
                return;
            }
            
            string flightID = flights[flightIndex - 1].getFlightID();
            
            clearScreen();
            printHeader("RESERVATIONS FOR FLIGHT " + flightID);
            
            vector<Reservation> flightReservations;
            for (const auto& reservation : reservations) {
                if (equalsIgnoreCase(reservation.getFlightID(), flightID)) {
                    flightReservations.push_back(reservation);
                }
            }
            
            if (flightReservations.empty()) {
                printInfoMessage("No reservations found for this flight.");
                pressEnterToContinue();
                return;
            }
            
            printSubHeader("Reservations");
            
            vector<pair<string, int>> resColumns = {
                {"Reservation ID", 20},
                {"Passenger Name", 20},
                {"Flight Number", 20},
                {"Airline", 25},
                {"Destination", 25},
                {"Seat Number", 18},
                {"Status", 15}
            };
            
            printTableHeader(resColumns);
            
            for (const auto& reservation : flightReservations) {
                vector<pair<string, int>> row = {
                    {reservation.getReservationID(), 22},
                    {reservation.getPassengerName(), 20},
                    {reservation.getFlightID(), 17},
                    {reservation.getAirlineName(), 25},
                    {reservation.getDestination(), 29},
                    {reservation.getSeatNumber(), 13.5},
                    {reservation.getStatus(), 15}
                };
            
                printTableRow(row);
            }
        
            char deleteOption = getYesNoInput("\nDo you want to delete a reservation? (y/n): ");

            if (deleteOption == 'y') {
                string reservationID;
                bool validReservationID = false;
            
                while (!validReservationID) {
                    printPrompt("Enter Reservation ID to delete (or 'b' to go back):");
                    getline(cin, reservationID);
                
                    if (reservationID.empty()) {
                        printErrorMessage("Reservation ID cannot be empty. Please try again.");
                        continue;
                    } else if (reservationID == "b" || reservationID == "B") {
                        return;
                    }
                
                    auto it = find_if(reservations.begin(), reservations.end(),
                                     [&reservationID](const Reservation& r) { 
                                         return equalsIgnoreCase(r.getReservationID(), reservationID); 
                                     });
                
                    if (it == reservations.end()) {
                        printErrorMessage("Reservation not found. Please try again.");
                    } else {
                        validReservationID = true;
                    
                        char confirm = getYesNoInput("\nConfirm delete (y/n):");
                    
                        if (confirm == 'y') {
                            for (auto& flight : flights) {
                                if (equalsIgnoreCase(flight.getFlightID(), it->getFlightID())) {
                                    flight.cancelSeat(it->getSeatNumber());
                                    break;
                                }
                            }
                        
                            reservations.erase(it);
                        
                            Flight::saveAllFlights();
                            Reservation::saveAllReservations();
                        
                            printSuccessMessage("Reservation deleted successfully!");
                        } else {
                            printInfoMessage("Deletion cancelled.");
                        }
                    }
                }
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
    
        pressEnterToContinue();
    }

    void manageFlightStatus() {
        clearScreen();
        printHeader("FLIGHT STATUS");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 15},
                {"Airline", 26.5},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 15},
                    {flights[i].getAirlineName(), 25},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex = getValidIntegerInput("\nEnter flight number to view status:", 0, flights.size());

            if(flightIndex == 0){
                return;
            }
            
            Flight& flight = flights[flightIndex - 1];
            
            clearScreen();
            printHeader("FLIGHT STATUS");
            
            cout << "  Flight Number: " << flight.getFlightID() << "\n";
            cout << "  Airline: " << flight.getAirlineName() << "\n";
            cout << "  Departure Time: " << flight.getDepartureTime() << "\n";
            cout << "  Arrival Time: " << flight.getArrivalTime() << "\n";
            cout << "  Status: " << flight.getStatus() << "\n";
            
            char editOption = getYesNoInput("\nDo you want to edit the flight? (y/n):");

            if (editOption == 'y') {
                string airline, departureTime, arrivalTime, status;
                
                printPrompt("\nEnter Airline (current: " + flight.getAirlineName() + "):");
                getline(cin, airline);
                if (airline.empty()) airline = flight.getAirlineName();
                
                printPrompt("Enter Departure Time (current: " + flight.getDepartureTime() + "):");
                getline(cin, departureTime);
                if (departureTime.empty()) departureTime = flight.getDepartureTime();
                
                printPrompt("Enter Arrival Time (current: " + flight.getArrivalTime() + "):");
                getline(cin, arrivalTime);
                if (arrivalTime.empty()) arrivalTime = flight.getArrivalTime();
                
                printPrompt("Enter Flight Status (current: " + flight.getStatus() + "):");
                getline(cin, status);
                if (status.empty()) status = flight.getStatus();
                
                char confirm = getYesNoInput("\nConfirm changes? (y/n):");
                
                if (confirm == 'y') {
                    flight.setAirlineName(airline);
                    flight.setDepartureTime(departureTime);
                    flight.setArrivalTime(arrivalTime);
                    flight.setStatus(status);
                    
                    Flight::saveAllFlights();
                    
                    printSuccessMessage("Flight information updated successfully!");
                } else {
                    printInfoMessage("Changes cancelled.");
                }
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void viewSeatMaps() {
        clearScreen();
        printHeader("VIEW SEAT MAPS");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 15},
                {"Airline", 26.5},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 15},
                    {flights[i].getAirlineName(), 25},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex = getValidIntegerInput("\nEnter flight number to view seat map:", 0, flights.size());

            if(flightIndex == 0){
                return;
            }
            
            Flight& flight = flights[flightIndex - 1];
            
            clearScreen();
            flight.displaySeatMap();
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void manageWaitingList() {
        clearScreen();
        printHeader("MANAGE WAITING LIST");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 15},
                {"Airline", 26.5},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 15},
                    {flights[i].getAirlineName(), 25},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex = getValidIntegerInput("\nEnter flight number to manage waiting list:", 0, flights.size());

            if(flightIndex == 0){
                return;
            }
            
            Flight& flight = flights[flightIndex - 1];
            string flightID = flight.getFlightID();
            
            if (waitingLists.find(flightID) == waitingLists.end()) {
                waitingLists[flightID] = WaitingList(flightID);
            }
            
            WaitingList& waitingList = waitingLists[flightID];
            
            clearScreen();
            waitingList.display();
            
            if (waitingList.isEmpty()) {
                pressEnterToContinue();
                return;
            }
            
            printSubHeader("Options");
            printMenuOption(1, "Promote passenger");
            printMenuOption(2, "Delete passenger");
            printMenuOption(3, "Return to menu");
            
            int choice = getValidIntegerInput("Enter your choice:", 1, 3);
            
            switch (choice) {
                case 1: {
                    if (flight.isFullyBooked()) {
                        throw BookingException("Flight is fully booked. Cannot promote passenger.");
                    }
                    
                    pair<string, string> nextPassenger = waitingList.getNextPassenger();
                    if (nextPassenger.first.empty()) {
                        throw ValidationException("No passengers in the waiting list");
                    }
                    
                    flight.displaySeatMap();
                    
                    string seatNumber;
                    bool validSeatNumber = false;
                    
                    while (!validSeatNumber) {
                        printPrompt("\nEnter seat number for the passenger:");
                        getline(cin, seatNumber);
                        
                        if (seatNumber.empty()) {
                            printErrorMessage("Seat number cannot be empty. Please try again.");
                            continue;
                        }
                        
                        if (!flight.isSeatAvailable(seatNumber)) {
                            printErrorMessage("Seat is not available. Please choose another seat.");
                            continue;
                        }
                        
                        validSeatNumber = true;
                    }
                    
                    char confirm = getYesNoInput("\nConfirm changes? (y/n):");
                    
                    if (confirm == 'y') {
                        flight.bookSeat(seatNumber);
                        
                        Reservation reservation(nextPassenger.second, flightID, flight.getAirlineName(), 
                                               flight.getDestination(), seatNumber, nextPassenger.first);
                        reservations.push_back(reservation);
                        
                        waitingList.removePassenger(nextPassenger.first);
                        
                        Flight::saveAllFlights();
                        Reservation::saveAllReservations();
                        waitingList.saveToFile();
                        
                        printSuccessMessage("Passenger promoted successfully!");
                    } else {
                        printInfoMessage("Promotion cancelled.");
                    }
                    break;
                }
                case 2: {
                    string username;
                    bool validUsername = false;
                    
                    while (!validUsername) {
                        printPrompt("\nEnter username of passenger to delete:");
                        getline(cin, username);
                        
                        if (username.empty()) {
                            printErrorMessage("Username cannot be empty. Please try again.");
                            continue;
                        }
                        
                        validUsername = true;
                    }
                    
                    char confirm = getYesNoInput("\nConfirm deletion? (y/n):");
                    
                    if (confirm == 'y') {
                        if (waitingList.removePassenger(username)) {
                            waitingList.saveToFile();
                            printSuccessMessage("Passenger removed from waiting list successfully!");
                        } else {
                            printErrorMessage("Passenger not found in waiting list. Please check the username and try again.");
                        }
                    } else {
                        printInfoMessage("Deletion cancelled.");
                    }
                    break;
                }
                case 3:
                    return;
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void manageUserAccounts() {
        clearScreen();
        printHeader("USER ACCOUNTS");
        
        try {
            printSubHeader("Customer Accounts");
            
            vector<User*> customers;
            for (auto& user : users) {
                if (!user->getIsAdmin()) {
                    customers.push_back(user);
                }
            }
            
            if (customers.empty()) {
                printInfoMessage("No customer accounts found.");
                pressEnterToContinue();
                return;
            }
            
            vector<pair<string, int>> columns = {
                {"Username", 20},
                {"Name", 30}
            };
            
            printTableHeader(columns);
            
            for (auto& user : customers) {
                vector<pair<string, int>> row = {
                    {user->getUsername(), 20},
                    {user->getName(), 30}
                };
                
                printTableRow(row);
            }
            
            char deleteOption = getYesNoInput("\nDo you want to delete a user account? (y/n):");
            
            if (deleteOption == 'y') {
                string username;
                bool validUsername = false;
                
                while (!validUsername) {
                    printPrompt("Enter username of account to delete:");
                    getline(cin, username);
                    
                    if (username.empty()) {
                        printErrorMessage("Username cannot be empty. Please try again.");
                    } else {
                        auto it = find_if(users.begin(), users.end(),
                                         [&username](const User* u) { 
                                             return u->getUsername() == username && !u->getIsAdmin(); 
                                         });
                    
                        if (it == users.end()) {
                            printErrorMessage("Customer account not found. Please try again.");
                        } else {
                            validUsername = true;
                        
                            char confirm = getYesNoInput("\nConfirm delete (y/n):");
                        
                            if (confirm == 'y') {
                                string actualUsername = (*it)->getUsername();
                            
                                reservations.erase(
                                    remove_if(reservations.begin(), reservations.end(),
                                             [&actualUsername](const Reservation& r) { 
                                                 return r.getUsername() == actualUsername; 
                                             }),
                                    reservations.end());
                            
                                for (auto& pair : waitingLists) {
                                    pair.second.removePassenger(actualUsername);
                                }
                            
                                delete *it;
                                users.erase(it);
                            
                                User::saveAllUsers();
                                Reservation::saveAllReservations();
                                WaitingList::saveAllWaitingLists();
                            
                                printSuccessMessage("User account deleted successfully!");
                            } else {
                                printInfoMessage("Deletion cancelled.");
                            }
                        }
                    }
                }
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }
};

class Customer : public User {
public:
    Customer() {
        setIsAdmin(false);
    }

    Customer(const string& username, const string& password, const string& name)
        : User(username, password, name, false) {}

    void displayMenu() override {
        int choice;
        do {
            clearScreen();
            printHeader("CUSTOMER DASHBOARD");
            
            cout << "  Welcome, " << getName() << "!" << endl;
            cout << "  " << getCurrentDateTime() << endl;
            
            printSeparator();
            
            printMenuOption(1, "View Flights");
            printMenuOption(2, "View Booking");
            printMenuOption(3, "Cancel Booking");
            printMenuOption(4, "Logout");
            
            choice = getValidIntegerInput("Enter your choice:", 1, 4);
            
            switch (choice) {
                case 1:
                    viewFlights();
                    break;
                case 2:
                    viewBooking();
                    break;
                case 3:
                    cancelBooking();
                    break;
                case 4:
                    printInfoMessage("Logging out...");
                    break;
            }
        } while (choice != 4);
    }

    void viewFlights() {
        clearScreen();
        printHeader("VIEW FLIGHTS");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"Flight ID", 15},
                {"Airline", 20},
                {"Destination", 30},
                {"Departure Time", 30},
                {"Arrival Time", 25},
                {"Available Seats", 15}
            };
            
            printTableHeader(columns);
            
            for (const auto& flight : flights) {
                vector<pair<string, int>> row = {
                    {flight.getFlightID(), 15},
                    {flight.getAirlineName(), 20},
                    {flight.getDestination(), 25},
                    {flight.getDepartureTime(), 30},
                    {flight.getArrivalTime(), 35},
                    {to_string(flight.getAvailableSeats()), 15}
                };
                
                printTableRow(row);
            }
            
            char bookOption = getYesNoInput("\nDo you want to book a flight? (y/n): ");
        
            if (bookOption == 'y') {
                bookFlight();
            } else {
                pressEnterToContinue();
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
            pressEnterToContinue();
        }
    }

    void bookFlight() {
        printSubHeader("BOOK FLIGHT");
        
        if (flights.empty()) {
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        try {
            string destination;
            bool validDestination = false;
            
            while (!validDestination) {
                printPrompt("Enter your destination or 'b' to go back:");
                getline(cin, destination);
                
                if(destination == "b" || destination == "B"){
                    return;
                }
                
                if (destination.empty()) {
                    printErrorMessage("Destination cannot be empty. Please try again.");
                } else {
                    validDestination = true;
                }
            }

            clearScreen();
            
            vector<Flight*> matchingFlights;
            
            for (auto& flight : flights) {
                if (containsIgnoreCase(flight.getDestination(), destination)) {
                    matchingFlights.push_back(&flight);
                }
            }
            
            if (matchingFlights.empty()) {
                throw ValidationException("No flights found for destination: " + destination);
            }
            
            printSubHeader("Flights for destination " + destination);
            
            vector<pair<string, int>> columns = {
                {"No.", 5},
                {"Flight ID", 10},
                {"Airline", 20},
                {"Departure Time", 25},
                {"Arrival Time", 25},
                {"Available Seats", 15}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < matchingFlights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 5},
                    {matchingFlights[i]->getFlightID(), 10},
                    {matchingFlights[i]->getAirlineName(), 20},
                    {matchingFlights[i]->getDepartureTime(), 25},
                    {matchingFlights[i]->getArrivalTime(), 25},
                    {to_string(matchingFlights[i]->getAvailableSeats()), 15}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex = getValidIntegerInput("\nChoose flight. Enter flight number:", 0, matchingFlights.size());

            if(flightIndex == 0){
                return;
            }
            
            Flight* selectedFlight = matchingFlights[flightIndex - 1];
            
            if (selectedFlight->isFullyBooked()) {
                printWarningMessage("This flight is fully booked.");
                
                char waitingListOption = getYesNoInput("Do you want to be added to the waiting list? (y/n):");
                
                if (waitingListOption == 'y') {
                    if (waitingLists.find(selectedFlight->getFlightID()) == waitingLists.end()) {
                        waitingLists[selectedFlight->getFlightID()] = WaitingList(selectedFlight->getFlightID());
                    }
                    
                    waitingLists[selectedFlight->getFlightID()].addPassenger(getUsername(), getName());
                    waitingLists[selectedFlight->getFlightID()].saveToFile();
                    
                    printSuccessMessage("You have been added to the waiting list for this flight.");
                }
                
                pressEnterToContinue();
                return;
            }
            
            selectedFlight->displaySeatMap();
            
            string seatNumber;
            bool validSeatNumber = false;
            
            while (!validSeatNumber) {
                printPrompt("\nEnter seat number (e.g., 1A) or 'b' to go back:");
                getline(cin, seatNumber);
                
                if(seatNumber == "B" || seatNumber == "b"){
                    return;
                }
                
                if (seatNumber.empty()) {
                    printErrorMessage("Seat number cannot be empty. Please try again.");
                    continue;
                }
                
                if (!selectedFlight->isSeatAvailable(seatNumber)) {
                    printErrorMessage("Seat is not available. Please choose another seat.");
                    continue;
                }
                
                validSeatNumber = true;
            }
            
            printSubHeader("Payment Method");
            printMenuOption(1, "GCash");
            printMenuOption(2, "Card");
            printBackOption();
            
            int paymentMethod = getValidIntegerInput("Enter your choice:", 0, 2);
            
            if(paymentMethod == 0){
                return;
            }
            
            unique_ptr<PaymentStrategy> paymentStrategy;
            string paymentDetails;
            
            if (paymentMethod == 1) {
                string gcashNumber;
                bool validGcashNumber = false;
                
                while (!validGcashNumber) {
                    printPrompt("\nEnter GCash number:");
                    getline(cin, gcashNumber);
                    
                    if (gcashNumber.empty()) {
                        printErrorMessage("GCash number cannot be empty. Please try again.");
                    } else {
                        validGcashNumber = true;
                    }
                }
                
                paymentStrategy = make_unique<GCashPaymentStrategy>(gcashNumber);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else if (paymentMethod == 2) {
                string cardNumber, expiryDate, cvv;
                bool validCardNumber = false, validExpiryDate = false, validCVV = false;
                
                while (!validCardNumber) {
                    printPrompt("\nEnter Card number:");
                    getline(cin, cardNumber);
                    
                    if (cardNumber.empty()) {
                        printErrorMessage("Card number cannot be empty. Please try again.");
                    } else {
                        validCardNumber = true;
                    }
                }
                
                while (!validExpiryDate) {
                    printPrompt("Enter expiration date (MM/YY):");
                    getline(cin, expiryDate);
                    
                    if (expiryDate.empty()) {
                        printErrorMessage("Expiration date cannot be empty. Please try again.");
                    } else {
                        validExpiryDate = true;
                    }
                }
                
                while (!validCVV) {
                    printPrompt("Enter CVV:");
                    getline(cin, cvv);
                    
                    if (cvv.empty()) {
                        printErrorMessage("CVV cannot be empty. Please try again.");
                    } else {
                        validCVV = true;
                    }
                }
                
                paymentStrategy = make_unique<CreditCardPaymentStrategy>(cardNumber, expiryDate, cvv);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else {
                throw ValidationException("Invalid payment method");
            }
            
            double flightPrice = 500.00;
            
            clearScreen();
            printSubHeader("Payment Summary");
            
            cout << "  Flight: " << selectedFlight->getFlightID() << " - " << selectedFlight->getAirlineName() << endl;
            cout << "  Destination: " << selectedFlight->getDestination() << endl;
            cout << "  Seat: " << seatNumber << endl;
            cout << "  Payment Method: " << paymentDetails << endl;
            cout << "  Amount: $" << fixed << setprecision(2) << flightPrice << endl;
            
            char confirm = getYesNoInput("\nConfirm payment? (y/n):");
            
            bool paymentConfirmed = false;
            if (confirm == 'y') {
                paymentConfirmed = paymentStrategy->processPayment(flightPrice);
            }
            
            if (paymentConfirmed) {
                selectedFlight->bookSeat(seatNumber);
                
                Reservation reservation(getName(), selectedFlight->getFlightID(), selectedFlight->getAirlineName(), 
                                       selectedFlight->getDestination(), seatNumber, getUsername(), paymentDetails);
                reservations.push_back(reservation);
                
                Flight::saveAllFlights();
                reservation.saveToFile();
                
                printSuccessMessage("Payment successful! Your flight has been booked.");
                
                clearScreen();
                printHeader("BOARDING PASS");

                const int fixedWidth = 70;

                cout << "  +-" << string(fixedWidth, '-') << "-+" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << "   " + selectedFlight->getAirlineName() + " Airlines" << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  PASSENGER: " << setw(fixedWidth - 12) << left << getName() << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  FLIGHT: " << setw(15) << left << selectedFlight->getFlightID() 
                     << "DATE: " << setw(fixedWidth - 30) << left << selectedFlight->getDepartureTime().substr(0, selectedFlight->getDepartureTime().find(" - ")) << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  FROM/TO: " << setw(fixedWidth - 10) << left << selectedFlight->getDestination() << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  SEAT: " << setw(fixedWidth - 7) << left << seatNumber << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  BOARDING TIME: " << setw(fixedWidth - 16) << left << selectedFlight->getDepartureTime().substr(selectedFlight->getDepartureTime().find(" - ") + 3) << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  |  " << setw(fixedWidth - 1) << left << "Thank you for choosing " + selectedFlight->getAirlineName() + "!" << " |" << endl;
                cout << "  | " << setw(fixedWidth) << left << " " << " |" << endl;
                cout << "  +-" << string(fixedWidth, '-') << "-+" << endl;
                
            } else {
                printInfoMessage("Payment cancelled. Booking not completed.");
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void viewBooking() {
        clearScreen();
        printHeader("VIEW BOOKING");
        
        try {
            vector<Reservation> customerReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getUsername() == getUsername()) {
                    customerReservations.push_back(reservation);
                }
            }
            
            if (customerReservations.empty()) {
                printInfoMessage("You have no bookings.");
                pressEnterToContinue();
                return;
            }
            
            printSubHeader("Your Bookings");
            
            vector<pair<string, int>> columns = {
                {"Reservation ID", 20},
                {"Flight ID", 17},
                {"Airline", 26},
                {"Destination", 25},
                {"Seat Number", 20},
                {"Status", 15}
            };
            
            printTableHeader(columns);
            
            for (const auto& reservation : customerReservations) {
                vector<pair<string, int>> row = {
                    {reservation.getReservationID(), 21},
                    {reservation.getFlightID(), 16},
                    {reservation.getAirlineName(), 25},
                    {reservation.getDestination(), 30},
                    {reservation.getSeatNumber(), 15},
                    {reservation.getStatus(), 15}
                };
                
                printTableRow(row);
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }

    void cancelBooking() {
        clearScreen();
        printHeader("CANCEL BOOKING");
        
        try {
            vector<Reservation> customerReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getUsername() == getUsername()) {
                    customerReservations.push_back(reservation);
                }
            }
            
            if (customerReservations.empty()) {
                printInfoMessage("You have no bookings to cancel.");
                pressEnterToContinue();
                return;
            }
            
            printSubHeader("Your Bookings");
            
            vector<pair<string, int>> columns = {
                {"No.", 5},
                {"Reservation ID", 20},
                {"Flight ID", 15},
                {"Airline", 25},
                {"Destination", 26.5},
                {"Seat Number", 15}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < customerReservations.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 5},
                    {customerReservations[i].getReservationID(), 21},
                    {customerReservations[i].getFlightID(), 15},
                    {customerReservations[i].getAirlineName(), 25},
                    {customerReservations[i].getDestination(), 30},
                    {customerReservations[i].getSeatNumber(), 15}
                };
                
                printTableRow(row);
            }
            
            printBackOption();
            
            int bookingIndex = getValidIntegerInput("\nEnter booking number to cancel:", 0, customerReservations.size());

            if(bookingIndex == 0){
                return;
            }
            
            Reservation& selectedReservation = customerReservations[bookingIndex - 1];
            
            clearScreen();
            printSubHeader("Cancellation Confirmation");
            
            cout << "  Reservation ID: " << selectedReservation.getReservationID() << endl;
            cout << "  Flight: " << selectedReservation.getFlightID() << " - " << selectedReservation.getAirlineName() << endl;
            cout << "  Destination: " << selectedReservation.getDestination() << endl;
            cout << "  Seat: " << selectedReservation.getSeatNumber() << endl;
            
            char confirm = getYesNoInput("\nConfirm cancellation? (y/n):");
            
            if (confirm == 'y') {
                for (auto& flight : flights) {
                    if (equalsIgnoreCase(flight.getFlightID(), selectedReservation.getFlightID())) {
                        flight.cancelSeat(selectedReservation.getSeatNumber());
                        break;
                    }
                }
                
                auto it = find_if(reservations.begin(), reservations.end(),
                                 [&selectedReservation](const Reservation& r) { 
                                     return equalsIgnoreCase(r.getReservationID(), selectedReservation.getReservationID()); 
                                 });
                
                if (it != reservations.end()) {
                    reservations.erase(it);
                }
                
                Flight::saveAllFlights();
                Reservation::saveAllReservations();
                
                printSuccessMessage("Booking has been successfully cancelled.");
            } else {
                printInfoMessage("Cancellation cancelled.");
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }
};

void User::loadUsers() {
    try {
        for (auto user : users) {
            delete user;
        }
        users.clear();
        
        DatabaseManager* dbManager = DatabaseManager::getInstance();
        string data = dbManager->loadData("users.txt");
        
        stringstream dataStream(data);
        string line;
        
        while (getline(dataStream, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 4) {
                string username = tokens[0];
                string password = tokens[1];
                string name = tokens[2];
                bool isAdmin = (tokens[3] == "admin");
                
                User* user;
                if (isAdmin) {
                    user = new Admin(username, password, name);
                } else {
                    user = new Customer(username, password, name);
                }
                
                users.push_back(user);
            }
        }
    } catch (const exception& e) {
        printErrorMessage("Error loading users: " + string(e.what()));
    }
}

void initializeSystem() {
    try {
        createDirectory("seatmaps");
        createDirectory("waitinglists");
        
        Flight::loadFlights();
        User::loadUsers();
        Reservation::loadReservations();
        WaitingList::loadWaitingLists();
    } catch (const exception& e) {
        printErrorMessage("Error initializing system: " + string(e.what()));
        exit(1);
    }
}

void signUp() {
    clearScreen();
    printHeader("SIGN UP");

    try {
        printSubHeader("Account Type");
        printMenuOption(1, "Admin");
        printMenuOption(2, "Customer");
        printMenuOption(3, "Back to Main Menu");
        
        int userType = getValidIntegerInput("Enter your choice:", 1, 3);

        if (userType == 3) {
            return;
        }

        string username, password, confirmPassword, name;
        bool validUsername = false, validPassword = false, validConfirmPassword = false, validName = false;

        while (!validUsername) {
            printPrompt("\nEnter username (or 'b' to go back):");
            getline(cin, username);
            
            if (username.empty()) {
                printErrorMessage("Username cannot be empty. Please try again.");
            } else if (User::usernameExists(username)) {
                printErrorMessage("Username already exists. Please choose another one.");
            } else if (username == "b" || username == "B"){
                return;
            } else {
                validUsername = true;
            }
        }

        while (!validPassword) {
            printPrompt("Enter password:");
            getline(cin, password);
            
            if (password.empty()) {
                printErrorMessage("Password cannot be empty. Please try again.");
            } else {
                validPassword = true;
            }
        }

        while (!validConfirmPassword) {
            printPrompt("Confirm password:");
            getline(cin, confirmPassword);
            
            if (password != confirmPassword) {
                printErrorMessage("Passwords do not match. Please try again.");
            } else {
                validConfirmPassword = true;
            }
        }

        while (!validName) {
            printPrompt("Enter your full name:");
            getline(cin, name);
            
            if (name.empty()) {
                printErrorMessage("Name cannot be empty. Please try again.");
            } else {
                validName = true;
            }
        }

        User* newUser;
        if (userType == 1) {
            newUser = new Admin(username, password, name);
        } else {
            newUser = new Customer(username, password, name);
        }

        users.push_back(newUser);
        newUser->saveToFile();

        printSuccessMessage("Sign up successful! You can now log in.");
    } catch (const exception& e) {
        printErrorMessage(e.what());
    }

    pressEnterToContinue();
}

void logIn() {
    clearScreen();
    printHeader("LOG IN");

    try {
        printSubHeader("Account Type");
        printMenuOption(1, "Admin");
        printMenuOption(2, "Customer");
        printMenuOption(3, "Back to Main Menu");
        
        int userType = getValidIntegerInput("Enter your choice:", 1, 3);

        if (userType == 3) {
            return;
        }

        string username, password;
        bool validCredentials = false;

        while (!validCredentials) {
            printPrompt("\nEnter username (or 'b' to go back):");
            getline(cin, username);
            
            if (username.empty()) {
                printErrorMessage("Username cannot be empty. Please try again.");
                continue;
            } else if (username == "b" || username == "B"){
                return;
            }
            
            printPrompt("Enter password:");
            getline(cin, password);
            
            if (password.empty()) {
                printErrorMessage("Password cannot be empty. Please try again.");
                continue;
            }
            
            User* user = User::login(username, password);
            if (user == nullptr) {
                printErrorMessage("Invalid username or password. Please try again.");
                continue;
            }
            
            bool isAdmin = user->getIsAdmin();
            if ((userType == 1 && !isAdmin) || (userType == 2 && isAdmin)) {
                printErrorMessage("Invalid user type for this account. Please try again.");
                continue;
            }
            
            validCredentials = true;
            
            printSuccessMessage("Login successful! Welcome, " + user->getName() + "!");
            pressEnterToContinue();
            
            currentUser = user;
            currentUser->displayMenu();
            currentUser = nullptr;
        }
    } catch (const exception& e) {
        printErrorMessage(e.what());
        pressEnterToContinue();
    }
}

int main() {
    initializeSystem();

    #ifdef _WIN32
        system("chcp 65001 > nul");
        system("title Airline Reservation System");
    #endif

    int choice;
    do {
        clearScreen();
        
        printHeader("AIRLINE RESERVATION SYSTEM");
        
        cout << "  Welcome to the Airline Reservation System!" << endl;
        cout << "  " << getCurrentDateTime() << endl;
        
        printSeparator();
        
        printMenuOption(1, "Sign Up");
        printMenuOption(2, "Log In");
        printMenuOption(3, "Exit");
        
        choice = getValidIntegerInput("Enter your choice:", 1, 3);
        
        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                logIn();
                break;
            case 3:
                printInfoMessage("Thank you for using the Airline Reservation System. Goodbye!\n");
                break;
        }
    } while (choice != 3);

    for (auto user : users) {
        delete user;
    }

    delete DatabaseManager::getInstance();

    return 0;
}
