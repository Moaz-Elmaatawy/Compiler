%{

%}

DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*


%%
[;|,|(|)|{|}] {printf("%s punctiuation\n", yytext);}
int|if|else|while|boolean|float {printf("%s Keyword\n", yytext);}
{DIGIT}    {printf( "%s digit\n", yytext);}
{DIGIT}+    {printf( "%s number\n", yytext);}
{DIGIT}+"."{DIGIT}*        {printf( "%s float number\n", yytext);}
{ID}        {printf( "%s An identifier\n", yytext );}
"<="|"<"|">="|">"|"!="|"=="   {printf( "%s A relop\n", yytext );}
"="   {printf( "%s Assign\n", yytext );}
"+"|"-"   {printf( "%s addop\n", yytext );}
"*"|"/"   {printf( "%s mulop\n", yytext );}
"{"[^{}\n]*"}"     /* eat up one-line comments */
[ \t\n]+          /* eat up whitespace */
.           {printf( "%s Unrecognized character\n", yytext );}
%%

int yywrap(){}
int main(){

// Explanation:
// yywrap() - wraps the above rule section
/* yyin - takes the file pointer
		which contains the input*/
/* yylex() - this is the main flex function
		which runs the Rule Section*/
// yytext is the text in the buffer

// Uncomment the lines below
// to take input from file
/*
 FILE *fp;
 char filename[50];
 printf("Enter the filename: \n");
 scanf("%s",filename);
 fp = fopen(filename,"r");
 yyin = fp;
*/
yylex();

return 0;
}
