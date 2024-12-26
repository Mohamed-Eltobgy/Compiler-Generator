#include "FirstNFollow.cpp"

class parser {
public:
    std::map<std::pair<std::string, std::string>, std::string> productionMap;
    std::stack<std::string> stack;
    std::vector<std::string> derivation;
    std::string startSymbol;

    void createPredictionParsingTable(std::unordered_map<std::string, std::unordered_set<std::string>> follow) {
        std::cout << "follow in parser\n";
        for (const auto& pair : follow) {
            std::cout << pair.first << " follows: ";
            for (const auto& follower : pair.second) {
                std::cout << follower << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\n\n";
        for (auto it = follow.begin(); it != follow.end(); it++) {
            std::string nonTerminal = it->first;
            std::string tmp = "sync";
            if (productionMap.find(make_pair(nonTerminal, "ε")) != productionMap.end()) {
                tmp = productionMap[make_pair(nonTerminal, "ε")];
            }
            for (auto f : it->second) {
                productionMap[make_pair(nonTerminal, f)] = tmp;
            }
        }
    }
    
    void parse(std::vector<std::pair<std::string, std::string>>& tokens) {
        stack.push("$");
        stack.push(startSymbol);
        derivation.push_back(startSymbol);
        for (int i = 0; i < tokens.size(); i++) {
            std::string top = stack.top();
            std::cout << "Top: " << top << " Token: " << tokens[i].first << "\n";
            if (top == "$" && tokens[i].first == "$") { // Accept
                std::cout << "Accepted\n";
                break;
            } else if (top == "ε" ) {
                stack.pop();
                i--;
                std::cout << "ε" << std::endl;
            } else if (top == "sync") {
                stack.pop();
                i--;
                std::cout << "Input Missing" << std::endl;
            } else if (top == tokens[i].first) {  // Match
                stack.pop();
                std::cout << " -> Match " << top << std::endl;
            } else if (productionMap.find(make_pair(top, tokens[i].first)) != productionMap.end()) { // replace with production
                std::string production = productionMap[make_pair(top, tokens[i].first)];
                stack.pop();
                
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
            } else { // error panic mode recovery
                std::cout << "Error excess input -> " << "Top: " << top << " Token: " << tokens[i].first << "\n";
            }

            std::stack<std::string> tempStack = stack;
            std::string currDerivation = "";
            while (!tempStack.empty()) {
                currDerivation += tempStack.top() + " ";
                tempStack.pop();
            }
            derivation.push_back(currDerivation);
        }
    }

    void printDerivation(std::string outputFile) {
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "Error opening file for writing!" << std::endl;
            return;
        }

        for (const auto& curr : derivation) {
            outFile << curr << std::endl;
        }

        outFile.close();
        std::cout << "Vector contents written to output.txt" << std::endl;
    }

    parser(std::string path_to_rules) {
        FirstNFollow fnf(path_to_rules);
        productionMap = fnf.productionMap;
        startSymbol = fnf.startSymbol ;
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