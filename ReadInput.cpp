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
                std::string key = "";
                for (int i = 0; i < words.length(); i++) {
                    if (words[i] != ' ') {
                        key += words[i];
                    } else if (key.length() != 0){
                        keywords.insert(key);
                        key = "";
                    }
                }
                if (key.length() > 0)
                    keywords.insert(key);
            } else if (line[0] == '[' && line.back() == ']') {
                // Parse punctuation symbols
                std::string words = line.substr(1, line.size() - 2);
                std::string punc = "";
                for (int i = 0; i < words.length(); i++) {
                    if (words[i] != ' ') {
                        punc += words[i];
                    } else if (punc.length() != 0){
                        punctuation.insert(punc);
                        punc = "";
                    }
                }
                if (punc.length() > 0)
                    punctuation.insert(punc);
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

    void editRegulars() {
        std::unordered_map<std::string, std::string> substitute;

        for (auto& [key, value] : definitions) {
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

        for (auto& [key, value] : definitions) {
            for (int i = 0; i < value.length(); i++) {
                if (i > 0 && value[i] == '|' && value[i - 1] != '\\') {
                    std::string before = value.substr(0, i+1);
                    std::string after = value.substr(i + 1);
                    value = before + "(" + after;
                    int count = 0;
                    int j;
                    for (j = i + 2; j < value.length(); j++) {
                        if (value[j] == '(') {
                            count++;
                        } else if (value[j] == ')') {
                            count--;
                            if (count < 0) {
                                std::string before = value.substr(0, j+1);
                                std::string after = value.substr(j + 1);
                                value = before + ")" + after;
                                break;
                            }
                        }
                    }
                    if (j == value.length()) {
                        value += ')';
                    }
                }
            }
            substitute[key] = value;
        }

        for (auto& [key, value] : regexRules) {
            for (int i = 0; i < value.length(); i++) {
                if (i > 0 && value[i] == '|' && value[i - 1] != '\\') {
                    std::string before = value.substr(0, i+1);
                    std::string after = value.substr(i + 1);
                    value = before + "(" + after;
                    int count = 0;
                    int j;
                    for (j = i + 2; j < value.length(); j++) {
                        if (value[j] == '(') {
                            count++;
                        } else if (value[j] == ')') {
                            count--;
                            if (count < 0) {
                                std::string before = value.substr(0, j+1);
                                std::string after = value.substr(j + 1);
                                value = before + ")" + after;
                                break;
                            }
                        }
                    }
                    if (j == value.length()) {
                        value += ')';
                    }
                }
            }
        }
        for (int j = 0; j < regexRules.size(); j++) {
            handleRanges(regexRules[j].second);
            std::vector<std::string> temp = splitTokens(regexRules[j].second);
            std::string tempStr = regexRules[j].second;
            for (int i = 0; i < temp.size(); i++) {
                if (substitute.find(temp[i]) != substitute.end()) {
                    int start = tempStr.find(temp[i]);
                    std::string before = tempStr.substr(0, start);
                    std::string after = tempStr.substr(start + temp[i].length());
                    std::string subs = substitute[temp[i]];
                    tempStr = before + subs + after;
                }
            }
            regexRules[j].second = tempStr;
        }

    }

    std::vector<std::string> GetPriorities() {
        std::vector<std::string> result;
        result.insert(result.end(), keywords.begin(), keywords.end());
        result.insert(result.end(), punctuation.begin(), punctuation.end());
        for (const auto& [name, _] : regexRules) 
        {
            result.push_back(name);
        }
        std::cout << "\n TOKEN NAMES PRIORITIES:\n";
        for (const auto& element : result) {
            std::cout << element << " ";
        }
        std::cout << "\n----------------------------------------" << std::endl;
        return result;
    }

    ReadInput() {
        parseInputFile("LexicalRules.txt");
        editRegulars();
        print();
    }
};