#include <iostream>
#include "ReToNFA.cpp"

//testing from nfa to dfa function
int main() {
    ReToNFA reToNFA;
    ReToNFA::NFA nfa;
    nfa.startState=0;
    nfa.finalState=2;

     
    nfa.states[0] = {0, {{"a", {1,2}}},"0"};
    nfa.states[1] = {1, {{"b", {2}},{"a", {0}}},"1"};
    nfa.states[2] = {2,{},"2"}; 

    nfa.acceptedFinalStates = {2};

    ReToNFA::DFA dfa = reToNFA.NFAToDFA(nfa);

    std::cout << "DFA States and Transitions:\n";
    for (const auto& [id, state] : dfa.states) {
        std::cout << "State " << id << ":\n";
        for (const auto& [symbol, targets] : state.transitions) {
            std::cout << "  " << (symbol == "\0" ? "ε" : symbol) << " -> { ";
            for (int target : targets) {
                std::cout << target << " ";
            }
            std::cout << "}\n";
        }
        std::cout << "Token names: { ";
        for (const auto& token : state.tokenNames) {
            std::cout << token << " ";
        }
        std::cout << "}\n";
    }

    std::cout << "DFA Final States: { ";
    for (int finalState : dfa.acceptedFinalStates) {
        std::cout << finalState << " ";
    }
    std::cout << "}\n";

    return 0;
}