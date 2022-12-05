#include "FileWriter.h"

FileWriter::FileWriter() = default;

void FileWriter::printTransitionTable(map<string, map<string, string>> transitions) {
    ofstream file("Transition_Table.txt");
    int currStateNumber = 0;
    auto currTransistion = transitions.find("S" + to_string(currStateNumber));
    while (currTransistion != transitions.end()) {
        file << setw(12) << left << currTransistion->first;
        for (auto j : currTransistion->second) {
            string stateOut = j.first + " -> " + ((j.second.empty()) ? "X" : j.second);
            file << setw(20) << left << stateOut;
        }
        file << endl;
        currTransistion = transitions.find("S" + to_string(++currStateNumber));
    }
    file.close();
}

void FileWriter::printAcceptingStates(map<string, string> acceptingStates) {
    ofstream file("Accepting_States.txt");
    file << setw(12) << left <<  "State" << setw(12) << left << "Type" << endl;
    for (auto accState : acceptingStates) {
        file << setw(12) << left <<  accState.first << setw(12) << left << accState.second << endl;
    }
    file.close();
}

void FileWriter::printResult(string filePath, vector<Token> tokens) {
    ofstream file(filePath);
    file << setw(12) << left << "Type" << setw(12) << left << "Lexeme" << endl;
    for (auto token : tokens) {
        file << setw(12) << left <<  token.GetType() << setw(12) << left << token.GetLexeme() << endl;
    }
    file.close();
}
