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
    DatabaseManager() {
        // Initialize database connection
        cout << "Database connection initialized." << endl;
    }
    
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
            cerr << "Error saving data: " << e.what() << endl;
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
            cerr << "Error saving data: " << e.what() << endl;
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
            cerr << "Error loading data: " << e.what() << endl;
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
            cerr << "Error deleting file: " << e.what() << endl;
            return false;
        }
    }
    
    ~DatabaseManager() {
        cout << "Database connection closed." << endl;
    }
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
        cout << "Processing GCash payment of $" << fixed << setprecision(2) << amount 
             << " using number " << gcashNumber << "..." << endl;
        
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
        cout << "Processing Credit Card payment of $" << fixed << setprecision(2) << amount 
             << " using card ending with " << cardNumber.substr(cardNumber.length() - 4) << "..." << endl;
        
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
    cout << "Press any key to continue..." << flush;
    #ifdef _WIN32
        system("pause > nul");
    #else
        // More reliable method for Unix-like systems
        system("bash -c 'read -n 1 -s'");
    #endif
    cout << endl;
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
            cerr << "Error checking seat availability: " << e.what() << endl;
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
            cerr << "Error booking seat: " << e.what() << endl;
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
            cerr << "Error canceling seat: " << e.what() << endl;
            return false;
        }
    }
    
    void displaySeatMap() const {
        cout << "\nSeat Map for Flight " << flightID << " (" << airlineName << "):\n";
        cout << "Destination: " << destination << "\n";
        cout << "Available Seats: " << availableSeats << " out of " << capacity << "\n\n";
        
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
        cout << "\nLegend: O - Available, X - Occupied, | - Aisle\n";
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
            cerr << "Error saving flight: " << e.what() << endl;
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
            cerr << "Error loading flights: " << e.what() << endl;
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
            cerr << "Error saving all flights: " << e.what() << endl;
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
            cerr << "Error saving reservation: " << e.what() << endl;
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
            cerr << "Error loading reservations: " << e.what() << endl;
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
            cerr << "Error saving all reservations: " << e.what() << endl;
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
        cout << "\nWaiting List for Flight " << flightID << ":\n";
        cout << setw(5) << "No." << setw(20) << "Passenger Name" << setw(20) << "Username" << "\n";
        cout << string(45, '-') << "\n";
        
        for (size_t i = 0; i < passengers.size(); i++) {
            cout << setw(5) << i + 1 
                 << setw(20) << passengers[i].second 
                 << setw(20) << passengers[i].first << "\n";
        }
        
        if (passengers.empty()) {
            cout << "No passengers in the waiting list.\n";
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
            cerr << "Error saving waiting list: " << e.what() << endl;
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
            cerr << "Error loading waiting lists: " << e.what() << endl;
        }
    }
    
    static void saveAllWaitingLists() {
        try {
            for (auto& pair : waitingLists) {
                pair.second.saveToFile();
            }
        } catch (const exception& e) {
            cerr << "Error saving all waiting lists: " << e.what() << endl;
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
            cerr << "Error saving user: " << e.what() << endl;
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
            cerr << "Error saving all users: " << e.what() << endl;
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
            cout << "\n===== ADMIN MENU =====\n";
            cout << "1. Create Flight\n";
            cout << "2. Delete Flight\n";
            cout << "3. Reservations (View/Delete)\n";
            cout << "4. Flight Status (View/Edit)\n";
            cout << "5. View Seat Maps\n";
            cout << "6. Manage Waiting List\n";
            cout << "7. User Accounts\n";
            cout << "8. Logout\n";
            cout << "Enter your choice: ";
            
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
                    cout << "\nLogging out...\n";
                    break;
                default:
                    cout << "\nInvalid choice. Please try again.\n";
                    pressEnterToContinue();
            }
        } while (choice != 8);
    }
    
    void createFlight() {
        clearScreen();
        cout << "\n===== CREATE FLIGHT =====\n";
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        string airlineName, planeID, destination, departureTime, arrivalTime;
        int capacity;
        
        try {
            cout << "Enter airline name (or 0 to go back): ";
            getline(cin, airlineName);
            if (airlineName == "0") return;//added back feature
            if (airlineName.empty()) {
                throw ValidationException("Airline name cannot be empty");
            }
            
            cout << "Enter plane number/ID: ";
            getline(cin, planeID);
            if (planeID.empty()) {
                throw ValidationException("Plane ID cannot be empty");
            }
            
            cout << "Enter airplane capacity: ";
            if (!(cin >> capacity)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw ValidationException("Invalid capacity input");
            }
            if (capacity <= 0) {
                throw ValidationException("Capacity must be greater than zero");
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            cout << "Enter flight destination (e.g., Manila to South Africa): ";
            getline(cin, destination);
            if (destination.empty()) {
                throw ValidationException("Destination cannot be empty");
            }
            
            cout << "Enter flight departure time (e.g., May 10, 2025 - 08:00 AM): ";
            getline(cin, departureTime);
            if (departureTime.empty()) {
                throw ValidationException("Departure time cannot be empty");
            }
            
            cout << "Enter arrival time (e.g., May 10, 2025 - 09:30 AM): ";
            getline(cin, arrivalTime);
            if (arrivalTime.empty()) {
                throw ValidationException("Arrival time cannot be empty");
            }
            
            char confirm;
            cout << "\nConfirm flight creation (y/n): ";
            cin >> confirm;
            
            if (tolower(confirm) == 'y') {
                Flight flight(airlineName, planeID, capacity, destination, departureTime, arrivalTime);
                flights.push_back(flight);
                flight.saveToFile();
                
                // Create waiting list for this flight
                WaitingList waitingList(flight.getFlightID());
                waitingLists[flight.getFlightID()] = waitingList;
                
                cout << "\nFlight created successfully!\n";
            } else {
                cout << "\nFlight creation cancelled.\n";
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void deleteFlight() {
        clearScreen();
        cout << "\n===== DELETE FLIGHT =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        try {
            string airlineName;
            cout << "Enter airline name (or 0 to go back): ";
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
            
            cout << "\nAvailable flights for " << airlineName << ":\n";
            cout << setw(10) << "Flight ID" << setw(20) << "Destination" << setw(25) << "Departure Time" << setw(25) << "Arrival Time" << "\n";
            cout << string(80, '-') << "\n";
            
            for (const auto& flight : airlineFlights) {
                cout << setw(10) << flight.getFlightID()
                     << setw(20) << flight.getDestination()
                     << setw(25) << flight.getDepartureTime()
                     << setw(25) << flight.getArrivalTime() << "\n";
            }
            
            string flightID;
            cout << "\nEnter Flight ID to delete: ";
            getline(cin, flightID);
            
            auto it = find_if(flights.begin(), flights.end(), 
                             [&flightID](const Flight& f) { return f.getFlightID() == flightID; });
            
            if (it == flights.end()) {
                throw ValidationException("Flight not found");
            }
            
            char confirm;
            cout << "\nConfirm delete (y/n): ";
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
                
                cout << "\nFlight deleted successfully!\n";
            } else {
                cout << "\nDeletion cancelled.\n";
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void manageReservations() {
        clearScreen();
        cout << "\n===== RESERVATIONS =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        try {
            // Display all flights
            cout << "Available Flights:\n";
            cout << setw(10) << "No." << setw(10) << "Flight ID" << setw(20) << "Airline" << setw(25) << "Destination" << "\n";
            cout << string(65, '-') << "\n";
            
            for (size_t i = 0; i < flights.size(); i++) {
                cout << setw(10) << i + 1
                     << setw(10) << flights[i].getFlightID()
                     << setw(20) << flights[i].getAirlineName()
                     << setw(25) << flights[i].getDestination() << "\n";
            }
            
            cout << "" << endl;

            cout << "0. Back to Main Menu\n";

            int flightIndex;
            cout << "\nEnter flight number to view reservations: ";
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
            cout << "\n===== RESERVATIONS FOR FLIGHT " << flightID << " =====\n";
            
            vector<Reservation> flightReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getFlightID() == flightID) {
                    flightReservations.push_back(reservation);
                }
            }
            
            if (flightReservations.empty()) {
                cout << "No reservations found for this flight.\n";
                pressEnterToContinue();
                return;
            }
            
            cout << "\nReservations:\n";
            cout << setw(15) << "Reservation ID" 
                 << setw(25) << "Passenger Name" 
                 << setw(15) << "Flight Number" 
                 << setw(20) << "Airline" 
                 << setw(30) << "Destination" 
                 << setw(15) << "Seat Number" 
                 << setw(15) << "Status" << "\n";
            cout << string(135, '-') << "\n";
            
            for (const auto& reservation : flightReservations) {
                cout << setw(15) << reservation.getReservationID()
                     << setw(25) << reservation.getPassengerName()
                     << setw(15) << reservation.getFlightID()
                     << setw(20) << reservation.getAirlineName()
                     << setw(30) << reservation.getDestination()
                     << setw(15) << reservation.getSeatNumber()
                     << setw(15) << reservation.getStatus() << "\n";
            }
            
            char deleteOption;
            cout << "\nDo you want to delete a reservation? (y/n) or 'b' to go back: ";
            cin >> deleteOption;

            if(tolower(deleteOption) == 'b'){
                return;
            } //added function for back
            
            else if (tolower(deleteOption) == 'y') {
                string reservationID;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Enter Reservation ID to delete: ";
                getline(cin, reservationID);
                
                auto it = find_if(reservations.begin(), reservations.end(),
                                 [&reservationID](const Reservation& r) { return r.getReservationID() == reservationID; });
                
                if (it == reservations.end()) {
                    throw ValidationException("Reservation not found");
                }
                
                char confirm;
                cout << "\nConfirm delete (y/n): ";
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
                    
                    cout << "\nReservation deleted successfully!\n";
                } else {
                    cout << "\nDeletion cancelled.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void manageFlightStatus() {
        clearScreen();
        cout << "\n===== FLIGHT STATUS =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        try {
            // Display all flights
            cout << "Available Flights:\n";
            cout << setw(10) << "No." << setw(10) << "Flight ID" << setw(20) << "Airline" << setw(25) << "Destination" << "\n";
            cout << string(65, '-') << "\n";
            
            for (size_t i = 0; i < flights.size(); i++) {
                cout << setw(10) << i + 1
                     << setw(10) << flights[i].getFlightID()
                     << setw(20) << flights[i].getAirlineName()
                     << setw(25) << flights[i].getDestination() << "\n";
            }
            
            cout << "" << endl;

            cout << "0. Back to Admin Menu\n"; //Back option

            int flightIndex;
            cout << "\nEnter flight number to view status: ";
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
            cout << "\n===== FLIGHT STATUS =====\n";
            cout << "Flight Number: " << flight.getFlightID() << "\n";
            cout << "Airline: " << flight.getAirlineName() << "\n";
            cout << "Departure Time: " << flight.getDepartureTime() << "\n";
            cout << "Arrival Time: " << flight.getArrivalTime() << "\n";
            cout << "Status: " << flight.getStatus() << "\n";
            
            char editOption;
            cout << "\nDo you want to edit the flight? (y/n) or 'b' to go back: ";
            cin >> editOption;

            if(tolower(editOption) == 'b'){
                return;
            }//added back feature
            
            if (tolower(editOption) == 'y') {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                string airline, departureTime, arrivalTime, status;
                
                cout << "\nEnter Airline (current: " << flight.getAirlineName() << "): ";
                getline(cin, airline);
                if (airline.empty()) airline = flight.getAirlineName();
                
                cout << "Enter Departure Time (current: " << flight.getDepartureTime() << "): ";
                getline(cin, departureTime);
                if (departureTime.empty()) departureTime = flight.getDepartureTime();
                
                cout << "Enter Arrival Time (current: " << flight.getArrivalTime() << "): ";
                getline(cin, arrivalTime);
                if (arrivalTime.empty()) arrivalTime = flight.getArrivalTime();
                
                cout << "Enter Flight Status (current: " << flight.getStatus() << "): ";
                getline(cin, status);
                if (status.empty()) status = flight.getStatus();
                
                char confirm;
                cout << "\nConfirm changes? (y/n): ";
                cin >> confirm;
                
                if (tolower(confirm) == 'y') {
                    flight.setAirlineName(airline);
                    flight.setDepartureTime(departureTime);
                    flight.setArrivalTime(arrivalTime);
                    flight.setStatus(status);
                    
                    // Save changes
                    Flight::saveAllFlights();
                    
                    cout << "\nFlight information updated successfully!\n";
                } else {
                    cout << "\nChanges cancelled.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void viewSeatMaps() {
        clearScreen();
        cout << "\n===== VIEW SEAT MAPS =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        try {
            // Display all flights
            cout << "Available Flights:\n";
            cout << setw(10) << "No." << setw(10) << "Flight ID" << setw(20) << "Airline" << setw(25) << "Destination" << "\n";
            cout << string(65, '-') << "\n";
            
            for (size_t i = 0; i < flights.size(); i++) {
                cout << setw(10) << i + 1
                     << setw(10) << flights[i].getFlightID()
                     << setw(20) << flights[i].getAirlineName()
                     << setw(25) << flights[i].getDestination() << "\n";
            }
            
            cout << "" << endl;

            cout << "0. Back to Main Menu\n";//added back option

            int flightIndex;
            cout << "\nEnter flight number to view seat map: ";
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
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void manageWaitingList() {
        clearScreen();
        cout << "\n===== MANAGE WAITING LIST =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        try {
            // Display all flights
            cout << "Available Flights:\n";
            cout << setw(10) << "No." << setw(10) << "Flight ID" << setw(20) << "Airline" << setw(25) << "Destination" << "\n";
            cout << string(65, '-') << "\n";
            
            for (size_t i = 0; i < flights.size(); i++) {
                cout << setw(10) << i + 1
                     << setw(10) << flights[i].getFlightID()
                     << setw(20) << flights[i].getAirlineName()
                     << setw(25) << flights[i].getDestination() << "\n";
            }
            
            cout << "" << endl;

            cout << "0. Back to Main Menu\n"; //added back option

            int flightIndex;
            cout << "\nEnter flight number to manage waiting list: ";
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
            
            cout << "\nOptions:\n";
            cout << "1. Promote passenger\n";
            cout << "2. Delete passenger\n";
            cout << "3. Return to menu\n";
            cout << "Enter your choice: ";
            
            int choice;
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
                    cout << "\nEnter seat number for the passenger: ";
                    cin >> seatNumber;
                    
                    if (!flight.isSeatAvailable(seatNumber)) {
                        throw BookingException("Seat is not available. Please choose another seat.");
                    }
                    
                    char confirm;
                    cout << "\nConfirm changes? (y/n): ";
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
                        
                        cout << "\nPassenger promoted successfully!\n";
                    } else {
                        cout << "\nPromotion cancelled.\n";
                    }
                    break;
                }
                case 2: {
                    // Delete passenger
                    string username;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    cout << "\nEnter username of passenger to delete: ";
                    getline(cin, username);
                    
                    char confirm;
                    cout << "\nConfirm deletion? (y/n): ";
                    cin >> confirm;
                    
                    if (tolower(confirm) == 'y') {
                        if (waitingList.removePassenger(username)) {
                            waitingList.saveToFile();
                            cout << "\nPassenger removed from waiting list successfully!\n";
                        } else {
                            throw ValidationException("Passenger not found in waiting list");
                        }
                    } else {
                        cout << "\nDeletion cancelled.\n";
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
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void manageUserAccounts() {
        clearScreen();
        cout << "\n===== USER ACCOUNTS =====\n";
        
        try {
            // Display all customer accounts
            cout << "Customer Accounts:\n";
            cout << setw(20) << "Username" << setw(30) << "Name" << "\n";
            cout << string(50, '-') << "\n";
            
            vector<User*> customers;
            for (auto& user : users) {
                if (!user->getIsAdmin()) {
                    customers.push_back(user);
                    cout << setw(20) << user->getUsername() << setw(30) << user->getName() << "\n";
                }
            }
            
            if (customers.empty()) {
                cout << "No customer accounts found.\n";
                pressEnterToContinue();
                return;
            }
            
            char deleteOption;
            cout << "\nDo you want to delete a user account? (y/n): ";
            cin >> deleteOption;
            
            if (tolower(deleteOption) == 'y') {
                string username;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                cout << "Enter username of account to delete: ";
                getline(cin, username);
                
                auto it = find_if(users.begin(), users.end(),
                                 [&username](const User* u) { return u->getUsername() == username && !u->getIsAdmin(); });
                
                if (it == users.end()) {
                    throw ValidationException("Customer account not found");
                }
                
                char confirm;
                cout << "\nConfirm delete (y/n): ";
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
                    
                    cout << "\nUser account deleted successfully!\n";
                } else {
                    cout << "\nDeletion cancelled.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
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
            cout << "\n===== CUSTOMER MENU =====\n";
            cout << "1. View Flights\n";
            cout << "2. View Booking\n";
            cout << "3. Cancel Booking\n";
            cout << "4. Logout\n";
            cout << "Enter your choice: ";
            
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
                    cout << "\nLogging out...\n";
                    break;
                default:
                    cout << "\nInvalid choice. Please try again.\n";
                    pressEnterToContinue();
            }
        } while (choice != 4);
    }
    
    void viewFlights() {
        clearScreen();
        cout << "\n===== VIEW FLIGHTS =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        try {
            // Display all flights
            cout << "Available Flights:\n";
            cout << setw(10) << "Flight ID" 
                 << setw(20) << "Airline" 
                 << setw(30) << "Destination" 
                 << setw(25) << "Departure Time" 
                 << setw(25) << "Arrival Time" 
                 << setw(15) << "Available Seats" << "\n";
            cout << string(125, '-') << "\n";
            
            for (const auto& flight : flights) {
                cout << setw(10) << flight.getFlightID()
                     << setw(20) << flight.getAirlineName()
                     << setw(30) << flight.getDestination()
                     << setw(25) << flight.getDepartureTime()
                     << setw(25) << flight.getArrivalTime()
                     << setw(15) << flight.getAvailableSeats() << "\n";
            }
            
            char bookOption;
            cout << "\nDo you want to book a flight? (y/n) or 0 to go back: ";
            cin >> bookOption;

            if(bookOption == '0'){
                return;
            }// added back feature
            
            if (tolower(bookOption) == 'y') {
                bookFlight();
            } else {
                pressEnterToContinue();
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
            pressEnterToContinue();
        }
    }
    
    void bookFlight() {
        cout << "\n===== BOOK FLIGHT =====\n";
        
        if (flights.empty()) {
            cout << "No flights available.\n";
            pressEnterToContinue();
            return;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        try {
            string destination;
            cout << "Enter your destination or 'b' to go back: ";
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
            cout << "\nFlights for destination " << destination << ":\n";
            cout << setw(5) << "No." 
                 << setw(10) << "Flight ID" 
                 << setw(20) << "Airline" 
                 << setw(25) << "Departure Time" 
                 << setw(25) << "Arrival Time" 
                 << setw(15) << "Available Seats" << "\n";
            cout << string(100, '-') << "\n";
            
            for (size_t i = 0; i < matchingFlights.size(); i++) {
                cout << setw(5) << i + 1
                     << setw(10) << matchingFlights[i]->getFlightID()
                     << setw(20) << matchingFlights[i]->getAirlineName()
                     << setw(25) << matchingFlights[i]->getDepartureTime()
                     << setw(25) << matchingFlights[i]->getArrivalTime()
                     << setw(15) << matchingFlights[i]->getAvailableSeats() << "\n";
            }
            
            int flightIndex;
            cout << "\nChoose flight. Enter flight number: ";
            
            cout<<""<<endl;

            cout<< "0. Back to Main Menu\n";//added back option

            cin >> flightIndex;

            if(flightIndex == 0){
                return;
            } //added back feature
            
            if (flightIndex < 1 || flightIndex > static_cast<int>(matchingFlights.size())) {
                throw ValidationException("Invalid flight number");
            }
            
            Flight* selectedFlight = matchingFlights[flightIndex - 1];
            
            if (selectedFlight->isFullyBooked()) {
                cout << "\nThis flight is fully booked.\n";
                
                char waitingListOption;
                cout << "Do you want to be added to the waiting list? (y/n): ";
                cin >> waitingListOption;
                
                if (tolower(waitingListOption) == 'y') {
                    // Add to waiting list
                    if (waitingLists.find(selectedFlight->getFlightID()) == waitingLists.end()) {
                        waitingLists[selectedFlight->getFlightID()] = WaitingList(selectedFlight->getFlightID());
                    }
                    
                    waitingLists[selectedFlight->getFlightID()].addPassenger(getUsername(), getName());
                    waitingLists[selectedFlight->getFlightID()].saveToFile();
                    
                    cout << "\nYou have been added to the waiting list for this flight.\n";
                }
                
                pressEnterToContinue();
                return;
            }
            
            // Show seat map
            selectedFlight->displaySeatMap();
            
            string seatNumber;
            cout << "\nEnter seat number (e.g., 1A) or 'b' to go back: ";
            cin >> seatNumber;

            if(seatNumber == "B" || seatNumber == "b"){
                return;
            } // added back feature
            
            if (!selectedFlight->isSeatAvailable(seatNumber)) {
                throw BookingException("Seat is not available. Please choose another seat.");
            }
            
            // Choose payment method - Strategy Pattern implementation
            int paymentMethod;
            cout << "\nChoose payment method:\n";
            cout << "1. GCash\n";
            cout << "2. Card\n";
            cout << "0. Back to Main Menu\n"; // added back option
            cout << "Enter your choice: ";
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
                cout << "\nEnter GCash number: ";
                getline(cin, gcashNumber);
                
                paymentStrategy = make_unique<GCashPaymentStrategy>(gcashNumber);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else if (paymentMethod == 2) {
                // Card
                string cardNumber, expiryDate, cvv;
                
                cout << "\nEnter Card number: ";
                getline(cin, cardNumber);
                
                cout << "Enter expiration date (MM/YY): ";
                getline(cin, expiryDate);
                
                cout << "Enter CVV: ";
                getline(cin, cvv);
                
                paymentStrategy = make_unique<CreditCardPaymentStrategy>(cardNumber, expiryDate, cvv);
                paymentDetails = paymentStrategy->getPaymentDetails();
            } else {
                throw ValidationException("Invalid payment method");
            }
            
            // Process payment using the selected strategy
            double flightPrice = 500.00; // Example price
            
            char confirm;
            cout << "\nConfirm payment of $" << fixed << setprecision(2) << flightPrice << "? (y/n): ";
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
                
                cout << "\nPayment successful! Your flight has been booked.\n";
            } else {
                cout << "\nPayment cancelled. Booking not completed.\n";
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void viewBooking() {
        clearScreen();
        cout << "\n===== VIEW BOOKING =====\n";
        
        try {
            // Find reservations for this customer
            vector<Reservation> customerReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getUsername() == getUsername()) {
                    customerReservations.push_back(reservation);
                }
            }
            
            if (customerReservations.empty()) {
                cout << "You have no bookings.\n";
                pressEnterToContinue();
                return;
            }
            
            // Display bookings
            cout << "Your Bookings:\n";
            cout << setw(15) << "Reservation ID" 
                 << setw(15) << "Flight ID" 
                 << setw(20) << "Airline" 
                 << setw(30) << "Destination" 
                 << setw(15) << "Seat Number" 
                 << setw(15) << "Status" << "\n";
            cout << string(110, '-') << "\n";
            
            for (const auto& reservation : customerReservations) {
                cout << setw(15) << reservation.getReservationID()
                     << setw(15) << reservation.getFlightID()
                     << setw(20) << reservation.getAirlineName()
                     << setw(30) << reservation.getDestination()
                     << setw(15) << reservation.getSeatNumber()
                     << setw(15) << reservation.getStatus() << "\n";
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
        }
        
        pressEnterToContinue();
    }
    
    void cancelBooking() {
        clearScreen();
        cout << "\n===== CANCEL BOOKING =====\n";
        
        try {
            // Find reservations for this customer
            vector<Reservation> customerReservations;
            for (const auto& reservation : reservations) {
                if (reservation.getUsername() == getUsername()) {
                    customerReservations.push_back(reservation);
                }
            }
            
            if (customerReservations.empty()) {
                cout << "You have no bookings to cancel.\n";
                pressEnterToContinue();
                return;
            }
            
            // Display bookings
            cout << "Your Bookings:\n";
            cout << setw(5) << "No." 
                 << setw(15) << "Reservation ID" 
                 << setw(15) << "Flight ID" 
                 << setw(20) << "Airline" 
                 << setw(30) << "Destination" 
                 << setw(15) << "Seat Number" << "\n";
            cout << string(100, '-') << "\n";
            
            for (size_t i = 0; i < customerReservations.size(); i++) {
                cout << setw(5) << i + 1
                     << setw(15) << customerReservations[i].getReservationID()
                     << setw(15) << customerReservations[i].getFlightID()
                     << setw(20) << customerReservations[i].getAirlineName()
                     << setw(30) << customerReservations[i].getDestination()
                     << setw(15) << customerReservations[i].getSeatNumber() << "\n";
            }
            
            int bookingIndex;
            cout << "\nEnter booking number to cancel or 0 to go back: ";
            cin >> bookingIndex;

            if(bookingIndex == 0){
                return;
            } // added back feature
            
            if (bookingIndex < 1 || bookingIndex > static_cast<int>(customerReservations.size())) {
                throw ValidationException("Invalid booking number");
            }
            
            Reservation& selectedReservation = customerReservations[bookingIndex - 1];
            
            char confirm;
            cout << "\nConfirm cancellation? (y/n): ";
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
                
                cout << "\nBooking has been successfully cancelled.\n";
            } else {
                cout << "\nCancellation cancelled.\n";
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
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
        cerr << "Error loading users: " << e.what() << endl;
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
        cerr << "Error initializing system: " << e.what() << endl;
        exit(1);
    }
}

void signUp() {
    clearScreen();
    cout << "\n===== SIGN UP =====\n";

    try {
        char userType;
        cout << "Sign up as:\n";
        cout << "1. Admin\n";
        cout << "2. Customer\n";
        cout << "3. back to Main Menu\n";
        cout << "Enter your choice (0/1/2): ";
        cin >> userType;

        if (userType == '3') {
            return;
        }

        if (userType != '1' && userType != '2') {
            throw ValidationException("Invalid choice");
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string username, password, confirmPassword, name;

        cout << "\nEnter username: ";
        getline(cin, username);
        if (username.empty()) {
            throw ValidationException("Username cannot be empty");
        }
        if (User::usernameExists(username)) {
            throw ValidationException("Username already exists. Please choose another one");
        }

        cout << "Enter password: ";
        getline(cin, password);
        if (password.empty()) {
            throw ValidationException("Password cannot be empty");
        }

        cout << "Confirm password: ";
        getline(cin, confirmPassword);
        if (password != confirmPassword) {
            throw ValidationException("Passwords do not match");
        }

        cout << "Enter your full name: ";
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

        cout << "\nSign up successful! You can now log in.\n";
    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
    }

    pressEnterToContinue();
}


void logIn() {
    clearScreen();
    cout << "\n===== LOG IN =====\n";

    try {
        char userType;
        cout << "Log in as:\n";
        cout << "1. Admin\n";
        cout << "2. Customer\n";
        cout << "3. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> userType;

        if (userType == '3') {
            return;
        }

        if (userType != '1' && userType != '2') {
            throw ValidationException("Invalid choice");
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string username, password;

        cout << "\nEnter username: ";
        getline(cin, username);

        cout << "Enter password: ";
        getline(cin, password);

        User* user = User::login(username, password);
        if (user == nullptr) {
            throw ValidationException("Invalid username or password");
        }

        bool isAdmin = user->getIsAdmin();
        if ((userType == '1' && !isAdmin) || (userType == '2' && isAdmin)) {
            throw ValidationException("Invalid user type for this account");
        }

        cout << "\nLogin successful! Welcome, " << user->getName() << "!\n";
        pressEnterToContinue();

        currentUser = user;
        currentUser->displayMenu();
        currentUser = nullptr;
    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        pressEnterToContinue();
    }
}


int main() {
    // Initialize the system
    initializeSystem();
    
    int choice;
    do {
        clearScreen();
        cout << "\n===== AIRLINE RESERVATION SYSTEM =====\n";
        cout << "1. Sign Up\n";
        cout << "2. Log In\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        
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
                cout << "\nThank you for using the Airline Reservation System. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
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
