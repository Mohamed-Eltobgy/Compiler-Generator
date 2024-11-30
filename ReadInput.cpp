#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <regex>

class ReadInput {
public:
    std::vector<std::pair<std::string, std::string>> definitions;
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
                        name = std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");
                        value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
                        regexRules.emplace_back(name, value);
                        break;
                    } else if (line[i] == '=') {
                        // Parse a definition
                        std::string name = line.substr(0, i);
                        std::string value = line.substr(i + 1);
                        name = std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");
                        value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
                        definitions.emplace_back(name, value);
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
        std::cout << "\n----------------------------------------" << std::endl;
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
    
    void handleRanges(std::string& str) {
        std::vector<std::string> temp = splitTokens(str);
        for (size_t i = 0; i < temp.size(); i++) {
            if (i > 0 && temp[i] == "-" && i + 1 < temp.size()) {
                if (i > 1 && temp[i - 1] == "\\" && temp[i - 2] != "\\") {
                    continue;
                }

                char start = temp[i - 1][0];
                char end = temp[i + 1][0];

                if ((std::isalpha(start) && std::isalpha(end) && start < end) ||
                    (std::isdigit(start) && std::isdigit(end) && start < end)) {
                    std::string replacement = "(";
                    for (char c = start; c <= end; c++) {
                        replacement += c;
                        if (c != end)
                            replacement += "|";
                    }
                    replacement += ")";

                    temp[i - 1] = replacement;
                    temp.erase(temp.begin() + i, temp.begin() + i + 2);
                    i--;
                }
            }
        }

        str = "";
        for (const std::string& token : temp) {
            str += token + " ";
        }
        if (!str.empty())
            str.pop_back(); // Remove trailing space
    }

    void editRegulars(std::vector<std::pair<std::string, std::string>>& defs, std::vector<std::pair<std::string, std::string>>& regexRules) {
        std::unordered_map<std::string, std::string> substitute;

        for (auto& [key, value] : defs) {
            handleRanges(value);
            std::vector<std::string> temp = splitTokens(value);
            for (int i = 0; i < temp.size(); i++) {
                if (substitute.find(temp[i]) != substitute.end()) {
                    int start = value.find(temp[i]);
                    std::string before = value.substr(0, start);
                    std::string after = value.substr(start + temp[i].length());
                    value = before + substitute[temp[i]] + after;
                }
            }
            substitute[key] = value;
        }

        for (auto& [key, value] : regexRules) {
            handleRanges(value);
            std::vector<std::string> temp = splitTokens(value);
            for (int i = 0; i < temp.size(); i++) {
                if (substitute.find(temp[i]) != substitute.end()) {
                    int start = value.find(temp[i]);
                    std::string before = value.substr(0, start);
                    std::string after = value.substr(start + temp[i].length());
                    value = before + substitute[temp[i]] + after;
                }
            }
        }
    }

    ReadInput() {
        parseInputFile("input.txt");
        editRegulars(definitions, regexRules);
        print();
    }
};