#include "FirstNFollow.cpp"

class parser {
public:
    std::map<std::pair<std::string, std::string>, std::string> productionMap;
    std::stack<std::string> stack;
    std::vector<std::string> derivation;
    std::string startSymbol;
    bool isLL1 = true;

    void createPredictionParsingTable(std::unordered_map<std::string, std::unordered_set<std::string>> follow) {
        for (auto it = follow.begin(); it != follow.end(); it++) {
            std::string nonTerminal = it->first;
            std::string temp = "sync";
            if (productionMap.find(make_pair(nonTerminal, "ε")) != productionMap.end()) {
                temp = productionMap[make_pair(nonTerminal, "ε")];
            }
            for (auto f : it->second) {
                if (temp == "sync" && productionMap.find(make_pair(nonTerminal, f)) != productionMap.end()) {
                    continue;
                } else if (temp == "ε" && productionMap.find(make_pair(nonTerminal, f)) != productionMap.end()) {
                    isLL1 = false;
                    break;
                }
                productionMap[make_pair(nonTerminal, f)] = temp;
            }
            if (!isLL1) {
                break;
            }
        }
    }
    
    void parse(std::vector<std::pair<std::string, std::string>>& tokens) {
        stack.push("$");
        stack.push(startSymbol);
        
        std::string currentForm = startSymbol; 
        derivation.push_back(currentForm);

        std::set<std::string> nonTerminals;
        nonTerminals.insert("$");
        for (auto x : productionMap) {
            nonTerminals.insert(x.first.first);
        }

        for (int i = 0; i < tokens.size(); i++) {
            std::string top = stack.top();
            std::cout << "Top: " << top << " Token: " << tokens[i].first << "\n";
            
            if (nonTerminals.find(top) != nonTerminals.end()) {
                if (top == "$" && tokens[i].first == "$") {
                    stack.pop();
                    std::cout << "Accepted: Stack is empty\n";
                }else if (productionMap.find(make_pair(top, tokens[i].first)) != productionMap.end()) {
                    std::string production = productionMap[make_pair(top, tokens[i].first)];
                    stack.pop();
                    
                    // Update the current sentential form by replacing the leftmost occurrence
                    // of the non-terminal (top) with its production
                    size_t pos = currentForm.find(top);
                    std::string temp = (production == "ε") ? "" : production;
                    currentForm += " ";
                    if (pos != std::string::npos) {
                        if (temp.length() == 0)
                            currentForm = currentForm.substr(0, pos) + currentForm.substr(pos + top.length() + 1);
                        else
                            currentForm = currentForm.substr(0, pos) + temp + currentForm.substr(pos + top.length());
                        derivation.push_back(currentForm);
                    }
                    currentForm.pop_back();
                    std::istringstream iss(production);
                    std::string token;
                    std::vector<std::string> tokensList;

                    while (iss >> token) {
                        tokensList.push_back(token);
                    }

                    for (auto it = tokensList.rbegin(); it != tokensList.rend(); ++it) {
                        stack.push(*it);
                    }
                    i--;

                    std::cout << " -> Replace production " << std::endl;
                } else {
                    std::cout << " -> Error excess input -> Ignore current token.\n";
                }
            } else {
                if (top == tokens[i].first) {
                    std::cout << " -> Match " << top << std::endl;
                } else if (top == "ε") {
                    i--;
                    std::cout << " -> Remove ε" << std::endl;
                } else if (top == "sync") {
                    i--;
                    std::cout << " -> Sync with input" << std::endl;
                } else {
                    i--;
                    std::cout << " -> Error missing input: " << top << "\n";
                }
                stack.pop();
            }
        }

        if (!stack.empty()) {
            std::cout << "Error: Stack is not empty.\n";
        }
    }

    void printDerivation(std::string outputFile) {
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error opening file for writing!" << std::endl;
            return;
        }

        for (size_t i = 0; i < derivation.size(); i++) {
            outFile << derivation[i] << std::endl;
        }

        outFile.close();
    }

    std::string centerText(const std::string& text, int width) {
        if (text.size() >= width) return text; // Return as-is if longer than width.
        int padding = width - text.size();
        int padLeft = padding / 2;
        int padRight = padding - padLeft;
        return std::string(padLeft, ' ') + text + std::string(padRight, ' ');
    }

    void writeParsingTableToFile(const std::string& filename) {
        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file" << std::endl;
            return;
        }

        std::set<std::string> terminals;
        std::set<std::string> nonTerminals;

        for (auto x : productionMap) {
            nonTerminals.insert(x.first.first);
            terminals.insert(x.first.second);
        }
        terminals.erase("ε");
        const int colWidth = 55;
        const int totalWidth = colWidth * (terminals.size() + 1);

        // Header row
        file << centerText("Symbol", colWidth);
        for (const auto& terminal : terminals) {
            file << centerText(terminal, colWidth);
        }
        file << "\n" << std::string(totalWidth, '-') << "\n";

        for (const auto& nonTerminal : nonTerminals) {
            file << centerText(nonTerminal, colWidth); // Non-terminal in the first column
            for (const auto& terminal : terminals) {
                std::string cellContent;
                if (productionMap.find({nonTerminal, terminal}) != productionMap.end()) {
                    cellContent = productionMap[{nonTerminal, terminal}];
                } else {
                    cellContent = "Error";
                }
                file << centerText(cellContent, colWidth); // Center content in cell
            }
            file << "\n";
        }

        file.close();
    }

    parser(std::string path_to_rules) {
        FirstNFollow fnf(path_to_rules);
        productionMap = fnf.productionMap;
        startSymbol = fnf.startSymbol;
        isLL1 = fnf.isLL1;
        createPredictionParsingTable(fnf.followSets);
    }
};

// int main() {
//     parser p("rules.txt");
//     for (const auto& pair : p.productionMap) {
//         const std::pair<std::string, std::string>& key = pair.first;
//         const std::string& value = pair.second;

//         std::cout << "Non terminal: " << key.first << "  terminal: " << key.second << "\nValues: " << value ;
//         std::cout << "\n\n";
//     }
//     return 0;
// }