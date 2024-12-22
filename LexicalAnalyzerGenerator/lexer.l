%{
#include <stdio.h>
%}

%option noyywrap

/* Regular Definitions */
letter       [a-zA-Z]
digit        [0-9]
digits       {digit}+

/* Regular Expressions */
%%

"if"|"else"|"while"|"boolean"|"int"|"float"         { printf("Keyword: %s\n", yytext); }
[;,\(\)\{\}]                                        { printf("Punctuation: %s\n", yytext); }

{letter}({letter}|{digit})*                         { printf("ID: %s\n", yytext); }
{digit}+|({digit}+\.{digit}+)([E]{digits})?         { printf("num: %s\n", yytext); }

"=="|"!="|">="|"<="|">"|"<"                         { printf("Relop: %s\n", yytext); }

"="                                                 { printf("Assign: =\n"); }
"\+"|"-"                                            { printf("Addop: %s\n", yytext); }
"\*"|"/"                                            { printf("Mulop: %s\n", yytext); }

[ \t\n]+                                            { /* Ignore whitespaces */ }
.                                                   { printf("Undefined character: %s\n", yytext); }

%%

int main() {
    FILE *file = fopen("in.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    yyin = file;

    yylex();

    fclose(file);
    return 0;
}