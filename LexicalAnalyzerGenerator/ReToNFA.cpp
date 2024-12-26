#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <stack>
#include <queue>
#include <iomanip> 

struct MapHash {
    std::size_t operator()(const std::map<std::string, int>& m) const {
        std::size_t seed = 0;
        for (const auto& [key, value] : m) {
            seed ^= std::hash<std::string>{}(key) ^ (std::hash<int>{}(value) << 1);
        }
        return seed;
    }
};


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
        int isEscape = -2;
        bool isUnion = false;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] != "(" && tokens[i] != ")" && tokens[i] != "|" && tokens[i] != "+" && tokens[i] != "*" &&
                tokens[i] != "L" && tokens[i] != "\\") {
                std::string  nextToken = (i + 1 < tokens.size()) ? tokens[i + 1] : "";
                i += combineNFAs(nfaS, tokens[i], nextToken, isUnion, opHist.size());
            } else if (isEscape == i - 1) {
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
                    if (nfaS.size() > opHist.size()) {
                        opHist.push(isUnion);
                    } else if (nfaS.size() == opHist.size()) {
                        opHist.push(isUnion);
                        nfaS.push(createInitialNFA("\0"));
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
                            NFA secTop = nfaS.top();
                            nfaS.pop();
                            NFA newNfa = unionNFA(secTop, topNfa);
                            nfaS.push(newNfa);
                        } else {
                            NFA secTop = nfaS.top();
                            nfaS.pop();
                            NFA newNfa = concatenateNFA(secTop, topNfa);
                            nfaS.push(newNfa);
                        }
                        opHist.pop();
                    }
                } else if (tokens[i] == "\\") {
                    isEscape = i;
                }
            }
            if (i == isEscape + 1)
                isEscape = -2;
        }
        return nfaS.top();
    }

    NFA buildCombinedNFA(ReadInput parsedInput) {
        stateID = 1;
        combinedNFA = NFA{0};
        combinedNFA.states[0] = State{0};

        for (int i = 0; i < parsedInput.regexRules.size(); i++) {
            std::vector<std::string> tokens = splitTokens(parsedInput.regexRules[i].second);
            
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

        for (const auto& key : parsedInput.keywords) {
            std::vector<std::string> charsOfKey;
            for (char c : key) {
                charsOfKey.push_back(std::string(1, c));
            }
            NFA nfa = buildNFAFromRegex(charsOfKey);
            nfa.states[nfa.finalState].tokenName = key;

            // add the new NFA to the combined NFA
            combinedNFA.states[0].transitions["\0"].insert(nfa.startState);
            combinedNFA.acceptedFinalStates.push_back(nfa.finalState);

            for (const auto& [id, state] : nfa.states) {
                combinedNFA.states[id] = state;
            }
        }

        for (const auto& punc : parsedInput.punctuation) {
            std::vector<std::string> charsOfKey;
            for (char c : punc) {
                charsOfKey.push_back(std::string(1, c));
            }
            NFA nfa = buildNFAFromRegex(charsOfKey);
           
            char lastChar = punc.back();
            nfa.states[nfa.finalState].tokenName = std::string(1, lastChar);

            // add the new NFA to the combined NFA
            combinedNFA.states[0].transitions["\0"].insert(nfa.startState);
            combinedNFA.acceptedFinalStates.push_back(nfa.finalState);

            for (const auto& [id, state] : nfa.states) {
                combinedNFA.states[id] = state;
            }
        }
        return combinedNFA;
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
/////////////////////////////////////////////////////////////////////////////////////////////////

DFA minimizeDFA(const DFA& dfa) {
    // Step 1: Partition into final and non-final states
    std::set<int> finalStates(dfa.acceptedFinalStates.begin(), dfa.acceptedFinalStates.end());
    std::set<int> nonFinalStates;

    for (const auto& [id, state] : dfa.states) {
        if (finalStates.find(id) == finalStates.end()) {
            nonFinalStates.insert(id);
        }
    }

    // Initial partition
    std::vector<std::set<int>> partitions = {};

    // For finalState in final states, split into separate partitions by token names
    std::set<int> processedStates;
    for (int finalState : finalStates) {
        // Skip already processed states
        if (processedStates.find(finalState) != processedStates.end()) {
            continue;
        }

        const auto& tokenNames = dfa.states.at(finalState).tokenNames;
        std::set<int> partition = {finalState};
        
        // Get highest priority token name 
        std::string highestPriorityToken = *tokenNames.begin();
        for (const std::string& tokenName : tokenNames) {
            if (std::find(tokenNamePriority.begin(), tokenNamePriority.end(), tokenName) < std::find(tokenNamePriority.begin(), tokenNamePriority.end(), highestPriorityToken)) {
                highestPriorityToken = tokenName;
            }
        }
        
        // Compare with other final states
        for (int state : finalStates) {
            
            const auto& tokenNames_2 = dfa.states.at(state).tokenNames;
            std::string highestPriorityToken_2 = *tokenNames_2.begin();
            for (const std::string& tokenName : tokenNames_2) {
                if (std::find(tokenNamePriority.begin(), tokenNamePriority.end(), tokenName) < std::find(tokenNamePriority.begin(), tokenNamePriority.end(), highestPriorityToken_2)) {
                    highestPriorityToken_2 = tokenName;
                }
            }

            if (state != finalState && 
                !processedStates.count(state) && 
                highestPriorityToken == highestPriorityToken_2) {
                partition.insert(state);
                processedStates.insert(state); // Mark as processed
            }
        }

        partitions.push_back(partition);
        processedStates.insert(finalState); // Mark this state as processed
    }


    // add nonfinal partition
    partitions.push_back(nonFinalStates);

    // Step 2: Refine partitions
    bool isRefined = true;
    while (isRefined) {
        isRefined = false;
        std::vector<std::set<int>> newPartitions;

        for (const auto& partition : partitions) {
            std::unordered_map<std::map<std::string, int>, std::set<int>, MapHash> transitionGroups;

            // Group states based on their transitions
            for (int state : partition) {
                std::map<std::string, int> transitionMap;

                for (const auto& [symbol, targets] : dfa.states.at(state).transitions) {
                    int targetPartition = -1;
                    if (!targets.empty()) {
                        int targetState = *targets.begin();
                        for (size_t i = 0; i < partitions.size(); ++i) {
                            if (partitions[i].count(targetState)) {
                                targetPartition = static_cast<int>(i);
                                break;
                            }
                        }
                    }
                    transitionMap[symbol] = targetPartition;
                }
                transitionGroups[transitionMap].insert(state);
            }

            // Create new partitions based on grouped states
            for (const auto& [_, group] : transitionGroups) {
                newPartitions.push_back(group);
                if (group.size() < partition.size()) {
                    isRefined = true;
                }
            }
        }

        partitions = std::move(newPartitions);
    }

    // Step 3: Build minimized DFA
    DFA minimizedDFA;
    std::unordered_map<int, int> stateMapping; // Maps old states to new states

    int newStateID = 0;
    for (const auto& partition : partitions) {
        for (int state : partition) {
            stateMapping[state] = newStateID;
        }
        newStateID++;
    }
    newStateID = 0;
    for (const auto& partition : partitions) {
        DFAState newState;
        newState.id = newStateID++;

        // Aggregate transitions and token names
        for (int state : partition) {
            const auto& oldState = dfa.states.at(state);
            // Todo: Add highest priority token name only
            newState.tokenNames.insert(oldState.tokenNames.begin(), oldState.tokenNames.end());

            for (const auto& [symbol, targets] : oldState.transitions) {
                if (!targets.empty()) {
                    int targetState = *targets.begin(); // Assuming DFA is deterministic
                    newState.transitions[symbol].insert(stateMapping[targetState]);
                }
            }
        }

        minimizedDFA.states[newState.id] = newState;
    }

    // Set start state
    minimizedDFA.startState = stateMapping[dfa.startState];
    
    // Set final states
    for (int finalState : dfa.acceptedFinalStates) {
        minimizedDFA.acceptedFinalStates.push_back(stateMapping[finalState]);
    }

    return minimizedDFA;
}
    
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct SymbolEntry {
    std::string lexeme; 
    int value;         
};

std::unordered_map<int, SymbolEntry> symbolTable;

void insertInSymbolTable(const std::string& matchedSubstring) {
    for (const auto& [id,entry] : symbolTable) {
        if (entry.lexeme == matchedSubstring) {
            return;
        }
    }
    int newId = symbolTable.size() + 1; 
    symbolTable[newId] = {matchedSubstring};
}

std::vector<std::string> tokenNamePriority;
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
        bool space=false;

        //input character by character
        for (size_t i=position; i<input.size();++i) 
        {
            std::string currentChar(1, input[i]);
            char ch=currentChar[0];
            if(isspace(ch)){
                space=true;
                break;
            }
        

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
                // Get the highest-priority token name if there are many token names
               if (!dfa.states.at(currentState).tokenNames.empty()) 
               {
                    const auto& tokenNames = dfa.states.at(currentState).tokenNames;
                    for (const std::string& priorityToken : tokenNamePriority) {
                        if (tokenNames.find(priorityToken) != tokenNames.end()) {
                            lastMatchedToken = priorityToken;
                            break; 
                        }
                    }
                    // if (!dfa.states.at(currentState).tokenNames.empty()) {
                    // lastMatchedToken = *dfa.states.at(currentState).tokenNames.begin();
                // }
                }
            }
        }

        if (matchFound) 
        {
            //get the matched substring and add it to the symbol table
            std::string matchedSubstring=input.substr(position, lastMatchPos - position + 1);
            if (space){
                position=lastMatchPos+2;
                space=false;
            }
            else{
            position=lastMatchPos+1;
            }

            if(lastMatchedToken=="id")
                insertInSymbolTable(matchedSubstring);
            tokens.emplace_back(lastMatchedToken,matchedSubstring);
        } 
        else 
        {
           // RECOVERY FUNCTION
            recoveryRoutine_char(input, position);
        }

        while (position < input.size() && isspace(input[position])) {
            position++;
        }

    }
    return tokens;
}

void recoveryRoutine_space(const std::string& input, size_t& position) {
    std::cout << "Error: Invalid token found: " << input.substr(position) << std::endl;
    while (position < input.size() && !isspace(input[position])) {
        position++;
    }
}

void recoveryRoutine_char(const std::string& input, size_t& position) {
    std::cout << "Error: Invalid token found: " << input[position] << std::endl;
    if (position < input.size()) {
        position++;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void write_output_to_file(const std::string filename,std::vector<std::pair<std::string, std::string>> tokens)
{
    std::ofstream file_output (filename,std::ios::trunc);
    if (!file_output)
    {
        std::cerr << "Error while opening the file !!!!!!!!";
    }
    for (const auto& token : tokens) {
        std::string processedToken = token.first;
        for (size_t i=0;i< processedToken.size();++i) {
            if (processedToken[i] == '\\' && i + 1 < processedToken.size() && (processedToken[i+1]=='(' || processedToken[i+1]==')'|| processedToken[i+1]=='[' || processedToken[i+1]==']') ) {
                // Skip the escape character and take the next character
                processedToken = processedToken.substr(0, i) + processedToken.substr(i + 1, 1) + processedToken.substr(i + 2);
            }
        }

        // Write the processed token (with escaped characters handled)
        file_output << processedToken << std::endl;
    }
    file_output.close();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string read_from_input_file(const std::string filename) {
    std::ifstream file_input(filename);
    if (!file_input.is_open()) {
        std::cerr << "Error: Could not open input file.\n";
        return "";
    }

    std::string result;
    char ch;
    while (file_input.get(ch)) 
    {
        // if (!isspace(ch)) {
            result += ch;
        // }
    }
    file_input.close();
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////
    void writeDFATableToFile(const DFA& dfa, const std::string& filename) {
        std::ofstream file(filename);

        if (!file.is_open())
        {std::cerr << "Error" << std::endl;return;}

        //all possible symbols
        std::set<std::string> allSymbols;
        for (const auto& state : dfa.states) 
        {
            for (const auto& transition : state.second.transitions) 
            {
                allSymbols.insert(transition.first);
            }
        }

    
        const int colWidth=15;
        
        //header
        file << std::setw(colWidth) << "State";
        for (const auto& symbol : allSymbols) 
        {
            file << std::setw(colWidth) << symbol;
        }
        file << "\n" << std::string(colWidth * (allSymbols.size() + 1), '-') << "\n";

        //transitions
        for (const auto& state : dfa.states) 
        {
            file << std::setw(colWidth) << state.first;
            for (const auto& symbol : allSymbols) 
            {
                if (state.second.transitions.find(symbol) != state.second.transitions.end()) 
                {
                    const auto& reachableStates = state.second.transitions.at(symbol);
                    std::string transitionStr = "";
                    for (int nextState : reachableStates)
                    {
                        transitionStr += std::to_string(nextState) + " ";
                    }
                    file << std::setw(colWidth) << transitionStr;
                } else 
                {
                    file << std::setw(colWidth) << "None";
                }
            }
            file << "\n";
        }
        file.close();
    }
};