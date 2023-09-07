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
    string binaryInstruction = "111"; // The three leading bits for C-instructions

    // Parse the destination (if present)
    size_t equalsPos = instruction.find('=');
    string destBits = "000";
    if (equalsPos != string::npos) {
        string dest = instruction.substr(0, equalsPos);
        if (dest.find('A') != string::npos) destBits[0] = '1';
        if (dest.find('D') != string::npos) destBits[1] = '1';
        if (dest.find('M') != string::npos) destBits[2] = '1';
    }

    // Parse the computation
    size_t compPos = equalsPos != string::npos ? equalsPos + 1 : 0;
    size_t semicolonPos = instruction.find(';');
    string comp;
    if (semicolonPos != string::npos) {
        comp = instruction.substr(compPos, semicolonPos - compPos);
    } else {
        comp = instruction.substr(compPos);
    }

    // Define the binary codes for common computations
    map<string, string> compCodes = {
        {"0", "0101010"},
        {"1", "0111111"},
        {"-1", "0111010"},
        {"D", "0001100"},
        {"A", "0110000"},
        {"!D", "0001101"},
        {"!A", "0110001"},
        {"-D", "0001111"},
        {"-A", "0110011"},
        {"D+1", "0011111"},
        {"A+1", "0110111"},
        {"D-1", "0001110"},
        {"A-1", "0110010"},
        {"D+A", "0000010"},
        {"D-A", "0010011"},
        {"A-D", "0000111"},
        {"D&A", "0000000"},
        {"D|A", "0010101"},
    };

    if (compCodes.find(comp) != compCodes.end()) {
        binaryInstruction += compCodes[comp];
    } else {
        cerr << "Error: Invalid computation: " << comp << endl;
        // Handle error or return an error code here.
    }

    // Parse the jump (if present)
    string jumpBits = "000";
    if (semicolonPos != string::npos) {
        string jump = instruction.substr(semicolonPos + 1);
        if (jump == "JGT") jumpBits = "001";
        else if (jump == "JEQ") jumpBits = "010";
        else if (jump == "JGE") jumpBits = "011";
        else if (jump == "JLT") jumpBits = "100";
        else if (jump == "JNE") jumpBits = "101";
        else if (jump == "JLE") jumpBits = "110";
        else if (jump == "JMP") jumpBits = "111";
        else {
            cerr << "Error: Invalid jump: " << jump << endl;
            // Handle error or return an error code here.
        }
    }

    binaryInstruction += destBits + compCodes[comp] + jumpBits;
    return binaryInstruction;
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
