#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class ReadGrammar {
public:
    std::unordered_map<std::string, std::vector<std::string>> grammar;

    std::pair<std::string, std::string> split_line(const std::string& line) 
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
    std::vector<std::string> split_alternatives(const std::string& input) {
        std::vector<std::string> alternatives;
        std::istringstream stream(input);
        std::string segment;
        while (std::getline(stream,segment,'|'))
        {
            alternatives.push_back(segment);
        }
        return alternatives;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void ParseGrammar(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::string LFS;
        std::string RHS;
        std::vector<std::string> alternatives;
        
        if (!file.is_open()) 
        {
            std::cerr << "Cannot open the file" << std::endl;
            return;
        }
        std::string buffer; //accumulate lines that don't contain `::=` to handle more than 1 line productions
        int numberOfProductions=0;
        while (std::getline(file,line)) 
        {
            if (!(line.find("::=") == std::string::npos))
              numberOfProductions=numberOfProductions+1;
              
            if (numberOfProductions<2){
                if (!buffer.empty()) 
                   buffer += " ";
                buffer += line;
                continue;
            }
            else{
                LFS="";
                RHS="";
                auto production=split_line(buffer);
                LFS=production.first;
                RHS=production.second;
                alternatives=split_alternatives(RHS);
                grammar[LFS]=alternatives;
                buffer=line;
                numberOfProductions=1;
            }    
        }
        //remaining
        LFS="";
        RHS="";
        auto production=split_line(buffer);
        LFS=production.first;
        RHS=production.second;
        alternatives=split_alternatives(RHS);
        grammar[LFS]=alternatives;
    }
};
// int main() {
//     ReadGrammar rg;
//     rg.ParseGrammar("rules.txt");
//     for (const auto& rule : rg.grammar) {
//         std::cout <<rule.first << "  => {";
//         for (size_t i=0;i<rule.second.size();++i) {
//             std::cout << "\"" << rule.second[i] << "\"";
//             if (i!=rule.second.size()-1) {
//                 std::cout << ",";
//             }
//         }
//         std::cout << "}" << std::endl;
//     }
//     return 0;
// }
