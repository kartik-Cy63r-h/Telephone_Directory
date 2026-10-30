# Telephone Directory Management System


## 1. Project Overview

This project implements a Telephone Directory Management System using C++. It uses hash tables and linear probing to store and manage contact information efficiently.

Each contact contains a name, a 7-digit phone number, an address, and a slot state.

## 2. Data Structures Used

The project uses the following structures and classes:

- **Contact:** Stores name, phone number, address and state.
- **slotState:** Maintains three states: available, in_use and removed.
- **result:** Stores operation success status and message.
- **directoryMap:** Implements a hash table containing 20 contact slots.
- **telephoneDirectory:** Manages nine hash tables and the main directory operations.
- **explicitStringArray:** Temporarily stores words read from input commands.

## 3. Hashing and Collision Resolution

The telephone directory contains 9 separate hash tables, each having 20 slots.

- The first digit of the phone number determines the hash table.
- The remaining six digits are used to calculate the hash value.

**Hash function:**

h(x) = x % 20

**Collision Resolution:** Linear Probing

When a slot is occupied, the next available slot is checked using:

(hash + 1) % 20

## 4. Main Operations

The system supports:

- **Insert:** Adds a new contact after validating the phone number and checking for duplicates.
- **Delete:** Deletes a contact by marking its slot as removed.
- **Modify:** Updates the address or changes the phone number of an existing contact.
- **Print:** Displays the telephone directory tables.

## 5. File Input and Output

The program reads commands from sequential input files:

- input1.txt
- input2.txt
- input3.txt
- input4.txt

It generates corresponding output files:

- output1.txt
- output2.txt
- output3.txt
- output4.txt

Each input file is processed independently using a fresh telephone directory.

## 6. Compilation and Execution

**Compile the program:**

```bash
g++ t_directory.cpp -o telephone
```

**Run the program:**

```bash
./telephone
```

Make sure the input files are present in the same working directory.


