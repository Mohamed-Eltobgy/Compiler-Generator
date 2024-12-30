#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

class ReadGrammar {
public:
   // ASSUMING EACH PRODUCTION WILL START WITH A NEW LINE
   std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar;  
   std::string startSymbol;

   void ParseGrammar(const std::string& filename) 
    {
        std::ifstream file(filename);
        if (!file) 
        {
            std::cerr << "Cannot open the file" << std::endl;
            return;
        }

        std::string line,currentProduction;
        bool inProduction=false;
        bool isFirstProduction=true;

        while (std::getline(file,line)) 
        {
            line = trim(line);
            if (line.empty()) 
                continue;

            if (line[0]=='#') 
            {
                if (inProduction) 
                {
                    parseGrammarLine(currentProduction);
                    currentProduction.clear();
                }
                currentProduction=line.substr(1);
                inProduction=true;
            } 
            else 
            {
                currentProduction+=" " + line;
            }

            if (isFirstProduction && !currentProduction.empty()) 
            {
                size_t equalPos=currentProduction.find('=');
                if (equalPos!=std::string::npos) {
                    startSymbol=trim(currentProduction.substr(0,equalPos));
                    isFirstProduction=false;
                }
            }
        }
        if (!currentProduction.empty()) 
            parseGrammarLine(currentProduction);
        file.close();
    }

    //////////////////////////////////////////////////////////
    void printGrammar() {
        std::cout << "Start Symbol: " << startSymbol << std::endl;
        for (const auto& rule : grammar) {
            std::cout << rule.first << " => {" << std::endl;
            for (const auto& alt : rule.second) {
                std::cout << "    {";
                for (size_t i = 0; i < alt.size(); ++i) {
                    std::cout << "\"" << alt[i] << "\"";
                    if (i != alt.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "}" << std::endl;
            }
            std::cout << "}" << std::endl;
        }
    }
    
private:
    static std::string trim(const std::string& str) 
    {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, last - first + 1);
    }

    void parseGrammarLine(const std::string& line) {
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) 
        {
            std::cerr << "Invalid grammar line: " << line << std::endl;
            return;
        }
        // if = is terminal not production =
        if ((equalPos > 0 && line[equalPos - 1] == '\'') && 
            (equalPos + 1 < line.size() && line[equalPos + 1] == '\'')) 
        {
            std::cerr << "Found '=' in non-terminal symbol, invalid grammar line: " << line << std::endl;
            return;
        }
        std::string lhs = trim(line.substr(0, equalPos));
        std::string rhs = trim(line.substr(equalPos + 1));

        std::vector<std::vector<std::string>> alternatives;
        std::stringstream ss(rhs);
        std::string alternative;
        while (std::getline(ss, alternative, '|')) 
        {
            std::stringstream altStream(trim(alternative));
            std::vector<std::string> symbols;
            std::string symbol;

            while (altStream >> symbol) 
            {
                if (symbol.front() == '\'' && symbol.back() == '\'') 
                {
                    // Remove quotes
                    symbols.push_back(symbol.substr(1, symbol.length() - 2));
                } 
                else 
                {
                    symbols.push_back(symbol);
                }
            }
            alternatives.push_back(symbols);
        }
        grammar[lhs] = alternatives;
    }
};

// int main() {
//     ReadGrammar rg;
//     rg.ParseGrammar("cfg_input.txt");
//     rg.printGrammar();
//     return 0;
// }
