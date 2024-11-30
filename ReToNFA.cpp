#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <stack>
#include "ReadInput.cpp"

class ReToNFA {
public:
    struct State {
        int id;
        std::unordered_map<std::string, std::set<int>> transitions;
        std::string tokenName;
    };

    struct NFA {
        int startState;
        int finalState;
        std::unordered_map<int, State> states; // All states
        std::vector<int> acceptedFinalStates;
    };

    int stateID = 1;
    NFA combinedNFA;

    NFA createInitialNFA(std::string c) {
        NFA nfa;
        int start = stateID, end = stateID + 1;
        stateID += 2;
        nfa.startState = start;
        nfa.finalState = end;

        nfa.states[start] = State{start};
        nfa.states[end] = State{end};
        nfa.states[start].transitions[c].insert(end);

        return nfa;
    }

    NFA concatenateNFA(NFA a, NFA b) {
        // Merge states
        a.states[a.finalState] = b.states[b.startState];
        b.states.erase(b.startState);
        for (const auto& [id, state] : b.states) {
            a.states[id] = state;
        }

        a.finalState = b.finalState;
        return a;
    }

    NFA unionNFA(NFA a, NFA b) {
        NFA nfa;
        int newStart = stateID;
        int newFinal = stateID + 1;
        stateID += 2;
        // connect New start state with ε-transitions to old start states
        nfa.startState = newStart;
        nfa.states[newStart] = State{newStart};
        nfa.states[newStart].transitions["\0"].insert(a.startState);
        nfa.states[newStart].transitions["\0"].insert(b.startState);

        // connect new final state with ε-transitions to old final states
        nfa.finalState = newFinal;
        nfa.states[newFinal] = State{newFinal};
        a.states[a.finalState].transitions["\0"].insert(newFinal);
        b.states[b.finalState].transitions["\0"].insert(newFinal);

        // Merge states
        for (const auto& [id, state] : a.states) {
            nfa.states[id] = state;
        }
        
        for (const auto& [id, state] : b.states) {
            nfa.states[id] = state;
        }

        return nfa;
    }

    NFA kleeneStar(NFA a, bool isPositve) {
        NFA nfa;
        int newStart = stateID;
        int newFinal = stateID + 1;
        stateID += 2;
        // New start and final states
        nfa.startState = newStart;
        nfa.finalState = newFinal;
        nfa.states[newStart] = State{newStart};
        nfa.states[newFinal] = State{newFinal};

        // ε-transitions for looping and empty string
        nfa.states[newStart].transitions["\0"].insert(a.startState);
        
        if (!isPositve) {
            nfa.states[newStart].transitions["\0"].insert(newFinal);
        }

        a.states[a.finalState].transitions["\0"].insert(a.startState);
        a.states[a.finalState].transitions["\0"].insert(newFinal);

        // Merge states
        for (const auto& [id, state] : a.states) {
            nfa.states[id] = state;
        }

        return nfa;
    }


    std::vector<std::string> splitTokens(const std::string& input) {
        std::vector<std::string> tokens;
        std::string token;

        for(char c : input){
            if (isalpha(c)){
                token += c;
            }else{
                if( token.size() > 0 ){
                    tokens.push_back(token);
                    token = "";
                }
                if(c == ' ')
                    continue;
                else{
                    token = c;
                    tokens.push_back(token);
                }
                token = "";
            }
        }
        if (token.length() > 0)
            tokens.push_back(token);
        return tokens;
    }

    int combineNFAs(std::stack<NFA>& nfaS, std::string token, std::string nextToken, bool isUnion, int bracketNum) {
        NFA nfa = createInitialNFA(token);
        bool flag = false;
        if (nextToken == "+") {
            NFA newNfa = kleeneStar(nfa, true);
            flag = true;
            nfa = newNfa;
        } else if (nextToken == "*") {
            NFA newNfa = kleeneStar(nfa, false);
            flag = true;
            nfa = newNfa;
        }
        if (isUnion && nfaS.size() > bracketNum) {
            isUnion = false;
            if (!nfaS.empty()) {
                NFA newNfa = unionNFA(nfaS.top(), nfa);
                nfaS.pop();
                nfaS.push(newNfa);
            }
        } else if (!nfaS.empty() && nfaS.size() > bracketNum) {
            NFA newNfa = concatenateNFA(nfaS.top(), nfa);
            nfaS.pop();
            nfaS.push(newNfa);
        } else {
            nfaS.push(nfa);
        }
        if (flag)
            return 1;
        return 0;
    }

    NFA buildNFAFromRegex(const std::vector<std::string> tokens) {
        std::stack<NFA> nfaS;
        std::stack<int> opHist;
        bool isUnion = false;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] != "(" && tokens[i] != ")" && tokens[i] != "|" && tokens[i] != "+" && tokens[i] != "*" &&
                tokens[i] != "L" && tokens[i] != "\\") {
                std::string  nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : "";
                i += combineNFAs(nfaS, tokens[i], nextToken, isUnion, opHist.size());
            } else if (i > 0 && tokens[i-1] == "\\") {
                if (tokens[i] == "L") {
                    combineNFAs(nfaS, "\0", "", isUnion, opHist.size());
                } else {
                    std::string  nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : "";
                    i += combineNFAs(nfaS, tokens[i], nextToken, isUnion, opHist.size());
                }
            } else {
                if (tokens[i] == "|") {
                    isUnion = true;
                } else if (tokens[i] == "+" && !nfaS.empty()) {
                    NFA newNfa = kleeneStar(nfaS.top(), true);
                    nfaS.pop();
                    nfaS.push(newNfa);
                } else if (tokens[i] == "*" && !nfaS.empty()) {
                    NFA newNfa = kleeneStar(nfaS.top(), false);
                    nfaS.pop();
                    nfaS.push(newNfa);
                } else if (tokens[i] == "L") {
                    std::string  nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : "";
                    i += combineNFAs(nfaS, tokens[i], nextToken, isUnion, opHist.size());
                } else if (tokens[i] == "(") {
                    if (!nfaS.empty()) {
                        opHist.push(isUnion);
                    }
                    isUnion = false;
                } else if (tokens[i] == ")") {
                    if (i + 1 < tokens.size()) {
                        if (tokens[i + 1] == "+") {
                            NFA newNfa = kleeneStar(nfaS.top(), true);
                            nfaS.pop();
                            nfaS.push(newNfa);
                            i++;
                        } else if (tokens[i + 1] == "*") {
                            NFA newNfa = kleeneStar(nfaS.top(), false);
                            nfaS.pop();
                            nfaS.push(newNfa);
                            i++;
                        }
                    }
                    if (!opHist.empty()) {
                        NFA topNfa = nfaS.top();
                        nfaS.pop();
                        if (opHist.top()) {
                            NFA newNfa = unionNFA(nfaS.top(), topNfa);
                            nfaS.pop();
                            nfaS.push(newNfa);
                        } else {
                            NFA newNfa = concatenateNFA(nfaS.top(), topNfa);
                            nfaS.pop();
                            nfaS.push(newNfa);
                        }
                        opHist.pop();
                    }
                }
            }
        }
        return nfaS.top();
    }

    void buildCombinedNFA() {
        stateID = 1;
        ReadInput parsedInput;
        combinedNFA = NFA{0};
        combinedNFA.states[0] = State{0};
        for (int i = 0; i < parsedInput.regexRules.size(); i++) {
            std::vector<std::string> tokens = splitTokens(parsedInput.regexRules[i].second);
        
            // for(auto& token : tokens)
            //     std::cout <<token << "\n";
            
            // build NFA for a single expression
            NFA nfa = buildNFAFromRegex(tokens);
            nfa.states[nfa.finalState].tokenName = parsedInput.regexRules[i].first;
            // add the new NFA to the combined NFA
            combinedNFA.states[0].transitions["\0"].insert(nfa.startState);
            combinedNFA.acceptedFinalStates.push_back(nfa.finalState);

            for (const auto& [id, state] : nfa.states) {
                combinedNFA.states[id] = state;
            }
        }
    }

    void print() {
        // Print NFA states and transitions
        for (const auto& [id, state] : combinedNFA.states) {
            std::cout << "State " << id << ":\n";
            for (const auto& [symbol, targets] : state.transitions) {
                std::cout << "  " << (symbol == "\0" ? "ε" : symbol) << " -> { ";
                for (int target : targets) {
                    std::cout << target << " ";
                }
                std::cout << "}\n";
            }
        }
        std::cout << "----------------------------------------\n" << std::endl;
    }

    ReToNFA() {
        buildCombinedNFA();
        print();
    }
};

int main() {
    ReToNFA();
    return 0;
}