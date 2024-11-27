#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <regex>


struct LexicalRule {
    std::string type;  // "definition" or "regex"
    std::string name;  // Name of the token/definition
    std::string value; // Regular expression or pattern
};

void parseInputFile(const std::string& filename,
                    std::unordered_map<std::string, std::string>& definitions,
                    std::vector<std::pair<std::string, std::string>>& regexRules,
                    std::unordered_set<std::string>& keywords,
                    std::unordered_set<std::string>& punctuation) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

        // Skip empty lines or comments
        if (line.empty() || line[0] == '#') continue;

        if (line.find('=') != std::string::npos) {
            // Parse a definition (e.g., "letter = a-z | A-Z")
            auto pos = line.find('=');
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            definitions[name] = value;
        } else if (line.find(':') != std::string::npos) {
            // Parse a regular expression (e.g., "id: letter (letter|digit)*")
            auto pos = line.find(':');
            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            regexRules.emplace_back(name, value);
        } else if (line[0] == '{' && line.back() == '}') {
            // Parse keywords or reserved words (e.g., "{int float boolean}")
            std::string words = line.substr(1, line.size() - 2);
            std::regex wordRegex("\\s+");
            std::sregex_token_iterator iter(words.begin(), words.end(), wordRegex, -1);
            std::sregex_token_iterator end;
            for (; iter != end; ++iter) {
                keywords.insert(iter->str());
            }
        } else if (line[0] == '[' && line.back() == ']') {
            // Parse punctuation symbols (e.g., "[; , ( ) { }]")
            std::string symbols = line.substr(1, line.size() - 2);
            std::regex symbolRegex("\\s+");
            std::sregex_token_iterator iter(symbols.begin(), symbols.end(), symbolRegex, -1);
            std::sregex_token_iterator end;
            for (; iter != end; ++iter) {
                punctuation.insert(iter->str());
            }
        }
    }
    file.close();
}

int main() {
    std::unordered_map<std::string, std::string> definitions;
    std::vector<std::pair<std::string, std::string>> regexRules;
    std::unordered_set<std::string> keywords;
    std::unordered_set<std::string> punctuation;

    parseInputFile("input.txt", definitions, regexRules, keywords, punctuation);

    // Debug output
    std::cout << "Definitions:\n";
    for (const auto& [key, value] : definitions) {
        std::cout << key << " = " << value << std::endl;
    }

    std::cout << "\nRegex Rules:\n";
    for (const auto& [name, value] : regexRules) {
        std::cout << name << ": " << value << std::endl;
    }

    std::cout << "\nKeywords:\n";
    for (const auto& keyword : keywords) {
        std::cout << keyword << " ";
    }
    std::cout << "\n";

    std::cout << "\nPunctuation:\n";
    for (const auto& symbol : punctuation) {
        std::cout << symbol << " ";
    }
    std::cout << "\n";

    return 0;
}