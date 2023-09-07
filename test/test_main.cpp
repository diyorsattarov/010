#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

map<string, int> symbolTable;

void initializeSymbolTable() {
    // Initialize the symbol table with predefined labels and addresses.
    symbolTable["R0"] = 0;
    symbolTable["R1"] = 1;
    symbolTable["R2"] = 2;
    // Add more predefined symbols as needed.
}

void parseLabels(ifstream& inputFile) {
    string line;
    int lineCounter = 0;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;
        iss >> token;

        if (token == "LABEL") {
            iss >> token;
            symbolTable[token] = lineCounter;
        } else {
            lineCounter++;
        }
    }

    inputFile.clear();
    inputFile.seekg(0, ios::beg);
}

string parseAInstruction(const string& instruction) {
    string address = instruction.substr(1);
    int value = stoi(address);

    return bitset<16>(value).to_string();
}

string parseCInstruction(const string& instruction) {
    // Implement parsing of C-instructions here.
    // Return the binary representation of the instruction.
    // Example: "D=A+1" -> "1110111111100001"
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: assembler input.asm output.hack" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);

    if (!inputFile || !outputFile) {
        cerr << "Error opening files." << endl;
        return 1;
    }

    initializeSymbolTable();
    parseLabels(inputFile);

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;
        iss >> token;

        if (token == "A") {
            string instruction;
            iss >> instruction;
            outputFile << "0" << parseAInstruction(instruction) << endl;
        } else if (token == "C") {
            string instruction;
            iss >> instruction;
            outputFile << "111" << parseCInstruction(instruction) << endl;
        }
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
