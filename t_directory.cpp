#include<iostream>
#include<fstream>
#include<string>


using namespace std;

const int tableSize = 20;


struct explicitStringArray{
    string elements[7];
    int size = 0;
    void add(const string& str) {
        if (size < 7) {
            elements[size++] = str;
        }
    }
    bool isEmpty() const {
        return size == 0;
    }

};


enum slotState { available, in_use, removed };

struct Contact {
    string name;
    string phone;
    string address;
    slotState state;

   Contact(){
    name = "";
    phone = "";
    address = "";
    state = available;
   }

   Contact& operator=(const Contact& other) {
        if (this != &other) {
            name = other.name;
            phone = other.phone;
            address = other.address;
            state = other.state;
        }
        return *this;
    }
};

struct result {
    bool success;
    string message;

    result() : success(false), message("") {}
    result(bool s, string m) : success(s), message(m) {}

    result& operator=(const result& other) {
        if (this != &other) {
            success = other.success;
            message = other.message;
        }
        return *this;
    }
};

class directoryMap {
    friend class telephoneDirectory;
private:
    Contact table[tableSize];

public:
    int getHash(const string& phone) const {
        int value = 0;
        for (int i = 1; i < 7; i++) {
            value = value * 10 + (phone[i] - '0');
        }
        return value % tableSize;
    }

    directoryMap() = default;

    result insertContact(const string& name, const string& phone, const string& address) {
        int hash = getHash(phone);
        int firstDeleted = -1;
        for (int probed = 0; probed < tableSize; ++probed) {
            if (table[hash].state == in_use && table[hash].phone == phone) return result(false, "Failure: Phone number already exists");  
            else if (table[hash].state == removed && firstDeleted == -1)  firstDeleted = hash; 
            else if (table[hash].state == available) {
                if (firstDeleted != -1) {
                    table[firstDeleted].name = name;
                    table[firstDeleted].phone = phone;
                    table[firstDeleted].address = address;
                    table[firstDeleted].state = in_use;
                    return result(true, "Success");
                } else {
                    table[hash].name = name;
                    table[hash].phone = phone;
                    table[hash].address = address;
                    table[hash].state = in_use;
                    return result(true, "Success");
                }
            }
            hash = (hash + 1) % tableSize; 
        }

        if (!(firstDeleted == -1)) {
            table[firstDeleted].name = name;
            table[firstDeleted].phone = phone;
            table[firstDeleted].address = address;
            table[firstDeleted].state = in_use;
            return result(true, "Success");
        }

        return result(false, "Failure: Table full");
    }

    int searchContact(const string& phone) const {
        int hash = getHash(phone);
        for (int probed = 0; probed < tableSize; ++probed) {
            if (table[hash].state == in_use && table[hash].phone == phone) return hash;      
            else if (table[hash].state == available) break;
            hash = (hash + 1) % tableSize;
        }
        return -1; 
    }

    result deleteContact(const string& phone) {
        int index = searchContact(phone);
        if (index != -1) {
            table[index].state = removed;
            return result(true, "Success");
        }
        return result(false, "Failure: Record not found");
    }

};

class telephoneDirectory {
private:
    directoryMap tables[9]; 
    bool isdigit(char c) const {
        return c >= '0' && c <= '9';
    }

    char getFirstDigit(const string& phone) const {
        return phone[0];
    }

    bool isValidPhone(const string& phone) const {
        int count = 0;
        for (char c : phone) {
            if (isdigit(c)) count++;
        }

        if(count == 7 && getFirstDigit(phone)>='1' && getFirstDigit(phone)<='9'){
            for(char c: phone){
                if(!isdigit(c)) return false;
            }
            return true;
        }
        return false;
    }

    int getTableIndex(const string& phone) const {
        return phone[0] - '1'; 
    }
    
public:

    void printTable(ofstream& fout) {
        for (int i = 0; i < 9; i++) {
            fout << "T " << (i + 1) << ":\n";
            for (int j = 0; j < tableSize; j++) {
                fout << j << ": ";
                if (tables[i].table[j].state == in_use) {
                    fout << tables[i].table[j].name << ", "
                         << tables[i].table[j].phone << ", "
                         << tables[i].table[j].address << "\n";
                } else {
                    fout << "empty\n";
                }
            }
        }
    }

    result addEntry(const string& name, const string& phone, const string& address) {
        if (isValidPhone(phone)) return tables[getTableIndex(phone)].insertContact(name, phone, address);
        return result(false, "Failure: Invalid phone number format.");  
    }

    result deleteEntry(const string& phone) {
        if (!isValidPhone(phone)) return result(false, "Failure: Invalid phone number format.");
        return tables[getTableIndex(phone)].deleteContact(phone);
    }

    result modifyEntry(const string& oldPhone, const string& newAddress, const string& newPhone = "") {
        if (!isValidPhone(oldPhone)) return result(false, "Failure: Invalid phone number format.");

        bool changePhone = (!newPhone.empty());
        if (changePhone && !isValidPhone(newPhone)) return result(false, "Failure: Invalid phone number format.");

        int oldIndex = getTableIndex(oldPhone);
        int recordPos = tables[oldIndex].searchContact(oldPhone);

        if (recordPos == -1) return result(false, "Failure: Old phone number not found.");    
        if (!changePhone) {
            tables[oldIndex].table[recordPos].address = newAddress;
            return result(true, "Success");
        } 
        
        int newIndex = getTableIndex(newPhone);
        if (tables[newIndex].searchContact(newPhone) != -1) return result(false, "Failure: New phone number already exists.");
            
        Contact oldRecord = tables[oldIndex].table[recordPos];
        tables[oldIndex].table[recordPos].state = removed; 
        result insertRes = tables[newIndex].insertContact(oldRecord.name, newPhone, newAddress);
        if (!insertRes.success) {
            tables[oldIndex].table[recordPos] = oldRecord; 
            return result(false, insertRes.message); 
        }
        return result(true, "Success");
    }

    void processCommands(const string& inFilename, const string& outFilename) {
        ifstream fin(inFilename);
        ofstream fout(outFilename);

        if (!fin.is_open()||!fout.is_open()) {
            cerr << "Error: Could not open files.\n";
            return;
        }
        string line;
        while (getline(fin, line)) {
            explicitStringArray tape;
            string word = "";
            for (char c : line) {
                if (c == ' '|| c == '\t'|| c == '\n'|| c == '\r') {
                    if (!word.empty()) {
                        tape.add(word);
                        word = "";
                    }
                } else {
                    word += c;
                }
            }
            if (!word.empty()) tape.add(word);
            if (tape.size == 0) continue;
            string command = tape.elements[0];
            result res(false, "Error: Unknown command.");
            if (command == "Insert" && tape.size == 4) {
                res = addEntry(tape.elements[1], tape.elements[2], tape.elements[3]);
            } else if (command == "Delete" && tape.size == 2) {
                res = deleteEntry(tape.elements[1]);
            } else if (command == "Modify" && (tape.size == 3 || tape.size == 4)) {
                string newPhone = (tape.size == 4) ? tape.elements[3] : "";
                res = modifyEntry(tape.elements[1], tape.elements[2], newPhone);
            }else if (command == "Print") {
                printTable(fout);
                continue;
            }
           fout<< res.message << "\n";
        } 
        fin.close();
        fout.close();
    }
};

int main() {
    
    int fileNumber = 1 ;
    bool foundFiles = false;
    while (true) {
        string inputFile = "input" + to_string(fileNumber) + ".txt";
        string outputFile = "output" + to_string(fileNumber) + ".txt";

        ifstream fin(inputFile);
        if (!fin.is_open()) break;
        fin.close();

        telephoneDirectory directory;
        
        cout << "Processing " << inputFile << "..." << endl;
        directory.processCommands(inputFile, outputFile);
        cout << "Finished. Check " << outputFile << " for results." << endl;
        foundFiles = true;
        fileNumber++;
    }
    if (!foundFiles) {
        cerr << "Error: No valid input files found." << endl;
    }
    else {
        cout << "All input files processed successfully." << endl;
    }
    return 0;
}