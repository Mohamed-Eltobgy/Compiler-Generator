#include <bits/stdc++.h>
#include <iostream>
#include "ReadGrammar.cpp";

std::vector<std::vector<std::string>> createPredictionParsingTable(std::map<std::string, std::vector<std::string>> first,
    std::map<std::string, std::vector<std::string>> follow, std::set<std::string> terminalSet) {
    
    int nonTerminalCount = first.size();
    int terminalCount = terminalSet.size();
    std::vector<std::vector<std::string>> predictionParsingTable(nonTerminalCount, std::vector<std::string>(terminalCount, ""));
  
    
    std::map <std::string, int> nonTerminalIndex;
    std::map <std::string, int> terminalIndex;
    int nonTerminalIdx = 0;
    int terminalIdx = 0;
    for (auto it = terminalSet.begin(); it != terminalSet.end(); it++) {
        terminalIndex[*it] = terminalIdx;
        terminalIdx++;
    }
    

    for (auto it = first.begin(); it != first.end(); it++) {
        std::string nonTerminal = it->first;
        std::vector<std::string> firstSet = it->second;
        nonTerminalIndex[nonTerminal] = nonTerminalIdx;
        nonTerminalIdx++ ;
        bool flag = false;

        for (int i = 0; i < firstSet.size(); i++) {
            std::string terminal = firstSet[i];    
            if (terminal != "epsilon") {
                predictionParsingTable[nonTerminalIndex[nonTerminal]][terminalIndex[terminal]] = "waiting  for production from embaby.... ";
            } else {
                flag = true;
            }
        }
        
        std::string tmp = "sync";
        if (flag) {
            tmp = "epsilon";
        }
        std::vector<std::string> followSet = follow[nonTerminal];
        for (int j = 0; j < followSet.size(); j++) {            
            std::string terminal = followSet[j];
            predictionParsingTable[nonTerminalIndex[nonTerminal]][terminalIndex[terminal]] = tmp;
        }
    }
}