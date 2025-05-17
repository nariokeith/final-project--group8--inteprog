#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <cmath>
#ifdef _WIN32
    #include <direct.h> // For Windows mkdir
    #include <io.h>     // For _access on Windows
    #define MKDIR(dir) _mkdir(dir)
    #define FILE_EXISTS(file) (_access(file, 0) != -1)
#else
    #include <sys/stat.h> // For POSIX mkdir
    #include <sys/types.h>
    #include <unistd.h>   // For access on Unix-like systems
    #define MKDIR(dir) mkdir(dir, 0755)
    #define FILE_EXISTS(file) (access(file, F_OK) != -1)
#endif

using namespace std;

// UI Helper Functions
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

// Exception classes for better error handling
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

// Singleton Pattern: Database Manager
class DatabaseManager {
private:
    // Private constructor for Singleton pattern
    DatabaseManager() {}
    
    // Static instance for Singleton pattern
    static DatabaseManager* instance;
    
    // Private copy constructor and assignment operator to prevent duplication
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
    // Public method to access the singleton instance
    static DatabaseManager* getInstance() {
        if (instance == nullptr) {
            instance = new DatabaseManager();
        }
        return instance;
    }
    
    // Database operations
    bool saveData(const string& filename, const string& data) {
        try {
            // If data is empty, delete the file instead of saving empty content
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
            // If data is empty, delete the file instead of saving empty content
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
                // Don't throw an exception for non-existent files, just return empty string
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
            // Check if file exists before attempting to delete
            if (!fileExists(filename)) {
                // File doesn't exist, consider deletion successful
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

// Initialize the static instance to nullptr
DatabaseManager* DatabaseManager::instance = nullptr;

// Strategy Pattern: Payment Strategy
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
        // Simulate GCash payment processing
        printInfoMessage("Processing GCash payment of $" + to_string(amount) + 
                        " using number " + gcashNumber + "...");
        
        // In a real system, this would connect to the GCash API
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
        // Simulate credit card payment processing
        printInfoMessage("Processing Credit Card payment of $" + to_string(amount) + 
                        " using card ending with " + cardNumber.substr(cardNumber.length() - 4) + "...");
        
        // In a real system, this would connect to a payment gateway
        return true;
    }
    
    string getPaymentDetails() override {
        return "Credit Card: XXXX-XXXX-XXXX-" + cardNumber.substr(cardNumber.length() - 4);
    }
};

// Global variables
vector<class Flight> flights;
vector<class User*> users;
vector<class Reservation> reservations;
map<string, class WaitingList> waitingLists; // Flight ID -> WaitingList
class User* currentUser = nullptr;

// Utility functions
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Cross-platform "press any key to continue" function
void pressEnterToContinue() {
    cout << "\nPress Enter to continue..." << flush;
    
    #ifdef _WIN32
        // Windows-specific pause
        system("pause > nul");  // The "> nul" suppresses the system message
    #else
        // Mac/Linux compatible pause
        cout << flush;  // Ensure the message is displayed
        // Use system-level read for a single character without needing Enter
        system("read -n 1 -s");
    #endif
}

// Cross-platform directory creation function
bool createDirectory(const string& path) {
    int result = MKDIR(path.c_str());
    // Return true if directory was created or already exists
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
    
    // Format time as HH:MM AM/PM
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

// Modified generateID function to ensure IDs are always unique across program restarts
string generateID(const string& prefix) {
    static map<string, int> counters;
    
    // If counter for this prefix hasn't been initialized yet
    if (counters.find(prefix) == counters.end()) {
        // Initialize with a default value
        counters[prefix] = 10000;
        
        // Read existing IDs from files to find the highest current ID
        DatabaseManager* dbManager = DatabaseManager::getInstance();
        string data;
        int highestID = 10000;
        
        if (prefix == "FL") {
            // For Flight IDs
            data = dbManager->loadData("flights.txt");
        } else if (prefix == "RES") {
            // For Reservation IDs
            data = dbManager->loadData("reservations.txt");
        } else if (prefix == "USR") {
            // For User IDs (if applicable)
            data = dbManager->loadData("users.txt");
        }
        
        // Parse the data to find the highest ID
        stringstream dataStream(data);
        string line;
        
        while (getline(dataStream, line)) {
            stringstream ss(line);
            string token;
            
            // Get the first token which should be the ID
            if (getline(ss, token, ',')) {
                // Check if this token starts with our prefix
                if (token.substr(0, prefix.length()) == prefix) {
                    // Extract the numeric part
                    string numPart = token.substr(prefix.length());
                    try {
                        int idNum = stoi(numPart);
                        if (idNum > highestID) {
                            highestID = idNum;
                        }
                    } catch (const exception& e) {
                        // Skip invalid ID formats
                        continue;
                    }
                }
            }
        }
        
        // Set the counter to the highest found ID
        counters[prefix] = highestID;
    }
    
    // Increment the counter and return the new ID
    return prefix + to_string(++counters[prefix]);
}

// Class definitions
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
    vector<vector<bool>> seatMap; // true if occupied, false if available
    int seatsPerRow; // Number of seats per row (excluding aisle)
    int totalColumns; // Total columns including aisle

public:
    Flight() : seatsPerRow(3), totalColumns(7) {} // Default constructor with typical values
    
    Flight(const string& airlineName, const string& planeID, int capacity, 
           const string& destination, const string& departureTime, 
           const string& arrivalTime) 
        : airlineName(airlineName), planeID(planeID), capacity(capacity), 
          availableSeats(capacity), destination(destination), 
          departureTime(departureTime), arrivalTime(arrivalTime), status("On Time") {
        
        flightID = generateID("FL");
        
        // Calculate optimal seat layout based on capacity
        calculateSeatLayout();
        
        // Initialize seat map with the calculated layout
        initializeSeatMap();
    }
    
    // Calculate optimal seat layout based on capacity
    void calculateSeatLayout() {
        // For small planes (less than 60 seats), use 2-2 configuration
        if (capacity < 60) {
            seatsPerRow = 2;
            totalColumns = 5; // 2 seats + aisle + 2 seats
        }
        // For medium planes (60-150 seats), use 3-3 configuration
        else if (capacity < 150) {
            seatsPerRow = 3;
            totalColumns = 7; // 3 seats + aisle + 3 seats
        }
        // For large planes (150+ seats), use 3-4-3 configuration
        else {
            seatsPerRow = 5;
            totalColumns = 11; // 3 seats + aisle + 4 seats + aisle + 3 seats
        }
    }
    
    // Initialize seat map with the calculated layout
    void initializeSeatMap() {
        // Calculate how many full rows we need
        int seatsPerFullRow = totalColumns - 1; // Subtract 1 for the aisle
        int fullRows = capacity / seatsPerFullRow;
        
        // Calculate remaining seats for the last row
        int remainingSeats = capacity % seatsPerFullRow;
        
        // Total rows needed
        int totalRows = fullRows + (remainingSeats > 0 ? 1 : 0);
        
        // Resize the seat map
        seatMap.resize(totalRows);
        
        // Initialize all rows
        for (int i = 0; i < totalRows; i++) {
            // For the last row with remaining seats
            if (i == totalRows - 1 && remainingSeats > 0) {
                seatMap[i].resize(totalColumns, false);
                
                // Mark seats as available based on remaining seats
                int seatsLeft = remainingSeats;
                int col = 0;
                
                // Fill left side
                while (col < seatsPerRow && seatsLeft > 0) {
                    seatMap[i][col] = false; // Available
                    col++;
                    seatsLeft--;
                }
                
                // Skip aisle
                col++;
                
                // Fill right side
                while (col < totalColumns && seatsLeft > 0) {
                    seatMap[i][col] = false; // Available
                    col++;
                    seatsLeft--;
                }
                
                // Mark remaining positions as unavailable (not part of the plane)
                while (col < totalColumns) {
                    seatMap[i][col] = true; // Unavailable/Not part of plane
                    col++;
                }
            } else {
                // Full row
                seatMap[i].resize(totalColumns, false);
                
                // Mark aisle as unavailable
                if (totalColumns == 5) { // 2-2 configuration
                    seatMap[i][2] = true; // Middle aisle
                } else if (totalColumns == 7) { // 3-3 configuration
                    seatMap[i][3] = true; // Middle aisle
                } else if (totalColumns == 11) { // 3-4-3 configuration
                    seatMap[i][3] = true; // First aisle
                    seatMap[i][8] = true; // Second aisle
                }
            }
        }
        
        // Count total available seats to ensure it matches capacity
        int totalSeats = 0;
        for (size_t i = 0; i < seatMap.size(); i++) {
            for (size_t j = 0; j < seatMap[i].size(); j++) {
                // Skip aisles
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
        
        // If we have too many seats, mark some as unavailable
        if (totalSeats > capacity) {
            int excessSeats = totalSeats - capacity;
            
            // Start from the last row, last seat and work backwards
            for (int i = seatMap.size() - 1; i >= 0 && excessSeats > 0; i--) {
                for (int j = seatMap[i].size() - 1; j >= 0 && excessSeats > 0; j--) {
                    // Skip aisles
                    if ((totalColumns == 5 && j == 2) || 
                        (totalColumns == 7 && j == 3) || 
                        (totalColumns == 11 && (j == 3 || j == 8))) {
                        continue;
                    }
                    
                    if (!seatMap[i][j]) {
                        seatMap[i][j] = true; // Mark as unavailable
                        excessSeats--;
                    }
                }
            }
        }
    }
    
    // Getters - Encapsulation
    string getFlightID() const { return flightID; }
    string getAirlineName() const { return airlineName; }
    string getPlaneID() const { return planeID; }
    int getCapacity() const { return capacity; }
    int getAvailableSeats() const { return availableSeats; }
    string getDestination() const { return destination; }
    string getDepartureTime() const { return departureTime; }
    string getArrivalTime() const { return arrivalTime; }
    string getStatus() const { return status; }
    
    // Setters - Encapsulation
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
    
    // Convert seat number (e.g., "1A") to row and column indices
    pair<int, int> seatNumberToIndices(const string& seatNumber) const {
        try {
            if (seatNumber.length() < 2) {
                throw ValidationException("Invalid seat number format");
            }
            
            // Extract row number (everything except the last character)
            int row;
            try {
                row = stoi(seatNumber.substr(0, seatNumber.length() - 1)) - 1;
            } catch (const invalid_argument&) {
                throw ValidationException("Invalid row number in seat");
            }
            
            // Extract column letter (last character)
            char colLetter = seatNumber.back();
            
            // Convert column letter to index
            int col;
            if (colLetter >= 'A' && colLetter <= 'Z') {
                col = colLetter - 'A';
                
                // Adjust for aisle
                if (totalColumns == 5) { // 2-2 configuration
                    if (col >= 2) col++; // Skip aisle
                } else if (totalColumns == 7) { // 3-3 configuration
                    if (col >= 3) col++; // Skip aisle
                } else if (totalColumns == 11) { // 3-4-3 configuration
                    if (col >= 3 && col < 7) col++; // Skip first aisle
                    else if (col >= 7) col += 2; // Skip both aisles
                }
            } else {
                throw ValidationException("Invalid column letter in seat");
            }
            
            return make_pair(row, col);
        } catch (const exception& e) {
            throw ValidationException(string("Error parsing seat number: ") + e.what());
        }
    }
    
    // Convert row and column indices to seat number (e.g., "1A")
    string indicesToSeatNumber(int row, int col) const {
        // Adjust for aisle
        int adjustedCol = col;
        if (totalColumns == 5) { // 2-2 configuration
            if (col > 2) adjustedCol--; // Account for aisle
        } else if (totalColumns == 7) { // 3-3 configuration
            if (col > 3) adjustedCol--; // Account for aisle
        } else if (totalColumns == 11) { // 3-4-3 configuration
            if (col > 3 && col <= 8) adjustedCol--; // Account for first aisle
            else if (col > 8) adjustedCol -= 2; // Account for both aisles
        }
        
        char colLetter = 'A' + adjustedCol;
        return to_string(row + 1) + colLetter;
    }
    
    // Methods
    bool isSeatAvailable(const string& seatNumber) const {
        try {
            pair<int, int> indices = seatNumberToIndices(seatNumber);
            int row = indices.first;
            int col = indices.second;
            
            if (row < 0 || row >= seatMap.size() || col < 0 || col >= totalColumns) {
                throw ValidationException("Seat number out of range");
            }
            
            // Check if it's an aisle
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
            
            // Check if it's an aisle
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
        
        // Display column headers (seat letters)
        cout << "    ";
        char seatLetter = 'A';
        for (int j = 0; j < totalColumns; j++) {
            // Skip aisle in column headers
            if ((totalColumns == 5 && j == 2) || 
                (totalColumns == 7 && j == 3) || 
                (totalColumns == 11 && (j == 3 || j == 8))) {
                cout << "    "; // Aisle
            } else {
                cout << seatLetter << "   ";
                seatLetter++;
            }
        }
        cout << "\n";
        
        // Display seat map
        for (size_t i = 0; i < seatMap.size(); i++) {
            cout << setw(2) << i + 1 << "  ";
            
            for (size_t j = 0; j < seatMap[i].size(); j++) {
                // Display aisle
                if ((totalColumns == 5 && j == 2) || 
                    (totalColumns == 7 && j == 3) || 
                    (totalColumns == 11 && (j == 3 || j == 8))) {
                    cout << "|   "; // Aisle
                } else {
                    // Check if this is a valid seat (part of the plane)
                    if (j < seatMap[i].size()) {
                        if (seatMap[i][j]) {
                            cout << "X   "; // Occupied or not part of plane
                        } else {
                            cout << "O   "; // Available
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
                // Skip aisles
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
        return ""; // No available seats
    }
    
    bool isFullyBooked() const {
        return availableSeats == 0;
    }
    
    // File operations
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
            
            // Save seat map to a separate file - FIXED: Use overwrite instead of append
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
            string data = dbManager->loadData("flights.txt");
            
            stringstream dataStream(data);
            string line;
            
            while (getline(dataStream, line)) {
                stringstream ss(line);
                string token;
                vector<string> tokens;
                
                while (getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() >= 9) {
                    Flight flight;
                    flight.flightID = tokens[0];
                    flight.airlineName = tokens[1];
                    flight.planeID = tokens[2];
                    flight.capacity = stoi(tokens[3]);
                    flight.availableSeats = stoi(tokens[4]);
                    flight.destination = tokens[5];
                    flight.departureTime = tokens[6];
                    flight.arrivalTime = tokens[7];
                    flight.status = tokens[8];
                    
                    // Always calculate seat layout based on capacity
                    flight.calculateSeatLayout();
                    
                    // Load seat map
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
                    
                    // Initialize seat map if it's empty or incorrect size
                    if (flight.seatMap.empty()) {
                        flight.initializeSeatMap();
                    }
                    
                    flights.push_back(flight);
                }
            }
        } catch (const exception& e) {
            printErrorMessage("Error loading flights: " + string(e.what()));
        }
    }
    
    static void saveAllFlights() {
        try {
            // Clear the file first
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
    string username; // To link reservation to a customer
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
    
    // Getters - Encapsulation
    string getReservationID() const { return reservationID; }
    string getPassengerName() const { return passengerName; }
    string getFlightID() const { return flightID; }
    string getAirlineName() const { return airlineName; }
    string getDestination() const { return destination; }
    string getSeatNumber() const { return seatNumber; }
    string getStatus() const { return status; }
    string getUsername() const { return username; }
    string getPaymentMethod() const { return paymentMethod; }
    
    // File operations
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
            // Clear the file first
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
    vector<pair<string, string>> passengers; // pair<username, passengerName>

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
    
    // File operations
    void saveToFile() const {
        try {
            DatabaseManager* dbManager = DatabaseManager::getInstance();
            
            // If there are no passengers, delete the file instead of saving an empty file
            if (passengers.empty()) {
                dbManager->deleteFile("waitinglists/" + flightID + ".txt");
                return;
            }
            
            stringstream ss;
            for (const auto& passenger : passengers) {
                ss << passenger.first << "," << passenger.second << "\n";
            }
            
            // Clear the file first
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

// Forward declarations for derived classes
class Admin;
class Customer;

// Abstract base class - Abstraction
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
    
    // Getters - Encapsulation
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getName() const { return name; }
    bool getIsAdmin() const { return isAdmin; }
    
    // Setters for derived classes to use
    void setUsername(const string& value) { username = value; }
    void setPassword(const string& value) { password = value; }
    void setName(const string& value) { name = value; }
    void setIsAdmin(bool value) { isAdmin = value; }
    
    // Pure virtual method - Abstraction and Polymorphism
    virtual void displayMenu() = 0;
    
    // File operations
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
    
    // Static methods
    static void loadUsers();
    static void saveAllUsers() {
        try {
            // Clear the file first
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

// Derived class - Inheritance
class Admin : public User {
public:
    Admin() {
        setIsAdmin(true);
    }
    
    Admin(const string& username, const string& password, const string& name)
        : User(username, password, name, true) {}
    
    // Override method - Polymorphism
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
            
            printPrompt("Enter your choice:");
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                choice = 0;
            }
            
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
                default:
                    printErrorMessage("Invalid choice. Please try again.");
                    pressEnterToContinue();
            }
        } while (choice != 8);
    }
    
    void createFlight() {
        clearScreen();
        printHeader("CREATE FLIGHT");
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        string airlineName, planeID, destination, departureTime, arrivalTime;
        int capacity;
        
        try {
            printPrompt("Enter airline name (or 0 to go back):");
            getline(cin, airlineName);
            if (airlineName == "0") return;//added back feature
            if (airlineName.empty()) {
                throw ValidationException("Airline name cannot be empty");
            }
            
            printPrompt("Enter plane number/ID:");
            getline(cin, planeID);
            if (planeID.empty()) {
                throw ValidationException("Plane ID cannot be empty");
            }
            
            printPrompt("Enter airplane capacity:");
            if (!(cin >> capacity)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw ValidationException("Invalid capacity input");
            }
            if (capacity <= 0) {
                throw ValidationException("Capacity must be greater than zero");
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            printPrompt("Enter flight destination (e.g., Manila to South Africa):");
            getline(cin, destination);
            if (destination.empty()) {
                throw ValidationException("Destination cannot be empty");
            }
            
            printPrompt("Enter flight departure time (e.g., May 10, 2025 - 08:00 AM):");
            getline(cin, departureTime);
            if (departureTime.empty()) {
                throw ValidationException("Departure time cannot be empty");
            }
            
            printPrompt("Enter arrival time (e.g., May 10, 2025 - 09:30 AM):");
            getline(cin, arrivalTime);
            if (arrivalTime.empty()) {
                throw ValidationException("Arrival time cannot be empty");
            }
            
            // Display summary
            clearScreen();
            printSubHeader("Flight Summary");
            
            cout << "  Airline: " << airlineName << endl;
            cout << "  Plane ID: " << planeID << endl;
            cout << "  Capacity: " << capacity << " passengers" << endl;
            cout << "  Destination: " << destination << endl;
            cout << "  Departure: " << departureTime << endl;
            cout << "  Arrival: " << arrivalTime << endl;
            
            char confirm;
            printPrompt("\nConfirm flight creation (y/n):");
            cin >> confirm;
            
            if (tolower(confirm) == 'y') {
                Flight flight(airlineName, planeID, capacity, destination, departureTime, arrivalTime);
                flights.push_back(flight);
                flight.saveToFile();
                
                // Create waiting list for this flight
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
            printInfoMessage("No flights available.");
            pressEnterToContinue();
            return;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        try {
            printPrompt("Enter airline name (or 0 to go back):");
            string airlineName;
            getline(cin, airlineName);

            if(airlineName == "0"){
                return;
            }
            
            vector<Flight> airlineFlights;
            for (const auto& flight : flights) {
                if (flight.getAirlineName() == airlineName) {
                    airlineFlights.push_back(flight);
                }
            }
            
            if (airlineFlights.empty()) {
                throw ValidationException("No flights found for airline: " + airlineName);
            }
            
            printSubHeader("Available flights for " + airlineName);
            
            vector<pair<string, int>> columns = {
                {"Flight ID", 10},
                {"Destination", 30},
                {"Departure Time", 25},
                {"Arrival Time", 25}
            };
            
            printTableHeader(columns);
            
            for (const auto& flight : airlineFlights) {
                vector<pair<string, int>> row = {
                    {flight.getFlightID(), 10},
                    {flight.getDestination(), 30},
                    {flight.getDepartureTime(), 25},
                    {flight.getArrivalTime(), 25}
                };
                
                printTableRow(row);
            }
            
            printPrompt("\nEnter Flight ID to delete:");
            string flightID;
            getline(cin, flightID);
            
            auto it = find_if(flights.begin(), flights.end(), 
                             [&flightID](const Flight& f) { return f.getFlightID() == flightID; });
            
            if (it == flights.end()) {
                throw ValidationException("Flight not found");
            }
            
            char confirm;
            printPrompt("\nConfirm delete (y/n):");
            cin >> confirm;
            
            if (tolower(confirm) == 'y') {
                // Remove reservations for this flight
                reservations.erase(
                    remove_if(reservations.begin(), reservations.end(),
                             [&flightID](const Reservation& r) { return r.getFlightID() == flightID; }),
                    reservations.end());
                
                // Remove waiting list for this flight
                waitingLists.erase(flightID);
                
                // Delete seat map file
                DatabaseManager* dbManager = DatabaseManager::getInstance();
                dbManager->deleteFile("seatmaps/" + flightID + ".txt");
                
                // Delete waiting list file - only if it exists
                dbManager->deleteFile("waitinglists/" + flightID + ".txt");
                
                // Remove flight
                flights.erase(it);
                
                // Save changes
                Flight::saveAllFlights();
                Reservation::saveAllReservations();
                WaitingList::saveAllWaitingLists();
                
                printSuccessMessage("Flight deleted successfully!");
            } else {
                printInfoMessage("Deletion cancelled.");
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
            // Display all flights
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 10},
                {"Airline", 20},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 10},
                    {flights[i].getAirlineName(), 20},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex;
            printPrompt("\nEnter flight number to view reservations:");
            cin >> flightIndex;

            if (flightIndex == 0){
                return;
            } //added for back function
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(flights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            string flightID = flights[flightIndex - 1].getFlightID();
            
            // Display reservations for the selected flight
            clearScreen();
            printHeader("RESERVATIONS FOR FLIGHT " + flightID);
            
            vector<Reservation> flightReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getFlightID() == flightID) {
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
                {"Reservation ID", 15},
                {"Passenger Name", 25},
                {"Flight Number", 15},
                {"Airline", 20},
                {"Destination", 30},
                {"Seat Number", 15},
                {"Status", 15}
            };
            
            printTableHeader(resColumns);
            
            for (const auto& reservation : flightReservations) {
                vector<pair<string, int>> row = {
                    {reservation.getReservationID(), 15},
                    {reservation.getPassengerName(), 25},
                    {reservation.getFlightID(), 15},
                    {reservation.getAirlineName(), 20},
                    {reservation.getDestination(), 30},
                    {reservation.getSeatNumber(), 15},
                    {reservation.getStatus(), 15}
                };
                
                printTableRow(row);
            }
            
            char deleteOption;
            printPrompt("\nDo you want to delete a reservation? (y/n): ");
            cin >> deleteOption;

            if (tolower(deleteOption) == 'y') {
                string reservationID;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                printPrompt("Enter Reservation ID to delete:");
                getline(cin, reservationID);
                
                auto it = find_if(reservations.begin(), reservations.end(),
                                 [&reservationID](const Reservation& r) { return r.getReservationID() == reservationID; });
                
                if (it == reservations.end()) {
                    throw ValidationException("Reservation not found");
                }
                
                char confirm;
                printPrompt("\nConfirm delete (y/n):");
                cin >> confirm;
                
                if (tolower(confirm) == 'y') {
                    // Free up the seat
                    for (auto& flight : flights) {
                        if (flight.getFlightID() == it->getFlightID()) {
                            flight.cancelSeat(it->getSeatNumber());
                            break;
                        }
                    }
                    
                    // Remove reservation
                    reservations.erase(it);
                    
                    // Save changes
                    Flight::saveAllFlights();
                    Reservation::saveAllReservations();
                    
                    printSuccessMessage("Reservation deleted successfully!");
                } else {
                    printInfoMessage("Deletion cancelled.");
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
            // Display all flights
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 10},
                {"Airline", 20},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 10},
                    {flights[i].getAirlineName(), 20},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex;
            printPrompt("\nEnter flight number to view status:");
            cin >> flightIndex;

            if(flightIndex == 0){
                return;
            }//added back feature
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(flights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            Flight& flight = flights[flightIndex - 1];
            
            // Display flight status
            clearScreen();
            printHeader("FLIGHT STATUS");
            
            cout << "  Flight Number: " << flight.getFlightID() << "\n";
            cout << "  Airline: " << flight.getAirlineName() << "\n";
            cout << "  Departure Time: " << flight.getDepartureTime() << "\n";
            cout << "  Arrival Time: " << flight.getArrivalTime() << "\n";
            cout << "  Status: " << flight.getStatus() << "\n";
            
            char editOption;
            printPrompt("\nDo you want to edit the flight? (y/n):");
            cin >> editOption;

            if (tolower(editOption) == 'y') {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
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
                
                char confirm;
                printPrompt("\nConfirm changes? (y/n):");
                cin >> confirm;
                
                if (tolower(confirm) == 'y') {
                    flight.setAirlineName(airline);
                    flight.setDepartureTime(departureTime);
                    flight.setArrivalTime(arrivalTime);
                    flight.setStatus(status);
                    
                    // Save changes
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
            // Display all flights
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 10},
                {"Airline", 20},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 10},
                    {flights[i].getAirlineName(), 20},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex;
            printPrompt("\nEnter flight number to view seat map:");
            cin >> flightIndex;

            if(flightIndex == 0){
                return;
            }//added back feature
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(flights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            Flight& flight = flights[flightIndex - 1];
            
            // Display seat map
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
            // Display all flights
            printSubHeader("Available Flights");
            
            vector<pair<string, int>> columns = {
                {"No.", 10},
                {"Flight ID", 10},
                {"Airline", 20},
                {"Destination", 25}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < flights.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 10},
                    {flights[i].getFlightID(), 10},
                    {flights[i].getAirlineName(), 20},
                    {flights[i].getDestination(), 25}
                };
                
                printTableRow(row);
            }
            
            printBackOption();

            int flightIndex;
            printPrompt("\nEnter flight number to manage waiting list:");
            cin >> flightIndex;

            if(flightIndex == 0){
                return;
            } //added back feature
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(flights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            Flight& flight = flights[flightIndex - 1];
            string flightID = flight.getFlightID();
            
            if (waitingLists.find(flightID) == waitingLists.end()) {
                waitingLists[flightID] = WaitingList(flightID);
            }
            
            WaitingList& waitingList = waitingLists[flightID];
            
            // Display waiting list
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
            
            int choice;
            printPrompt("Enter your choice:");
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    // Promote passenger
                    if (flight.isFullyBooked()) {
                        throw BookingException("Flight is fully booked. Cannot promote passenger.");
                    }
                    
                    pair<string, string> nextPassenger = waitingList.getNextPassenger();
                    if (nextPassenger.first.empty()) {
                        throw ValidationException("No passengers in the waiting list");
                    }
                    
                    // Display seat map
                    flight.displaySeatMap();
                    
                    string seatNumber;
                    printPrompt("\nEnter seat number for the passenger:");
                    cin >> seatNumber;
                    
                    if (!flight.isSeatAvailable(seatNumber)) {
                        throw BookingException("Seat is not available. Please choose another seat.");
                    }
                    
                    char confirm;
                    printPrompt("\nConfirm changes? (y/n):");
                    cin >> confirm;
                    
                    if (tolower(confirm) == 'y') {
                        // Book the seat
                        flight.bookSeat(seatNumber);
                        
                        // Create reservation
                        Reservation reservation(nextPassenger.second, flightID, flight.getAirlineName(), 
                                               flight.getDestination(), seatNumber, nextPassenger.first);
                        reservations.push_back(reservation);
                        
                        // Remove from waiting list
                        waitingList.removePassenger(nextPassenger.first);
                        
                        // Save changes
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
                    // Delete passenger
                    string username;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    printPrompt("\nEnter username of passenger to delete:");
                    getline(cin, username);
                    
                    char confirm;
                    printPrompt("\nConfirm deletion? (y/n):");
                    cin >> confirm;
                    
                    if (tolower(confirm) == 'y') {
                        if (waitingList.removePassenger(username)) {
                            waitingList.saveToFile();
                            printSuccessMessage("Passenger removed from waiting list successfully!");
                        } else {
                            throw ValidationException("Passenger not found in waiting list");
                        }
                    } else {
                        printInfoMessage("Deletion cancelled.");
                    }
                    break;
                }
                case 3:
                    // Return to menu
                    return;
                default:
                    throw ValidationException("Invalid choice");
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
            // Display all customer accounts
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
            
            char deleteOption;
            printPrompt("\nDo you want to delete a user account? (y/n):");
            cin >> deleteOption;
            
            if (tolower(deleteOption) == 'y') {
                string username;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                printPrompt("Enter username of account to delete:");
                getline(cin, username);
                
                auto it = find_if(users.begin(), users.end(),
                                 [&username](const User* u) { return u->getUsername() == username && !u->getIsAdmin(); });
                
                if (it == users.end()) {
                    throw ValidationException("Customer account not found");
                }
                
                char confirm;
                printPrompt("\nConfirm delete (y/n):");
                cin >> confirm;
                
                if (tolower(confirm) == 'y') {
                    // Remove all reservations for this user
                    reservations.erase(
                        remove_if(reservations.begin(), reservations.end(),
                                 [&username](const Reservation& r) { return r.getUsername() == username; }),
                        reservations.end());
                    
                    // Remove from all waiting lists
                    for (auto& pair : waitingLists) {
                        pair.second.removePassenger(username);
                    }
                    
                    // Delete user
                    delete *it;
                    users.erase(it);
                    
                    // Save changes
                    User::saveAllUsers();
                    Reservation::saveAllReservations();
                    WaitingList::saveAllWaitingLists();
                    
                    printSuccessMessage("User account deleted successfully!");
                } else {
                    printInfoMessage("Deletion cancelled.");
                }
            }
        } catch (const exception& e) {
            printErrorMessage(e.what());
        }
        
        pressEnterToContinue();
    }
};

// Derived class - Inheritance
class Customer : public User {
public:
    Customer() {
        setIsAdmin(false);
    }
    
    Customer(const string& username, const string& password, const string& name)
        : User(username, password, name, false) {}
    
    // Override method - Polymorphism
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
            
            printPrompt("Enter your choice:");
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                choice = 0;
            }
            
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
                default:
                    printErrorMessage("Invalid choice. Please try again.");
                    pressEnterToContinue();
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
            // Display all flights
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
                    {flight.getArrivalTime(), 30},
                    {to_string(flight.getAvailableSeats()), 15}
                };
                
                printTableRow(row);
            }
            
            char bookOption;
            printPrompt("\nDo you want to book a flight? (y/n): ");
            cin >> bookOption;
        
            if (tolower(bookOption) == 'y') {
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
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        try {
            printPrompt("Enter your destination or 'b' to go back:");
            string destination;
            getline(cin, destination);
            if(destination == "b" || destination == "B"){
                return;
            } // added back feature

            clearScreen();
            
            // Find flights for the destination
            vector<Flight*> matchingFlights;
            for (auto& flight : flights) {
                if (flight.getDestination().find(destination) != string::npos) {
                    matchingFlights.push_back(&flight);
                }
            }
            
            if (matchingFlights.empty()) {
                throw ValidationException("No flights found for destination: " + destination);
            }
            
            // Display matching flights
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

            int flightIndex;
            printPrompt("\nChoose flight. Enter flight number:");
            cin >> flightIndex;

            if(flightIndex == 0){
                return;
            } //added back feature
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(matchingFlights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            Flight* selectedFlight = matchingFlights[flightIndex - 1];
            
            if (selectedFlight->isFullyBooked()) {
                printWarningMessage("This flight is fully booked.");
                
                char waitingListOption;
                printPrompt("Do you want to be added to the waiting list? (y/n):");
                cin >> waitingListOption;
                
                if (tolower(waitingListOption) == 'y') {
                    // Add to waiting list
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
            
            // Show seat map
            selectedFlight->displaySeatMap();
            
            string seatNumber;
            printPrompt("\nEnter seat number (e.g., 1A) or 'b' to go back:");
            cin >> seatNumber;

            if(seatNumber == "B" || seatNumber == "b"){
                return;
            } // added back feature
            
            if (!selectedFlight->isSeatAvailable(seatNumber)) {
                throw BookingException("Seat is not available. Please choose another seat.");
            }
            
            // Choose payment method - Strategy Pattern implementation
            printSubHeader("Payment Method");
            printMenuOption(1, "GCash");
            printMenuOption(2, "Card");
            printBackOption();
            
            int paymentMethod;
            printPrompt("Enter your choice:");
            cin >> paymentMethod;
            
            if(paymentMethod == 0){
                return;
            } // added back feature

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            // Create appropriate payment strategy based on user choice
            unique_ptr<PaymentStrategy> paymentStrategy;
            string paymentDetails;
            
            if (paymentMethod == 1) {
                // GCash
                string gcashNumber;
                printPrompt("\nEnter GCash number:");
                getline(cin, gcashNumber);
                
                paymentStrategy = make_unique<GCashPaymentStrategy>(gcashNumber);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else if (paymentMethod == 2) {
                // Card
                string cardNumber, expiryDate, cvv;
                
                printPrompt("\nEnter Card number:");
                getline(cin, cardNumber);
                
                printPrompt("Enter expiration date (MM/YY):");
                getline(cin, expiryDate);
                
                printPrompt("Enter CVV:");
                getline(cin, cvv);
                
                paymentStrategy = make_unique<CreditCardPaymentStrategy>(cardNumber, expiryDate, cvv);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else {
                throw ValidationException("Invalid payment method");
            }
            
            // Process payment using the selected strategy
            double flightPrice = 500.00; // Example price
            
            // Display payment summary
            clearScreen();
            printSubHeader("Payment Summary");
            
            cout << "  Flight: " << selectedFlight->getFlightID() << " - " << selectedFlight->getAirlineName() << endl;
            cout << "  Destination: " << selectedFlight->getDestination() << endl;
            cout << "  Seat: " << seatNumber << endl;
            cout << "  Payment Method: " << paymentDetails << endl;
            cout << "  Amount: $" << fixed << setprecision(2) << flightPrice << endl;
            
            char confirm;
            printPrompt("\nConfirm payment? (y/n):");
            cin >> confirm;
            
            bool paymentConfirmed = false;
            if (tolower(confirm) == 'y') {
                paymentConfirmed = paymentStrategy->processPayment(flightPrice);
            }
            
            if (paymentConfirmed) {
                // Book the seat
                selectedFlight->bookSeat(seatNumber);
                
                // Create reservation
                Reservation reservation(getName(), selectedFlight->getFlightID(), selectedFlight->getAirlineName(), 
                                       selectedFlight->getDestination(), seatNumber, getUsername(), paymentDetails);
                reservations.push_back(reservation);
                
                // Save changes
                Flight::saveAllFlights();
                reservation.saveToFile();
                
                printSuccessMessage("Payment successful! Your flight has been booked.");
                
                // Print boarding pass
                clearScreen();
                printHeader("BOARDING PASS");
                
                cout << "  +-" << string(60, '-') << "-+" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  | " << setw(60) << left << "   " + selectedFlight->getAirlineName() + " Airlines" << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  PASSENGER: " << setw(47) << left << getName() << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  FLIGHT: " << setw(15) << left << selectedFlight->getFlightID() 
                     << "DATE: " << setw(27) << left << selectedFlight->getDepartureTime().substr(0, selectedFlight->getDepartureTime().find(" - ")) << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  FROM/TO: " << setw(50) << left << selectedFlight->getDestination() << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  SEAT: " << setw(53) << left << seatNumber << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  BOARDING TIME: " << setw(44) << left << selectedFlight->getDepartureTime().substr(selectedFlight->getDepartureTime().find(" - ") + 3) << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  |  " << setw(58) << left << "Thank you for choosing " + selectedFlight->getAirlineName() + " Airlines!" << " |" << endl;
                cout << "  | " << setw(60) << left << " " << " |" << endl;
                cout << "  +-" << string(60, '-') << "-+" << endl;
                
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
            // Find reservations for this customer
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
            
            // Display bookings
            printSubHeader("Your Bookings");
            
            vector<pair<string, int>> columns = {
                {"Reservation ID", 15},
                {"Flight ID", 15},
                {"Airline", 20},
                {"Destination", 30},
                {"Seat Number", 15},
                {"Status", 15}
            };
            
            printTableHeader(columns);
            
            for (const auto& reservation : customerReservations) {
                vector<pair<string, int>> row = {
                    {reservation.getReservationID(), 15},
                    {reservation.getFlightID(), 15},
                    {reservation.getAirlineName(), 20},
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
            // Find reservations for this customer
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
            
            // Display bookings
            printSubHeader("Your Bookings");
            
            vector<pair<string, int>> columns = {
                {"No.", 5},
                {"Reservation ID", 15},
                {"Flight ID", 15},
                {"Airline", 20},
                {"Destination", 30},
                {"Seat Number", 15}
            };
            
            printTableHeader(columns);
            
            for (size_t i = 0; i < customerReservations.size(); i++) {
                vector<pair<string, int>> row = {
                    {to_string(i + 1), 5},
                    {customerReservations[i].getReservationID(), 15},
                    {customerReservations[i].getFlightID(), 15},
                    {customerReservations[i].getAirlineName(), 20},
                    {customerReservations[i].getDestination(), 30},
                    {customerReservations[i].getSeatNumber(), 15}
                };
                
                printTableRow(row);
            }
            
            printBackOption();
            
            int bookingIndex;
            printPrompt("\nEnter booking number to cancel:");
            cin >> bookingIndex;

            if(bookingIndex == 0){
                return;
            } // added back feature
            
            if (bookingIndex < 1 || bookingIndex > static_cast<int>(customerReservations.size())) {
                throw ValidationException("Invalid booking number");
            }
            
            Reservation& selectedReservation = customerReservations[bookingIndex - 1];
            
            // Display cancellation confirmation
            clearScreen();
            printSubHeader("Cancellation Confirmation");
            
            cout << "  Reservation ID: " << selectedReservation.getReservationID() << endl;
            cout << "  Flight: " << selectedReservation.getFlightID() << " - " << selectedReservation.getAirlineName() << endl;
            cout << "  Destination: " << selectedReservation.getDestination() << endl;
            cout << "  Seat: " << selectedReservation.getSeatNumber() << endl;
            
            char confirm;
            printPrompt("\nConfirm cancellation? (y/n):");
            cin >> confirm;
            
            if (tolower(confirm) == 'y') {
                // Free up the seat
                for (auto& flight : flights) {
                    if (flight.getFlightID() == selectedReservation.getFlightID()) {
                        flight.cancelSeat(selectedReservation.getSeatNumber());
                        break;
                    }
                }
                
                // Remove reservation
                auto it = find_if(reservations.begin(), reservations.end(),
                                 [&selectedReservation](const Reservation& r) { 
                                     return r.getReservationID() == selectedReservation.getReservationID(); 
                                 });
                
                if (it != reservations.end()) {
                    reservations.erase(it);
                }
                
                // Save changes
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

// Implementation of User::loadUsers() after the derived classes are defined
void User::loadUsers() {
    try {
        // Clear existing users
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

// Implementation of system functions
void initializeSystem() {
    try {
        // Create necessary directories using cross-platform function
        createDirectory("seatmaps");
        createDirectory("waitinglists");
        
        // Load data from files
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
        
        char userType;
        printPrompt("Enter your choice:");
        cin >> userType;

        if (userType == '3') {
            return;
        }

        if (userType != '1' && userType != '2') {
            throw ValidationException("Invalid choice");
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string username, password, confirmPassword, name;

        printPrompt("\nEnter username:");
        getline(cin, username);
        if (username.empty()) {
            throw ValidationException("Username cannot be empty");
        }
        if (User::usernameExists(username)) {
            throw ValidationException("Username already exists. Please choose another one");
        }

        printPrompt("Enter password:");
        getline(cin, password);
        if (password.empty()) {
            throw ValidationException("Password cannot be empty");
        }

        printPrompt("Confirm password:");
        getline(cin, confirmPassword);
        if (password != confirmPassword) {
            throw ValidationException("Passwords do not match");
        }

        printPrompt("Enter your full name:");
        getline(cin, name);
        if (name.empty()) {
            throw ValidationException("Name cannot be empty");
        }

        User* newUser;
        if (userType == '1') {
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
        
        char userType;
        printPrompt("Enter your choice:");
        cin >> userType;

        if (userType == '3') {
            return;
        }

        if (userType != '1' && userType != '2') {
            throw ValidationException("Invalid choice");
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string username, password;

        printPrompt("\nEnter username:");
        getline(cin, username);

        printPrompt("Enter password:");
        getline(cin, password);

        User* user = User::login(username, password);
        if (user == nullptr) {
            throw ValidationException("Invalid username or password");
        }

        bool isAdmin = user->getIsAdmin();
        if ((userType == '1' && !isAdmin) || (userType == '2' && isAdmin)) {
            throw ValidationException("Invalid user type for this account");
        }

        printSuccessMessage("Login successful! Welcome, " + user->getName() + "!");
        pressEnterToContinue();

        currentUser = user;
        currentUser->displayMenu();
        currentUser = nullptr;
    } catch (const exception& e) {
        printErrorMessage(e.what());
        pressEnterToContinue();
    }
}

int main() {
    // Initialize the system
    initializeSystem();
    
    // Set console code page to UTF-8 for Windows
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
        
        printPrompt("Enter your choice:");
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }
        
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
            default:
                printErrorMessage("Invalid choice. Please try again.");
                pressEnterToContinue();
        }
    } while (choice != 3);
    
    // Clean up
    for (auto user : users) {
        delete user;
    }
    
    // Clean up singleton
    delete DatabaseManager::getInstance();
    
    return 0;
}
