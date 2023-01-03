cmake_minimum_required(VERSION 3.20.0)

project(LEXICAL_ANALYZER)

set(CMAKE_CXX_STANDARD 17)

add_executable(LexicalAnalyzer
        src/main.cpp
        src/NFA/NFA.cpp
        src/NFA/NFA.h
        src/NFA/NFABuilder.cpp
        src/NFA/NFABuilder.h
        src/Parser/GrammarParser.cpp
        src/Parser/GrammarParser.h
        src/NFA/NFA.h
        src/NFA/NFACombiner.cpp
        src/NFA/NFACombiner.h
        src/Utils/RegularExpression.h
        src/Utils/Component.h
        src/Utils/ParserUtils.cpp
        src/Utils/ParserUtils.h
        src/Parser/LexicalParser.cpp
        src/Parser/LexicalParser.h
        src/DFA/DFA.cpp
        src/DFA/DFA.h
        src/SyntaxParser/ParsingTable.cpp
        src/SyntaxParser/ParsingTable.h
        src/SyntaxParser/LL1GrammarBuilder.h
        src/SyntaxParser/LL1GrammarBuilder.cpp
        src/SyntaxParser/GrammarTypes.h
        src/SyntaxParser/SyntaxParser.cpp
        src/SyntaxParser/SyntaxParser.h
        src/SyntaxParser/SyntaxUtils.cpp
        src/SyntaxParser/SyntaxUtils.h)
