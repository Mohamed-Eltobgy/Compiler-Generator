#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <stack>
#include "ReadInput.cpp"
#include <queue>

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
        NFAToDFA(combinedNFA);
        print();
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    struct DFAState {
        int id;
        std::unordered_map<std::string, std::set<int>> transitions;
        std::set<std::string> tokenNames;
    };

    struct DFA {
        int startState;
        int finalState;
        std::unordered_map<int,DFAState> states; // All states
        std::vector<int> acceptedFinalStates;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    DFA NFAToDFA(NFA nfa) 
    {

    DFA dfa;
    std::map<std::set<int>,int> stateMapping; //map NFA state sets to DFA states
    std::queue<std::set<int>> stateQueue; //Queue for the unprocessed NFA state sets
    int dfaStateID=0; //ids of the DFA states 
    
    //epsilon closure function of a set of NFA states
    auto epsilonClosure=[&](const std::set<int>& states) 
    {
        std::stack<int> stack;
        std::set<int> closure(states);

        for (int state : states)
            stack.push(state);

        while (!stack.empty()) {
            int current=stack.top();
            stack.pop();
            if (nfa.states[current].transitions.count("\0")) {
                for (int next : nfa.states[current].transitions["\0"]) {
                    if (closure.insert(next).second) { //first:iterator pointing to the inserted element second:true or false insertion was successful or not.
                        stack.push(next);
                    }
                }
            }
        }
        return closure;
    };

    //Function to compute the set of states reachable with a given symbol
    auto move=[&](const std::set<int>& states, const std::string& symbol) {
        std::set<int> result;
        for (int state : states) {
            if (nfa.states[state].transitions.count(symbol)) {
                for (int next : nfa.states[state].transitions[symbol]) {
                    result.insert(next);
                }
            }
        }
        return result;
    };

    //epsilon closure of the start state
    std::set<int> startClosure=epsilonClosure({nfa.startState});
    stateMapping[startClosure]=dfaStateID++;
    stateQueue.push(startClosure);

    dfa.startState=0;

    // Subset construction
    while (!stateQueue.empty()) 
    {
        std::set<int> currentSet=stateQueue.front();
        stateQueue.pop();

        int currentDFAState=stateMapping[currentSet];
        dfa.states[currentDFAState]={currentDFAState};

        //token names from the NFA states in the current set
        for (int state : currentSet) {
            if (!nfa.states[state].tokenName.empty()) {
                dfa.states[currentDFAState].tokenNames.insert(nfa.states[state].tokenName);
            }
        }

      
        // all possible transitions
        std::map<std::string, std::set<int>> symbolTransitions;
        for (int state : currentSet) 
        {
            for (const auto& [symbol, targets] : nfa.states[state].transitions) 
            {
                if (symbol=="\0") continue;
                symbolTransitions[symbol].insert(targets.begin(),targets.end());
            }
        }

        for (const auto& [symbol,targetStates] : symbolTransitions) 
        {
            std::set<int> newSet=epsilonClosure(targetStates);
            if (newSet.empty()) continue;
            if (!stateMapping.count(newSet)) 
            {
                stateMapping[newSet]=dfaStateID++;
                stateQueue.push(newSet);
            }
            dfa.states[currentDFAState].transitions[symbol].insert(stateMapping[newSet]);
        }
    }

    // final states
    for (const auto& [stateSet, dfaState] : stateMapping) 
    {
        for (int nfaFinal : nfa.acceptedFinalStates) 
        {
            if(stateSet.count(nfaFinal))
            {
                dfa.acceptedFinalStates.push_back(dfaState);
                break;
            }
        }
    }
    return dfa;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, int> symbolTable;
// Lexical Analyzer:Finds the longest prefix matching a regular expression
std::vector<std::pair<std::string, std::string>> lexicalAnalyzer(const DFA& dfa, const std::string& input) {
    size_t position=0;
    std::vector<std::pair<std::string, std::string>> tokens; //matched tokens and their values

    while (position<input.size()) 
    {
        int currentState=dfa.startState;
        size_t lastMatchPos=position;
        std::string lastMatchedToken;
        bool matchFound=false;

        //input character by character
        for (size_t i=position; i<input.size();++i) 
        {
            std::string currentChar(1, input[i]);

            // Check if there's a valid transition from the current state
            auto stateIt=dfa.states.find(currentState);
            if (stateIt==dfa.states.end()) break; // Invalid state

            const auto& transitions=stateIt->second.transitions;
            auto transitionIt=transitions.find(currentChar);
            if (transitionIt==transitions.end()) break; // No transition for this character

            // Move to the next state
            currentState=*transitionIt->second.begin();

            // Check if this state is an accepted final state
            if (std::find(dfa.acceptedFinalStates.begin(), dfa.acceptedFinalStates.end(), currentState)!=dfa.acceptedFinalStates.end()) {
                matchFound=true;
                lastMatchPos=i;
                // Get the highest-priority token name from the state assuming the first tokenname
                if (!dfa.states.at(currentState).tokenNames.empty()) {
                    lastMatchedToken = *dfa.states.at(currentState).tokenNames.begin();
                }

            }
        }

        if (matchFound) 
        {
            //get the matched substring and add it to the symbol table
            std::string matchedSubstring=input.substr(position, lastMatchPos - position + 1);
            position=lastMatchPos+1;

            // Insert the token and its value into the symbol table if not already present
            if (symbolTable.find(matchedSubstring) == symbolTable.end()) {
                symbolTable[matchedSubstring] = symbolTable.size() + 1;
            }

            tokens.emplace_back(lastMatchedToken,matchedSubstring);
        } 
        else 
        {
      
           // errorRecovery(input,position);
           // RECOVERY FUNCTION
         
        }
    }
    return tokens;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////


};

// int main() {
//     ReToNFA();
//     return 0;
// }