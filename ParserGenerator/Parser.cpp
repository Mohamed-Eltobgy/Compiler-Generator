#include "FirstNFollow.cpp"

void createPredictionParsingTable(std::map<std::pair<std::string, std::string>, std::string>& productionMap,
    std::unordered_map<std::string, std::unordered_set<std::string>> follow) {
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

int main() {
    FirstNFollow fnf;
    createPredictionParsingTable(fnf.productionMap, fnf.followSets);
    for (const auto& pair : fnf.productionMap) {
        const std::pair<std::string, std::string>& key = pair.first;
        const std::string& value = pair.second;

        std::cout << "Non terminal: " << key.first << "  terminal: "<< key.second <<"\nValues: " << value;
        std::cout << "\n";
        std::cout << "\n";
    }
    return 0;
}