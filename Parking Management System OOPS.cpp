#include <iostream>
#include <fstream>
#include <stdexcept>
#include <conio.h>
#include <cstdlib> // Necessary for system("cls")
#include <string>

using namespace std;

const int MAX_ROWS = 100;
const int MAX_COLUMNS = 100;

//================ ABSTRACT CLASS =================
class Vehicle
{
protected:
    string numberPlate;
    string make;
    string model;
    string color;
    float charges;

public:
    Vehicle()
    {
        numberPlate = "";
        make = "";
        model = "";
        color = "";
        charges = 0;
    }

    virtual void displayDetails() = 0;
    virtual ~Vehicle() {}
};

//================ DERIVED CLASS =================
class ParkedVehicle : public Vehicle
{
public:
    ParkedVehicle() : Vehicle() {}

    void setData(string np, string mk, string md, string clr, float ch)
    {
        numberPlate = np;
        make = mk;
        model = md;
        color = clr;
        charges = ch;
    }

    string getNumberPlate() { return numberPlate; }
    string getMake()        { return make; }
    string getModel()       { return model; }
    string getColor()       { return color; }
    float getCharges()      { return charges; }

    bool isEmpty() { return numberPlate == ""; }

    void clear()
    {
        numberPlate = "";
        make = "";
        model = "";
        color = "";
        charges = 0;
    }

    void displayDetails() 
    {
        cout << "\n===================================" << endl;
        cout << "Vehicle Details" << endl;
        cout << "===================================" << endl;
        cout << "Number Plate : " << numberPlate << endl;
        cout << "Make         : " << make << endl;
        cout << "Model        : " << model << endl;
        cout << "Color        : " << color << endl;
        cout << "Charges      : $" << charges << endl;
        cout << "===================================" << endl;
    }
};

//================ FILE MANAGER =================
class FileManager
{
public:
    static void writeLog(string data)
    {
        ofstream file("logs.txt", ios::app);
        if(file) file << data << endl;
        file.close();
    }

    static void addHistory(string data)
    {
        ofstream file("history.txt", ios::app);
        if(file) file << data << endl;
        file.close();
    }

    static void saveParking(ParkedVehicle parking[][MAX_COLUMNS], int rows, int columns)
    {
        ofstream file("parkingData.txt");
        if(!file) return;

        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < columns; j++)
            {
                if(!parking[i][j].isEmpty())
                {
                    file << i << "\n"
                         << j << "\n"
                         << parking[i][j].getNumberPlate() << "\n"
                         << parking[i][j].getMake() << "\n"
                         << parking[i][j].getModel() << "\n"
                         << parking[i][j].getColor() << "\n"
                         << parking[i][j].getCharges() << "\n";
                }
            }
        }
        file.close();
    }

    static void loadParking(ParkedVehicle parking[][MAX_COLUMNS], int &occupied)
    {
        ifstream file("parkingData.txt");
        if(!file) return;

        occupied = 0;
        int row, col;
        string plate, make, model, color, tempCharges;
        float charges;

        while(file >> row >> col)
        {
            file.ignore(); // Clean up newline after integers
            
            // Check if file unexpectedly ended mid-entry
            if (!getline(file, plate)) break;
            if (!getline(file, make)) break;
            if (!getline(file, model)) break;
            if (!getline(file, color)) break;
            if (!getline(file, tempCharges)) break;

            // Safe parsing block for string-to-float conversion
            if (!tempCharges.empty()) {
                try {
                    charges = stof(tempCharges);
                } catch (const exception &e) {
                    charges = 0.0f; // Fallback default value if parsing fails
                }
            } else {
                charges = 0.0f;
            }

            // Ensure indices match bounds before updating array
            if (row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLUMNS) {
                parking[row][col].setData(plate, make, model, color, charges);
                occupied++;
            }
        }
        file.close();
    }
};

//================ PARKING LOT =================
class ParkingLot
{
private:
    ParkedVehicle parking[MAX_ROWS][MAX_COLUMNS];
    int rows;
    int columns;
    int total;
    int occupied;
    float vehicleCharges[3];

public:
    ParkingLot(int r, int c)
    {
        rows = r;
        columns = c;
        total = rows * columns;
        occupied = 0;

        vehicleCharges[0] = 3.0;
        vehicleCharges[1] = 5.0;
        vehicleCharges[2] = 1.5;

        FileManager::loadParking(parking, occupied);
    }

    void displayParking()
    {
        cout << "\n     ";
        for(int j = 0; j < columns; j++)
            cout << "C" << j + 1 << "  ";
        cout << endl;

        for(int i = 0; i < rows; i++)
        {
            cout << "R" << i + 1 << "  ";
            for(int j = 0; j < columns; j++)
            {
                if(parking[i][j].isEmpty())
                    cout << "[ ] ";
                else
                    cout << "[X] ";
            }
            cout << endl;
        }
    }

    void addVehicle(int row, int column)
    {
        try
        {
            if(!parking[row][column].isEmpty())
                throw runtime_error("Parking spot already occupied!");

            string plate, make, model, color;
            int type, days;

            cout << "\nEnter Vehicle Details\n";
            
            cin.ignore(1000, '\n'); 

            cout << "Number Plate : ";  getline(cin, plate);
            cout << "Make         : ";  getline(cin, make);
            cout << "Model        : ";  getline(cin, model);
            cout << "Color        : ";  getline(cin, color);

            cout << "\nVehicle Type\n1. LTV\n2. HTV\n3. Others\nChoice : ";
            cin >> type;

            if(type < 1 || type > 3)
                throw invalid_argument("Invalid Vehicle Type!");

            cout << "Days : ";
            cin >> days;

            if(days <= 0)
                throw invalid_argument("Days must be greater than zero.");

            float charge = vehicleCharges[type - 1] * days;
            parking[row][column].setData(plate, make, model, color, charge);
            occupied++;

            FileManager::writeLog("Vehicle " + plate + " Entered.");
            FileManager::addHistory("Vehicle " + plate + " Parked.");
            FileManager::saveParking(parking, rows, columns);

            cout << "\nVehicle Added Successfully.\n";
        }
        catch(exception &e)
        {
            cout << "\nError : " << e.what() << endl;
        }
    }

    void removeVehicle(int row, int column)
    {
        try
        {
            if(parking[row][column].isEmpty())
                throw runtime_error("Parking Spot Empty!");

            string plate = parking[row][column].getNumberPlate();
            parking[row][column].clear();
            occupied--;

            FileManager::writeLog("Vehicle " + plate + " Removed.");
            FileManager::saveParking(parking, rows, columns);

            cout << "\nVehicle Removed Successfully.\n";
        }
        catch(exception &e)
        {
            cout << "\nError : " << e.what() << endl;
        }
    }

    void showVehicleDetails(int row, int column)
    {
        if(parking[row][column].isEmpty())
        {
            cout << "\nNo Vehicle Found.\n";
            return;
        }
        Vehicle *v = &parking[row][column];
        v->displayDetails();
    }

    void showStatus()
    {
        cout << "\n========== PARKING STATUS ==========\n";
        cout << "Total Spaces : " << total << endl;
        cout << "Occupied     : " << occupied << endl;
        cout << "Available    : " << total - occupied << endl;
    }

    void resetParkingLot()
    {
        for(int i = 0; i < rows; i++)
            for(int j = 0; j < columns; j++)
                parking[i][j].clear();

        occupied = 0;
        FileManager::saveParking(parking, rows, columns);
        FileManager::writeLog("Parking Lot Reset.");
        cout << "\nParking Lot Reset Successfully.\n";
    }

    int getRows()    { return rows; }
    int getColumns() { return columns; }
};

//================ MAIN =================
int main()
{
    try
    {
        int rows, columns;
        cout << "=====================================\n";
        cout << "     PARKING MANAGEMENT SYSTEM\n";
        cout << "=====================================\n\n";

        cout << "Enter Number of Rows (Max " << MAX_ROWS << "): ";
        cin >> rows;
        cout << "Enter Number of Columns (Max " << MAX_COLUMNS << "): ";
        cin >> columns;

        if (rows <= 0 || columns <= 0 || rows > MAX_ROWS || columns > MAX_COLUMNS)
            throw invalid_argument("Rows/Columns must be between 1 and 100.");

        ParkingLot lot(rows, columns);
        int choice;

        while (true)
        {
            system("cls");
            cout << "=====================================\n";
            cout << "     PARKING MANAGEMENT SYSTEM\n";
            cout << "=====================================\n";
            lot.displayParking();

            cout << "\n1. Add Vehicle\n2. Remove Vehicle\n3. Show Parking Status\n"
                 << "4. Show Vehicle Details\n5. Reset Parking Lot\n6. Exit\n";
            cout << "\nEnter Choice : ";
            cin >> choice;

            if (choice == 6)
            {
                cout << "\nThank You!\n";
                break;
            }

            int row, column;
            if (choice == 1 || choice == 2 || choice == 4)
            {
                cout << "Enter Row (1-" << lot.getRows() << ") : ";
                cin >> row;
                cout << "Enter Column (1-" << lot.getColumns() << ") : ";
                cin >> column;

                row--; column--; // Shift down for array indexes

                if (row < 0 || row >= lot.getRows() || column < 0 || column >= lot.getColumns())
                {
                    cout << "\nInvalid Parking Position!\n";
                    getch();
                    continue;
                }
            }

            switch (choice)
            {
                case 1: lot.addVehicle(row, column); break;
                case 2: lot.removeVehicle(row, column); break;
                case 3: lot.showStatus(); break;
                case 4: lot.showVehicleDetails(row, column); break;
                case 5: lot.resetParkingLot(); break;
                default: cout << "\nInvalid Choice!\n";
            }
            cout << "\nPress Any Key To Continue...";
            getch();
        }
    }
    catch (exception &e)
    {
        cout << "\nFatal Error : " << e.what() << endl;
    }
    return 0;
}