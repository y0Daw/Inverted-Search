# Inverted Search

## About the Project

Inverted Search is a file-based searching system implemented in C using **Hashing and Linked Lists**.

It reads multiple text files, extracts words, and maintains a database that records which files contain each word and how many times it appears.

Instead of searching through every file whenever a word is requested, the program uses a hash table to find the word efficiently.

## Features

- Create a database from multiple text files.
- Display the complete inverted index.
- Search for a word and find the files containing it.
- Save the database as a backup file.
- Load an existing database from a backup.
- Update the database by adding new text files.
- Check for duplicate filenames.
- Encode backup records using XOR and hexadecimal conversion.

## Data Structures Used

- **Hash Table:** Contains 28 indexes.
  - 0–25: Alphabetic characters (A–Z).
  - 26: Numeric characters (0–9).
  - 27: Special characters.
- **Main Linked List:** Stores unique words and their file counts.
- **Sub Linked List:** Stores filenames and word occurrence counts.
- **File Linked List:** Maintains the list of input files during database creation.

## Technologies Used

- C Programming
- Hashing
- Singly Linked Lists
- File Handling
- Dynamic Memory Allocation
- String Manipulation
- XOR Encoding
- Hexadecimal Conversion

## Compilation

Compile the source files using GCC:

```bash
gcc *.c -o inverted_search
```

## Execution

Pass the input text files as command-line arguments:

```bash
./inverted_search f1.txt f2.txt
```

## Menu Options

1. **Create Database** – Reads the input files and creates the inverted index.
2. **Display Database** – Displays words, file counts, filenames, and word counts.
3. **Search Data** – Searches for a word and displays its file details.
4. **Update Database** – Loads an existing backup and adds a new text file.
5. **Save Database** – Saves the current database into an encoded backup file.
6. **Exit** – Terminates the program.

## Backup

The database is saved in a text file using XOR encoding followed by hexadecimal conversion.

The saved backup can later be loaded to restore the database and continue updating it.

## Learning Outcomes

This project helped me understand hashing, linked-list operations, dynamic memory allocation, file handling, and implementing a practical search system using C.