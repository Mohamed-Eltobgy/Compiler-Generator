#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "ReadGrammar.cpp"
#include <iostream>

class FirstNFollow{
    public:
        std::unordered_map<std::string,std::vector<std::vector<std::string>>> grammar;
        std::unordered_map<std::string,std::unordered_set<std::string>> firstSets;
        std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
        
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
        //////////////////////////////////////////////////////////////////////////////////////////////
        std::unordered_set<std::string> Follow(const std::string& NonTerminal, const std::string& startSymbol) 
        {
            if (followSets.find(NonTerminal)!=followSets.end())
                return followSets[NonTerminal];

            std::unordered_set<std::string> followSet;
            if (NonTerminal==startSymbol) 
                followSet.insert("$");
        
            for (const auto& rule:grammar) 
            {
                const std::string& LHS=rule.first;
                const std::vector<std::vector<std::string>>& RHS=rule.second;

                for (const auto& production:RHS) 
                {
                    for (size_t i=0;i<production.size();i++) 
                    {
                        if (production[i]==NonTerminal) 
                        {
                            if (i+1 < production.size()) 
                            {
                                // FIRST of the succeeding
                                auto succeedingFirst=First(production[i+1]);
                                followSet.insert(succeedingFirst.begin(),succeedingFirst.end());
                                followSet.erase("ε");

                                if (succeedingFirst.find("ε")!=succeedingFirst.end()) 
                                {
                                    auto followOfLHS=Follow(LHS,startSymbol);
                                    followSet.insert(followOfLHS.begin(),followOfLHS.end());
                                }
                            } 
                            else 
                            {
                                if (LHS!=NonTerminal) //nonterminal and itself
                                { 
                                    auto followOfLHS=Follow(LHS,startSymbol);
                                    followSet.insert(followOfLHS.begin(),followOfLHS.end());
                                }
                            }
                        }
                    }
                }
            }
            followSets[NonTerminal]=followSet;
            return followSet;
        }

};

// int main() 
// {
//     ReadGrammar ri;
//     ri.ParseGrammar("rules.txt");
//     FirstNFollow f;
//     f.setGrammar(ri.grammar);
//     std::unordered_set<std::string> first=f.First("bterm' bterm");
//     std::cout << "first set: ";
//     for (const auto& symbol : first) 
//         std::cout<<symbol<<" ";
//     std::cout<<std::endl;

//     std::unordered_set<std::string> follow=f.Follow("bexpr","bexpr");
//     std::cout << "follow set: ";
//     for (const auto& symbol : follow) 
//         std::cout<<symbol<<" ";
//     std::cout<<std::endl;
//     return 0;
// } 