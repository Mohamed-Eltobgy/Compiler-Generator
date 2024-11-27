#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include<stack>
#include<sstream>

// State structure
struct State {
    int id;                               // State ID
    std::unordered_map<std::string, std::set<int>> transitions; // Transitions
};

// NFA structure
struct NFA {
    int startState;                       // Start state
    int finalState;                       // Final state
    std::unordered_map<int, State> states; // All states
};

NFA createCharNFA(std::string c) {
    NFA nfa;
    int start = 0, end = 1;
    nfa.startState = start;
    nfa.finalState = end;

    // Add states and transition
    nfa.states[start] = State{start};
    nfa.states[end] = State{end};
    nfa.states[start].transitions[c].insert(end);

    return nfa;
}

NFA concatenate(NFA a, NFA b) {
    // Connect final state of 'a' to start state of 'b' with an ε-transition
    a.states[a.finalState].transitions["\0"].insert(b.startState);

    // Merge states
    int mxId = a.states[a.finalState].id;
    for (const auto& [id, state] : b.states) {
        a.states[id + mxId] = state;
    }

    a.finalState = b.finalState;
    return a;
}

NFA unionNFA(NFA a, NFA b) {
    NFA nfa;
    int newStart = a.states.size() + b.states.size() + 3;
    int newFinal = newStart + 1;

    // New start state with ε-transitions to a and b
    nfa.startState = newStart;
    nfa.states[newStart] = State{newStart};
    nfa.states[newStart].transitions["\0"].insert(a.startState);
    nfa.states[newStart].transitions["\0"].insert(b.startState);

    // New final state with ε-transitions from a and b's final states
    nfa.finalState = newFinal;
    nfa.states[newFinal] = State{newFinal};
    a.states[a.finalState].transitions["\0"].insert(newFinal);
    b.states[b.finalState].transitions["\0"].insert(newFinal);

    // Merge states
    for (const auto& [id, state] : a.states) {
        nfa.states[id] = state;
    }
    int mxId = a.states[a.finalState].id;
    
    for (const auto& [id, state] : b.states) {
        nfa.states[id + mxId] = state;
    }

    return nfa;
}

NFA kleeneStar(NFA a, bool isPositve) {
    NFA nfa;
    int newStart = a.states.size();
    int newFinal = newStart + 1;

    // New start and final state
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

    return tokens;
}

NFA buildNFAFromRegex(const std::vector<std::string> tokens) {
    
    

 
   
}
   


int main() {
   
    std::vector<std::string> tokens = splitTokens("letter (letter|digit)* ");
  
    // for(auto& token : ans)
    //     std::cout <<token << "\n";

    // // Example: Regular expression for "a|b*"
    // std::string regexPostfix = "ab*|";

    // // Build NFA
    // NFA nfa = buildNFAFromRegex(regexPostfix);

    // // Print NFA states and transitions
    // for (const auto& [id, state] : nfa.states) {
    //     std::cout << "State " << id << ":\n";
    //     for (const auto& [symbol, targets] : state.transitions) {
    //         std::cout << "  " << (symbol == "\0" ? "ε" : std::string(1, symbol)) << " -> { ";
    //         for (int target : targets) {
    //             std::cout << target << " ";
    //         }
    //         std::cout << "}\n";
    //     }
    // }

    return 0;
}
