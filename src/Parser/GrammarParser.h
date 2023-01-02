#ifndef LEXICAL_ANALYZER_GRAMMARPARSER_H
#define LEXICAL_ANALYZER_GRAMMARPARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../Utils/Component.h"

using namespace std;

class GrammarParser {
public:
    explicit GrammarParser(const string &inputFilePath);

    const vector<pair<string, vector<component>>> &getRegDefComponents();

    vector<string> getRegExp();

private:

    vector<string> regExpNames;
    vector<string> keywordsNames;
    vector<string> punctuationsNames;

    vector<pair<string, vector<component>>> regDefComponents;
    unordered_set<string> regDefNames;

    vector<string> readInputFile(const string &inputFilePath);

    void parseLine(string s);

    void addAllSingleChars();

    void addSingleChar(char c);

    vector<component> getComponents(const string &str);

    componentType getOperationType(char c);

    static bool isConcatableType(componentType type);

    void addKeywords(const string &s);

    void addPunctuations(const string &s);

    void addRegDef(const string &name, const vector<component> &components);

    void addRegDef(const string &name, const string &expression);

};

#endif //LEXICAL_ANALYZER_GRAMMARPARSER_H
