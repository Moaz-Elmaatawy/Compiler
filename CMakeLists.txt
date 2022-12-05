cmake_minimum_required(VERSION 3.17.3)
project(LexicalAnalyzerGenerator)

set(CMAKE_CXX_STANDARD 14)

add_executable(LexicalAnalyzerGenerator main.cpp Nfa.h Nfa.cpp InputParser.h InputParser.cpp DFA.cpp DFA.h Token.cpp Token.h LanguageParser.cpp LanguageParser.h MinimizedDfa.cpp MinimizedDfa.h FileWriter.cpp FileWriter.h)
