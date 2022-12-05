#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include "Token.h"

using namespace std;

#ifndef FILEWRITER_H
#define FILEWRITER_H

class FileWriter {
public:
    FileWriter();
    void printTransitionTable(map<string, map<string, string>> transitions);
    void printAcceptingStates(map<string, string> acceptingStates);
    void printResult(string filePath, vector<Token> tokens);
};

#endif //FILEWRITER_H
