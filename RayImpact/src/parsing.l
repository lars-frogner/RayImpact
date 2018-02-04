%{
#include "parsing.h"
#include "parsing.tab.h"
#include "stdlib.h"
#include "string.h"

#define store_token_string() \
    for (int i = 0; i < yyleng-2; i++) \
        yylval.s[i] = yytext[i+1]; \
    yylval.s[yyleng-2] = '\0'

#define store_token_int() \
    yylval.i = atoi(yytext)

#define store_token_double() \
    yylval.f = atof(yytext)

#define store_token_bool() \
    yylval.i == (strcmp(yytext, "true") == 0)? 1 : 0

#define store_token_name() \
    for (int i = 0; i < yyleng; i++) \
        yylval.s[i] = yytext[i]; \
    yylval.s[yyleng] = '\0'

%}

%option yylineno

letter    [a-zA-Z]
digit    [0-9]
alph    [a-zA-Z0-9_]
linesp    [ \t]
newline \r?\n
whitesp [ \t\r?\n]
int        [-+]?[1-9]{digit}*
float    [-+]?{digit}+\.{digit}*
bool    (true)|(false)
str        \"(\\.|[^\"])*\"

%%
{whitesp}+        /* Swallow whitespace */;
#.*{newline}    /* Swallow comments */;

bool            { printf("Found %s\n", yytext); return BOOL_TYPE; }
int                { printf("Found %s\n", yytext); return INT_TYPE; }
float            { printf("Found %s\n", yytext); return FLOAT_TYPE; }
string            { printf("Found %s\n", yytext); return STRING_TYPE; }

point2            { printf("Found %s\n", yytext); return POINT2_TYPE; }
vector2            { printf("Found %s\n", yytext); return VECTOR2_TYPE; }
point3            { printf("Found %s\n", yytext); return POINT3_TYPE; }
vector3            { printf("Found %s\n", yytext); return VECTOR3_TYPE; }
normal3            { printf("Found %s\n", yytext); return NORMAL3_TYPE; }

rgb                { printf("Found %s\n", yytext); return RGB_TYPE; }
xyz                { printf("Found %s\n", yytext); return XYZ_TYPE; }
spd                { printf("Found %s\n", yytext); return SPD_TYPE; }
spdf            { printf("Found %s\n", yytext); return SPDF_TYPE; }

bools            { printf("Found %s\n", yytext); return BOOL_ARR_TYPE; }
ints            { printf("Found %s\n", yytext); return INT_ARR_TYPE; }
floats            { printf("Found %s\n", yytext); return FLOAT_ARR_TYPE; }
strings            { printf("Found %s\n", yytext); return STRING_ARR_TYPE; }

points2            { printf("Found %s\n", yytext); return POINT2_ARR_TYPE; }
vectors2        { printf("Found %s\n", yytext); return VECTOR2_ARR_TYPE; }
points3            { printf("Found %s\n", yytext); return POINT3_ARR_TYPE; }
vectors3        { printf("Found %s\n", yytext); return VECTOR3_ARR_TYPE; }
normals3        { printf("Found %s\n", yytext); return NORMAL3_ARR_TYPE; }

rgbs            { printf("Found %s\n", yytext); return RGB_ARR_TYPE; }
xyzs            { printf("Found %s\n", yytext); return XYZ_ARR_TYPE; }
spds            { printf("Found %s\n", yytext); return SPD_ARR_TYPE; }
spdsf            { printf("Found %s\n", yytext); return SPDF_ARR_TYPE; }

{str}            { printf("Found string: %s\n", yytext); store_token_string(); return STRING_VAL; }
{bool}            { printf("Found bool: %s\n", yytext); store_token_bool(); return BOOL_VAL; }
{int}            { printf("Found int: %s\n", yytext); store_token_int(); return INT_VAL; }
{float}            { printf("Found float: %s\n", yytext); store_token_double(); return FLOAT_VAL; }

{alph}+            { printf("Found alphanumeric: %s\n", yytext); store_token_name(); return ALPHANUMERIC_VAL; }

,                { printf("Found %s\n", yytext); return DELIM; }

\[                { printf("Found %s\n", yytext); return ARRAY_BEGIN; }
\]                { printf("Found %s\n", yytext); return ARRAY_END; }

\(                { printf("Found %s\n", yytext); return VEC_BEGIN; }
\)                { printf("Found %s\n", yytext); return VEC_END; }

\{                { printf("Found %s\n", yytext); return PARAM_BEGIN; }
\}                { printf("Found %s\n", yytext); return PARAM_END; }

;                { printf("Found %s\n", yytext); return LINE_END; }

.                { printf("Found other: %s\n", yytext); return INVALID; }
%%

int yywrap()
{
    return 1;
}