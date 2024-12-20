#include "include/ReadInput.cpp"
#include "include/ReToNFA.cpp"


int main() {
    const std::string input_filename = "input.txt";
    const std::string lexical_rules_filename = "LexicalRules.txt";
    const std::string output_filename = "output/output.txt";
    const std::string dfa_transitions_filename = "output/dfa_transitions.txt";
    
    ReToNFA RE_NFA_DFA;
    ReadInput parsed_input;

    // Part 1: Lexical Analyzer Generator
    parsed_input.readLexicalRules(lexical_rules_filename);
    ReToNFA::NFA combinedNFA = RE_NFA_DFA.buildCombinedNFA(parsed_input);
    std::string input = RE_NFA_DFA.read_from_input_file(input_filename);
    RE_NFA_DFA.tokenNamePriority = parsed_input.GetPriorities();
    ReToNFA::DFA dfa = RE_NFA_DFA.NFAToDFA(combinedNFA);
    ReToNFA::DFA minimized_dfa = RE_NFA_DFA.minimizeDFA(dfa);
    RE_NFA_DFA.writeDFATableToFile(minimized_dfa, dfa_transitions_filename);
    std::vector<std::pair<std::string, std::string>> tokens = RE_NFA_DFA.lexicalAnalyzer(minimized_dfa, input);
    RE_NFA_DFA.write_output_to_file(output_filename, tokens);
    
    std::cout << "Symbol Table:\n";
    for (const auto& entry : RE_NFA_DFA.symbolTable) {
        std::cout << "Symbol: " << entry.second.lexeme << "\tID: " << entry.first << "\n";
    }

    // Part 2: Parser Generator

    return 0;
}