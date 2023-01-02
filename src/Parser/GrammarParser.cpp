#include <iostream>
#include <fstream>
#include <regex>
#include <climits>
#include "GrammarParser.h"
#include "../Utils/ParserUtils.h"


GrammarParser::GrammarParser(const string &inputFilePath) {
    vector<string> inputFileLines = this->readInputFile(inputFilePath);
    this->addAllSingleChars();
    for (string &line: inputFileLines)
        if (!line.empty())
            parseLine(line);
}

const vector<pair<string, vector<component>>> &GrammarParser::getRegDefComponents() {
    return this->regDefComponents;
}

vector<string> GrammarParser::getRegExp() {
    vector<string> allNames;
    allNames.insert(allNames.end(), this->keywordsNames.begin(), this->keywordsNames.end());
    allNames.insert(allNames.end(), this->punctuationsNames.begin(), this->punctuationsNames.end());
    allNames.insert(allNames.end(), this->regExpNames.begin(), this->regExpNames.end());
    return allNames;
}

vector<string> GrammarParser::readInputFile(const string &inputFilePath) {
    fstream inputFile;
    inputFile.open(inputFilePath, ios::in);
    vector<string> inputFileLines;
    if (inputFile.is_open()) {
        string tp;
        while (getline(inputFile, tp)) {
            inputFileLines.push_back(tp);
        }
        inputFile.close();
    }
    return inputFileLines;
}

void GrammarParser::addAllSingleChars() {

    for (char i = 0; i < CHAR_MAX; i++)
        addSingleChar(i);
}

void GrammarParser::addSingleChar(char c) {
    string let{c};
    vector<component> letComponents{{REG_DEF, let}};
    addRegDef(let, letComponents);
}

void GrammarParser::parseLine(string s) {
    s = trim(s);
    if (s[0] == '[') {
        addPunctuations(s);
    } else if (s[0] == '{') {
        addKeywords(s);
    } else {
        int i = 0;
        while (!(s[i] == '=' || s[i] == ':')) i++;

        string name = s.substr(0, i);
        name = trim(name);
        if (s[i] == ':')
            this->regExpNames.emplace_back(name);

        string expression = s.substr(i + 1);
        expression = trim(expression);
        addRegDef(name, expression);
    }
}

void GrammarParser::addRegDef(const string &name, const string &expression) {
    addRegDef(name, getComponents(expression));
}

void GrammarParser::addRegDef(const string &name, const vector<component> &components) {
    this->regDefComponents.emplace_back(name, components);
    this->regDefNames.insert(name);
}

void GrammarParser::addPunctuations(const string &s) {

    for (int i = 1; i < s.size() - 1; i++) {
        if (s[i] == ' ') continue;
        char punctuation;
        if (s[i] == '\\') {
            punctuation = s[i + 1];
            i++;
        } else {
            punctuation = s[i];
        }
        addSingleChar(punctuation);
        this->punctuationsNames.emplace_back(string{punctuation});
    }

}

void GrammarParser::addKeywords(const string &s) {
    regex rgx("\\w+");
    for (sregex_iterator it(s.begin(), s.end(), rgx), it_end; it != it_end; ++it) {
        string keyword = (*it)[0];
        string keywordWithSpaces = addSpacesBetweenChars(keyword);

        addRegDef(keyword, keywordWithSpaces);
        this->keywordsNames.emplace_back(keyword);
    }
}

vector<component> GrammarParser::getComponents(const string &str) {
    vector<component> components;
    for (int i = 0; i < str.length(); i++) {
        componentType type = getOperationType(str[i]);
        if (type == CONCAT) continue;
        if (type == REG_DEF) {
            if (!components.empty() && isConcatableType(components.back().type))
                components.emplace_back(CONCAT);
            if (i + 1 < str.length() && str[i] == '\\' && str[i + 1] == 'L') {
                components.emplace_back(type, string{EPSILON});
                i++;
            } else {
                string name;
                int j = i;
                while (j < str.length() && getOperationType(str[j]) == REG_DEF) {
                    if (str[j] == '\\') {
                        name.push_back(str[j + 1]);
                        j += 2;
                    } else {
                        name.push_back(str[j]);
                        j += 1;
                    }
                }
                if (this->regDefNames.count(name)) {
                    components.emplace_back(type, name);
                } else {
                    vector<component> lettersComponents = getComponents(addSpacesBetweenChars(name));
                    components.insert(
                            components.end(),
                            make_move_iterator(lettersComponents.begin()),
                            make_move_iterator(lettersComponents.end())
                    );
                }
                i = j - 1;
            }
        } else {
            if (type == OPEN_BRACKETS && !components.empty() && isConcatableType(components.back().type))
                components.emplace_back(CONCAT);

            components.emplace_back(type);
        }
    }
    return components;
}

bool GrammarParser::isConcatableType(componentType type) {
    return type == REG_DEF || type == CLOSE_BRACKETS || type == KLEENE_CLOSURE || type == POS_CLOSURE;
}

componentType GrammarParser::getOperationType(char c) {
    switch (c) {
        case '(':
            return OPEN_BRACKETS;
        case ')':
            return CLOSE_BRACKETS;
        case '*':
            return KLEENE_CLOSURE;
        case '+':
            return POS_CLOSURE;
        case '|':
            return OR;
        case ' ':
            return CONCAT;
        case '-':
            return TO;
        default:
            return REG_DEF;
    }
}
