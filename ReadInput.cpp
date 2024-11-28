#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <regex>

class ReadInput {
public:
    std::unordered_map<std::string, std::string> definitions;
    std::vector<std::pair<std::string, std::string>> regexRules;
    std::unordered_set<std::string> keywords;
    std::unordered_set<std::string> punctuation;

    void parseInputFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            // remove leading and trailing whitespace
            line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

            // Skip empty lines
            if (line.empty()) continue;

            if (line[0] == '{' && line.back() == '}') {
                // Parse keywords
                std::string words = line.substr(1, line.size() - 2);
                std::regex wordRegex("\\s+");
                std::sregex_token_iterator iter(words.begin(), words.end(), wordRegex, -1);
                std::sregex_token_iterator end;
                for (; iter != end; ++iter) {
                    keywords.insert(iter->str());
                }
            } else if (line[0] == '[' && line.back() == ']') {
                // Parse punctuation symbols
                std::string symbols = line.substr(1, line.size() - 2);
                std::regex symbolRegex("\\s+");
                std::sregex_token_iterator iter(symbols.begin(), symbols.end(), symbolRegex, -1);
                std::sregex_token_iterator end;
                for (; iter != end; ++iter) {
                    punctuation.insert(iter->str());
                }
            } else {
                for (int i = 0; i < line.length(); i++) {
                    if (line[i] == ':') {
                        // Parse a regular expression
                        std::string name = line.substr(0, i);
                        std::string value = line.substr(i + 1);
                        regexRules.emplace_back(name, value);
                        break;
                    } else if (line[i] == '=') {
                        // Parse a definition
                        std::string name = line.substr(0, i);
                        std::string value = line.substr(i + 1);
                        definitions[name] = value;
                        break;
                    }
                }
            }
        }
        file.close();
    }

    void print() {
        std::cout << "Definitions:\n";
        for (const auto& [key, value] : definitions) {
            std::cout << key << " = " << value << std::endl;
        }

        std::cout << "\nRegex Rules:\n";
        for (const auto& [name, value] : regexRules) {
            std::cout << name << " : " << value << std::endl;
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
        std::cout << "----------------------------------------" << std::endl;
    }

    ReadInput() {
        parseInputFile("input.txt");
        print();
    }
};