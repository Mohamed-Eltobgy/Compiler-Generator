#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

class ReadGrammar {
public:
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar;
    std::string startSymbol;
    std::pair<std::string,std::string> splitLine(const std::string& line) 
    {
        std::string left,right;
        size_t pos=line.find("::=");
        if (pos!=std::string::npos) 
        { 
            left=line.substr(0,pos);
            right=line.substr(pos+3);
        } 
        else 
        {
            left=line;
            right="";
        }
        return std::make_pair(left,right);
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::string> splitWithDelimiter(const std::string& input,char delimiter) 
    {
        std::vector<std::string> tokens;
        std::istringstream stream(input);
        std::string token;
        while (std::getline(stream, token, delimiter)) 
        {
            if (!token.empty())
            {tokens.push_back(token);}
        }
        return tokens;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    void ParseGrammar(const std::string& filename) 
    {
        std::ifstream file(filename);
        std::string line;
        std::string LFS;
        std::string RHS;
        std::vector<std::string> alternatives;

        if (!file.is_open()) {
            std::cerr << "Cannot open the file" << std::endl;
            return;
        }
        std::string buffer; // Accumulate lines that don't contain `::=` to handle more than 1-line productions
        int numberOfProductions=0;
        bool isFirstLine = true;
        while (std::getline(file,line)) 
        {
            if (!(line.find("::=")==std::string::npos))
                numberOfProductions++;

            if (numberOfProductions<2) 
            {
                if (!buffer.empty())
                    buffer+=" ";
                buffer+=line;
                continue;
            } 
            else 
            {
                LFS="";
                RHS="";
                auto production=splitLine(buffer);
                LFS=production.first;
                if (isFirstLine) {
                    startSymbol = LFS;
                    isFirstLine = false;
                }
                RHS=production.second;
                alternatives=splitWithDelimiter(RHS, '|');

                std::vector<std::vector<std::string>> word_lists;
                for (const auto& alt : alternatives) {
                    word_lists.push_back(splitWithDelimiter(alt,' ')); 
                }
                grammar[LFS]=word_lists;

                buffer=line;
                numberOfProductions=1;
            }
        }
        LFS="";
        RHS="";
        auto production=splitLine(buffer);
        LFS=production.first;
        RHS=production.second;
        alternatives=splitWithDelimiter(RHS, '|'); 

        std::vector<std::vector<std::string>> word_lists;
        
        for (const auto& alt : alternatives) 
        {
            word_lists.push_back(splitWithDelimiter(alt,' '));
        }
        grammar[LFS]=word_lists;
    }
//////////////////////////////////////////////////////////
 void printGrammar() {
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
////////////////////////////////////////////////////
};
// int main() {
//     ReadGrammar rg;
//     rg.ParseGrammar("rules.txt");
//     rg.printGrammar();
//     return 0;
// }
