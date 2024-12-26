#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "ReadInput.cpp"
#include <iostream>

class FirstNFollow{
    public:
        std::unordered_map<std::string, std::vector<std::vector<std::string>>> grammar;
        std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
        std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
        std::map<std::pair<std::string, std::string>, std::string> productionMap;
        std::string startSymbol;
        // to avoid infinite recursion
        std::unordered_map<std::string, bool> visitedFirst; 
        std::unordered_map<std::string, bool> visitedFollow;


        void setGrammar(std::unordered_map<std::string, std::vector<std::vector<std::string>>> ReadGrammer) {
            grammar=ReadGrammer;
        }

        std::unordered_set<std::string> First(const std::string& expression) 
        {
            if (visitedFirst[expression]) return {};
            visitedFirst[expression]=true;
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
                {
                    visitedFirst[expression]=false;
                    return firstSets[singleToken];
                }

                std::unordered_set<std::string> fs;

                if (grammar.find(singleToken)==grammar.end()) //terminal
                {
                    fs.insert(singleToken);
                } 
                else // non terminal
                { 
                    for (const auto& production : grammar[singleToken]) //or
                    {
                        bool containsEpsilon=true;
                        std::string pro = "";
                        std::unordered_set<std::string> tempFs;
                        for (const auto& prodSymbol:production) { // get complete production
                            pro += prodSymbol + " ";
                        }

                        for (const auto& prodSymbol:production) //concatenation
                        {
                            if (prodSymbol==singleToken) 
                            {
                                continue; //skip self-referential
                            }
                            auto symbolFirst=First(prodSymbol);
                            fs.insert(symbolFirst.begin(),symbolFirst.end());
                            tempFs.insert(symbolFirst.begin(), symbolFirst.end());
                            if (symbolFirst.find("ε")==symbolFirst.end()) {
                                containsEpsilon=false;
                                break;
                            }
                        }
                         for (const auto& fss : tempFs) {
                            productionMap[make_pair(singleToken, fss)] = pro;
                        }
                        if (containsEpsilon) fs.insert("ε");
                    }
                }
                firstSets[singleToken]=fs;
                visitedFirst[expression]=false;
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
            visitedFirst[expression]=false;
            return result;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        std::unordered_set<std::string> Follow(const std::string& NonTerminal, const std::string& startSymbol) 
        {
            if (visitedFollow[NonTerminal]) return {}; // to void infinite recursion 
            visitedFollow[NonTerminal]=true;

            if (followSets.find(NonTerminal)!=followSets.end()) 
            {
                visitedFollow[NonTerminal]=false;
                return followSets[NonTerminal];
            }

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
                                std::string succeeding;
                                for (size_t sama=i+1;sama<production.size();++sama) 
                                {
                                    succeeding+=production[sama];
                                    if (i < production.size()-1) {
                                        succeeding+=" ";
                                    }
                                }
                                auto succeedingFirst=First(succeeding);
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
            visitedFollow[NonTerminal]=false;
            return followSet;
        }
        /////////////////////////////////////////////////////////////////////////////////
        FirstNFollow (std::string path_to_rules) {
            ReadGrammar ri;
            ri.ParseGrammar(path_to_rules);
            setGrammar(ri.grammar);
            startSymbol = ri.startSymbol;
            for (auto it = grammar.begin(); it != grammar.end(); it++) {
                First(it->first);
            }
            for (auto it = grammar.begin(); it != grammar.end(); it++) {
                Follow(it->first, ri.startSymbol);
            }
            std::cout << "First in fnf:\n";
            for (const auto& pair : firstSets) {
                std::cout << pair.first << " first: ";
                for (const auto& follower : pair.second) {
                    std::cout << follower << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            std::cout << "Follow in fnf:\n";
            for (const auto& pair : followSets) {
                std::cout << pair.first << " follows: ";
                for (const auto& follower : pair.second) {
                    std::cout << follower << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
};

// int main() 
// {
//     // FirstNFollow();
  
//     ReadGrammar ri;
//     ri.ParseGrammar("rules.txt");
//     FirstNFollow f;
//     f.setGrammar(ri.grammar);
//         std::cout << "Grammar:\n";
//     for (const auto& rule : f.grammar) {
//         std::cout << rule.first << " -> ";
//         for (const auto& production : rule.second) {
//             for (const auto& symbol : production) {
//                 std::cout << symbol << " ";
//             }
//             std::cout << "| ";
//         }
//         std::cout << "\n";
//     }
    // for (auto it = f.grammar.begin(); it != f.grammar.end(); it++) {
    //     f.First(it->first);
    // }

    // for (const auto& pair : f.productionMap) {
    //     const std::pair<std::string, std::string>& key = pair.first;
    //     const std::string& value = pair.second;

    //     std::cout << "Non terminal: " << key.first << "  terminal: "<< key.second <<"\nValues: " << value;
    //     std::cout << "\n";
    //     std::cout << "\n";
    // }
    // std::cout << "\n";
    // std::cout << "\n";
    // std::unordered_set<std::string> first=f.First("bterm' bterm");
    // std::cout << "first set: ";
    // for (const auto& symbol : first) 
    //     std::cout<<symbol<<" ";
    // std::cout<<std::endl;

    // std::unordered_set<std::string> follow=f.Follow("bexpr","bexpr");
    // std::cout << "follow set: ";
    // for (const auto& symbol : follow) 
    //     std::cout<<symbol<<" ";
    // std::cout<<std::endl;
    // return 0;
// } 

int main() {
    std::string grammarFile = "rules.txt";
    FirstNFollow fnf(grammarFile);
    return 0;
}
