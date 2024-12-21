#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "ReadGrammar.cpp"
#include <iostream>

class FirstNFollow{
    public:
        std::unordered_map<std::string,std::unordered_set<std::string>> firstSets;
        std::unordered_map<std::string,std::vector<std::vector<std::string>>> grammar;
      
        void setGrammar(std::unordered_map<std::string, std::vector<std::vector<std::string>>> ReadGrammer)
        {grammar=ReadGrammer;}

        std::unordered_set<std::string> First(const std::string& expression) 
        {
            std::vector<std::string> tokens;
            std::istringstream iss(expression);
            std::string token;
            //all tokens if the expression contains more than 1 token
            while (iss>>token) 
            {
                tokens.push_back(token);
            }
    
            if (tokens.size()==1) 
            {
            
                const std::string& singleToken=tokens[0];
                if (firstSets.find(singleToken)!= firstSets.end()) 
                {return firstSets[singleToken];}

                std::unordered_set<std::string> fs;

                if (grammar.find(singleToken)==grammar.end()) //terminal
                {
                    fs.insert(singleToken);
                } 
                else // non terminal
                { 
                    for (const auto& production:grammar[singleToken]) //or
                    {
                        bool containsEpsilon=true;
                        for (const auto& prodSymbol:production) //concatenation
                        {
                            auto symbolFirst=First(prodSymbol);
                            fs.insert(symbolFirst.begin(),symbolFirst.end());
                            if (symbolFirst.find("ε")==symbolFirst.end()) 
                            {
                                containsEpsilon=false;
                                break;
                            }
                        }
                        if (containsEpsilon) fs.insert("ε");
                    }
                }
                firstSets[singleToken]=fs;
                return fs;
            }
            //if expression contains more than one token
            std::unordered_set<std::string> result;
            bool containsEpsilon=true;

            for (const auto& token:tokens) 
            {
                if (!containsEpsilon) break;
                auto tokenFirst=First(token);
                result.insert(tokenFirst.begin(),tokenFirst.end());

                if (tokenFirst.find("ε")==tokenFirst.end()) 
                {
                    containsEpsilon=false;
                } 
                else 
                {
                    result.erase("ε");
                }
            }
            if (containsEpsilon) 
                result.insert("ε");

            return result;
        }
        ////////////////////////////////////////////////////////////////////////////
       
};

int main() 
{
    ReadGrammar ri;
    ri.ParseGrammar("rules.txt");
    FirstNFollow f;
    f.setGrammar(ri.grammar);
    std::unordered_set<std::string> bterm=f.First("bterm' bterm");
    std::cout << "bterm first set: ";
    for (const auto& symbol : bterm) 
        std::cout<<symbol<<" ";
    std::cout<<std::endl;
    return 0;
} 