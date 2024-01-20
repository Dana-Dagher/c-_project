#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <memory>
#include<iomanip>
#include <unordered_map>
using namespace std;

class Date {
private:
    int day, month, year;

public:
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    friend ostream& operator<<(ostream& os, const Date& date) {
        os << setw(2) << setfill('0') << date.day << '/'
            << setw(2) << setfill('0') << date.month << '/'
            << setw(4) << setfill('0') << date.year;
        return os;
    }
};


class Passenger {
private:
    string name;
    int age;
    string contactInfo;

public:
    Passenger(string n, int a, string contact) : name(n), age(a), contactInfo(contact) {}

    string getName() const { return name; }
    int getAge() const { return age; }
    string getContactInfo() const { return contactInfo; }
    void setContactInfo(const string& contact) { contactInfo = contact; }
};


class PaymentMethod {
private:
    string method;

public:
    PaymentMethod(string m) : method(m) {}

    string getMethod() const { return method; }
};


class Flight {
private:
    string flightNumber;
    string departureLocation;
    string arrivalLocation;
    string departureTime;
    int availableSeats;
    string date;
    double ticketPrice;
    vector<string> bookedPassengers;

public:
    Flight(string fNumber, string depLocation, string arrLocation, string depTime, int seats, string d, double price)
        : flightNumber(fNumber), departureLocation(depLocation), arrivalLocation(arrLocation),
        departureTime(depTime), availableSeats(seats), date(d), ticketPrice(price) {}

    string getFlightNumber() const { return flightNumber; }
    string getDepartureLocation() const { return departureLocation; }
    string getArrivalLocation() const { return arrivalLocation; }
    string getDepartureTime() const { return departureTime; }
    int getAvailableSeats() const { return availableSeats; }
    string getDate() const { return date; }

    bool bookSeat(const string& passengerName) {
        if (availableSeats > 0) {
            availableSeats--;
            bookedPassengers.push_back(passengerName);
            return true;
        }
        return false;
    }

    double getTicketPrice() const { return ticketPrice; }

};


class Booking {
private:
    string flightNumber;
    Date date;
    int seatsBooked;

public:
    Booking(const string& fNumber, const Date& d, int seats) : flightNumber(fNumber), date(d), seatsBooked(seats) {}
    Date getDate() const { return date; }
    string getFlightNumber() const { return flightNumber; }
    int getSeatsBooked() const { return seatsBooked; }

    friend ostream& operator<<(ostream& os, const Booking& booking) {
        os << "Flight: " << booking.flightNumber << " Date: " << booking.date << " Seats: " << booking.seatsBooked;
        return os;
    }

};


class BookingSystem {
private:
    vector<Flight> flights;
    vector<unique_ptr<Passenger>> passengers;
    vector<Booking> bookings;
    vector<string> bookingRecords;
    unordered_map<string, vector<Booking>> passengerBookings;
    unordered_map<string, bool> userPayments;
    unordered_map<string, double> creditCardInfo;

    void savePassengerInfoAndBooking(const string& passengerName, int age, const string& contactInfo, const string& flightNumber, const Date& date, int seatsBooked)
    {
        ofstream passengerFile("passenger_info_and_bookings.txt", ios::app);

        if (passengerFile.is_open()) {
            auto passengerKey = passengerName + to_string(age) + contactInfo;
            vector<Booking>& prevBookings = passengerBookings[passengerKey];

            passengerFile << "Passenger Name: " << passengerName << endl;
            passengerFile << "Age: " << age << endl;
            passengerFile << "Contact Info: " << contactInfo << endl;
            passengerFile << "Flight: " << flightNumber << " Date: " << date << endl << endl;

            passengerFile << "Previous bookings:" << endl;
            if (prevBookings.empty()) {
                passengerFile << "no previous bookings." << endl;
            }
            else {
                for (const auto& prevBooking : prevBookings) {
                    passengerFile << "Flight: " << prevBooking.getFlightNumber() << " Date: " << prevBooking.getDate() << endl;
                }
            }

            passengerFile << "New Booking:" << endl;
            passengerFile << "Flight: " << flightNumber << " Date: " << date << endl << endl;

            prevBookings.emplace_back(flightNumber, date, seatsBooked);

            passengerFile.close();
        }
        else {
            cout << "Unable to open file to save passenger information and booking." << endl;
        }
    }
   
    bool validateCreditCard(const string& creditCardNumber) {
        return creditCardInfo.find(creditCardNumber) != creditCardInfo.end();
    }

    bool validateDeposit(const string& creditCardNumber, double totalAmount) {
        if (creditCardInfo.find(creditCardNumber) != creditCardInfo.end()) {
            double deposit = creditCardInfo[creditCardNumber];
            return deposit >= totalAmount;
        }
        return false;
    }

    void updateDeposit(const string& creditCardNumber, double amount) {
        auto it = creditCardInfo.find(creditCardNumber);
        if (it != creditCardInfo.end()) {
            it->second -= amount;
        }
    }

public:
    void loadCreditCardInfo(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                istringstream iss(line);
                string creditCardNumber;
                double deposit;
                iss >> creditCardNumber >> deposit;
                creditCardInfo[creditCardNumber] = deposit;
            }
            file.close();
        }
        else {
            cout << "Unable to open file " << filename << endl;
        }
    }

    void loadFlightsFromFile(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                istringstream iss(line);
                string fNumber, depLocation, arrLocation, depTime, date;
                int seats;
                double price;
                iss >> fNumber >> depLocation >> arrLocation >> depTime >> seats >> date >> price;
                flights.emplace_back(fNumber, depLocation, arrLocation, depTime, seats, date, price);
            }
            file.close();
        }
        else {
            cout << "Unable to open file " << filename << endl;
        }
    }

    bool showAvailableFlights(const string& departure, const string& arrival, const string& desiredDate, int seatsNeeded) {
        string lowerDeparture = departure;
        string lowerArrival = arrival;
        transform(lowerDeparture.begin(), lowerDeparture.end(), lowerDeparture.begin(), ::tolower);
        transform(lowerArrival.begin(), lowerArrival.end(), lowerArrival.begin(), ::tolower);
        //
        vector<Flight*> availableFlights;
        for (auto& flight : flights) {
            string lowerFlightDeparture = flight.getDepartureLocation();
            string lowerFlightArrival = flight.getArrivalLocation();
            transform(lowerFlightDeparture.begin(), lowerFlightDeparture.end(), lowerFlightDeparture.begin(), ::tolower);
            transform(lowerFlightArrival.begin(), lowerFlightArrival.end(), lowerFlightArrival.begin(), ::tolower);

            if (lowerFlightDeparture == lowerDeparture &&
                lowerFlightArrival == lowerArrival &&
                flight.getDate() == desiredDate &&
                flight.getAvailableSeats() >= seatsNeeded) {
                availableFlights.push_back(&flight);
            }

        }

        if (availableFlights.empty()) {
            cout << "No available flights matching your criteria." << endl;
            return false;
        }
        else {
            cout << "Available flights:" << endl;
            for (size_t i = 0; i < availableFlights.size(); ++i) {
                cout << i + 1 << ". " << availableFlights[i]->getDepartureLocation() << " to "
                    << availableFlights[i]->getArrivalLocation() << " at "
                    << availableFlights[i]->getDepartureTime() << " on "
                    << availableFlights[i]->getDate() << " with "
                    << availableFlights[i]->getAvailableSeats() << " seats available" << endl;
            }
            return true;

        }
    }

    void processPayment(const string& passengerName, const int& paymentMethod, double totalAmount) {
        cout << "Processing payment for passenger: " << passengerName << endl;
        cout << "Total Amount: $" << totalAmount << endl;

        string creditCardNumber;
        //here
        if (paymentMethod < 4) {
            cout << "Enter your credit card number: ";
            cin >> creditCardNumber;

            if (!validateCreditCard(creditCardNumber)) {
                cout << "Invalid credit card number. Payment canceled. Exiting booking process." << endl;
                return;
            }

            if (!validateDeposit(creditCardNumber, totalAmount)) {
                cout << "Insufficient balance. Payment canceled. Exiting booking process." << endl;
                return;
            }

            updateDeposit(creditCardNumber, totalAmount);
        }
        cout << "Payment successful. Enjoy your flight!" << endl;
    }

    bool bookChosenFlight(const string& departure, const string& arrival, const string& desiredDate, int seatsNeeded, const string& passengerName) {

        vector<Flight*> availableFlights;
        for (auto& flight : flights) {
            string lowerFlightDeparture = flight.getDepartureLocation();
            string lowerFlightArrival = flight.getArrivalLocation();
            transform(lowerFlightDeparture.begin(), lowerFlightDeparture.end(), lowerFlightDeparture.begin(), ::tolower);
            transform(lowerFlightArrival.begin(), lowerFlightArrival.end(), lowerFlightArrival.begin(), ::tolower);

            if (lowerFlightDeparture == departure &&
                lowerFlightArrival == arrival &&
                flight.getDate() == desiredDate &&
                flight.getAvailableSeats() >= seatsNeeded) {
                availableFlights.push_back(&flight);
            }
        }
        int choice;
        cout << "Choose a flight: ";
        cin >> choice;

        Flight* chosenFlight = availableFlights[choice - 1];

        
        int seatsToBook;
        int originalSeatsNeeded = seatsNeeded;

        do {
            cout << "Enter number of seats to book (maximum " << chosenFlight->getAvailableSeats() << " available): ";
            cin >> seatsToBook;

            if (seatsToBook > chosenFlight->getAvailableSeats() || seatsToBook <= 0) {
                cout << "Invalid number of seats. Please choose a valid number within the available limit." << endl;
            }

        } while (seatsToBook > chosenFlight->getAvailableSeats() || seatsToBook != originalSeatsNeeded);

        for (int i = 0; i < seatsToBook; ++i) {
            cout << "Enter passenger's name for seat " << i + 1 << ": ";
            string name;
            cin >> name;

            cout << "Enter passenger's age: ";
            int age;
            cin >> age;

            cout << "Enter passenger's contact information: ";
            string contactInfo;
            cin >> contactInfo;

            unique_ptr<Passenger> passenger = make_unique<Passenger>(name, age, contactInfo);
            passengers.push_back(move(passenger));

            chosenFlight->bookSeat(name);

            bool found = false;
            for (auto& record : bookingRecords) {
                if (record == name) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                bookingRecords.push_back(name);
            }
            string dateString = chosenFlight->getDate();
            istringstream dateStream(dateString);

            int month, day, year;
            char discard;
            dateStream >> month >> discard >> day >> discard >> year;


            Date chosenDate(day, month, year);
            double totalAmount = chosenFlight->getTicketPrice() * seatsToBook;
            int paymentChoice;

            if (!userPayments[passengerName]) {
                bool isValidDeposit = false;

                do {
                    cout << "Total Amount: $" << fixed << setprecision(2) << totalAmount << endl;
                    cout << "Select payment method:" << endl;
                    cout << "1. PayPal" << endl;
                    cout << "2. MasterCard" << endl;
                    cout << "3. VisaCard" << endl;
                    cout << "4. Cash at Airport" << endl;

                    //int paymentChoice;
                    cout << "Enter your choice (1-4): ";
                    cin >> paymentChoice;

                    switch (paymentChoice) {
                    case 1:
                    case 2:
                    case 3: {
                        string creditCardNumber;
                        cout << "Enter the serial number of your credit card: ";
                        cin >> creditCardNumber;

                        if (!validateCreditCard(creditCardNumber)) {
                            cout << "Invalid credit card number. Would you like to:" << endl;
                            cout << "1. Re-enter the serial code" << endl;
                            cout << "2. Cancel booking" << endl;
                            cout << "Enter your choice (1 or 2): ";
                            int choice;
                            cin >> choice;

                            if (choice == 2) {
                                cout << "Booking canceled. Exiting booking process." << endl;
                                return false;
                            }
                        }
                        else if (!validateDeposit(creditCardNumber, totalAmount)) {
                            cout << "Insufficient balance. Would you like to:" << endl;
                            cout << "1. Cancel booking" << endl;
                            cout << "2. Choose another payment method" << endl;
                            cout << "Enter your choice (1 or 2): ";
                            int choice;
                            cin >> choice;

                            if (choice == 1) {
                                cout << "Booking canceled. Exiting booking process." << endl;
                                return false;
                            }
                        }
                        else {
                            isValidDeposit = true;
                        }
                        break;
                    }
                    case 4:
                        cout << "Payment Method: Cash at Airport" << endl;
                        isValidDeposit = true;
                        break;
                    default:
                        cout << "Invalid Choice. Please choose a valid payment method." << endl;
                        break;
                    }
                } while (!isValidDeposit);
            }
          
            processPayment(name, paymentChoice, totalAmount);
            userPayments[passengerName] = true;
            bookings.emplace_back(chosenFlight->getFlightNumber(), chosenDate, seatsToBook);

            savePassengerInfoAndBooking(name, age, contactInfo, chosenFlight->getFlightNumber(), chosenDate, seatsToBook);

        }

        cout << "Booking successful. Enjoy your flight!" << endl;
        return true;
    }

};



int main() {

    BookingSystem bookingSystem;
    bookingSystem.loadFlightsFromFile("flights.txt");
    bookingSystem.loadCreditCardInfo("bankFile.txt");
    int numUsers;

    cout << "Enter the number of users: ";
    cin >> numUsers;

    for (int i = 0; i < numUsers; ++i)
    {
        string departure, arrival, desiredDate, passengerName;
        int seatsNeeded;

        cout << "user:" << i + 1 << endl;
        cout << "Enter departure location: ";
        cin >> departure;
        transform(departure.begin(), departure.end(), departure.begin(), ::tolower);
        cout << "Enter arrival location: ";
        cin >> arrival;
        transform(arrival.begin(), arrival.end(), arrival.begin(), ::tolower);
        cout << "Enter desired date (format: mm/dd/yyyy): ";
        cin >> desiredDate;
        cout << "Enter number of seats needed: ";
        cin >> seatsNeeded;
        cout << "Enter passenger name: ";
        cin >> passengerName;

        bool flightsAvailable = bookingSystem.showAvailableFlights(departure, arrival, desiredDate, seatsNeeded);

        if (flightsAvailable) {
            bookingSystem.bookChosenFlight(departure, arrival, desiredDate, seatsNeeded, passengerName);
        }
        else {
            cout << "Skipping user " << i + 1 << " as there are no available flights matching the criteria." << endl;
        }
    }

    cout << "End of reservation, thank you" << endl;

    return 0;
}