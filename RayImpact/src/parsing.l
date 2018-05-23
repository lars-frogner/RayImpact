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
int        (0)|([-+]?[1-9]{digit}*)
float    [-+]?{digit}+\.{digit}*
bool    (true)|(false)
str        \"(\\.|[^\"])*\"

%%
{whitesp}+        /* Swallow whitespace */;
#.*{newline}    /* Swallow comments */;

bool            { parse_print("Found %s\n", yytext); return BOOL_TYPE; }
int                { parse_print("Found %s\n", yytext); return INT_TYPE; }
float            { parse_print("Found %s\n", yytext); return FLOAT_TYPE; }
string            { parse_print("Found %s\n", yytext); return STRING_TYPE; }

point2            { parse_print("Found %s\n", yytext); return POINT2_TYPE; }
vector2            { parse_print("Found %s\n", yytext); return VECTOR2_TYPE; }
point3            { parse_print("Found %s\n", yytext); return POINT3_TYPE; }
vector3            { parse_print("Found %s\n", yytext); return VECTOR3_TYPE; }
normal3            { parse_print("Found %s\n", yytext); return NORMAL3_TYPE; }

rgb                { parse_print("Found %s\n", yytext); return RGB_TYPE; }
xyz                { parse_print("Found %s\n", yytext); return XYZ_TYPE; }
spd                { parse_print("Found %s\n", yytext); return SPD_TYPE; }
spdf            { parse_print("Found %s\n", yytext); return SPDF_TYPE; }

bools            { parse_print("Found %s\n", yytext); return BOOL_ARR_TYPE; }
ints            { parse_print("Found %s\n", yytext); return INT_ARR_TYPE; }
floats            { parse_print("Found %s\n", yytext); return FLOAT_ARR_TYPE; }
strings            { parse_print("Found %s\n", yytext); return STRING_ARR_TYPE; }

points2            { parse_print("Found %s\n", yytext); return POINT2_ARR_TYPE; }
vectors2        { parse_print("Found %s\n", yytext); return VECTOR2_ARR_TYPE; }
points3            { parse_print("Found %s\n", yytext); return POINT3_ARR_TYPE; }
vectors3        { parse_print("Found %s\n", yytext); return VECTOR3_ARR_TYPE; }
normals3        { parse_print("Found %s\n", yytext); return NORMAL3_ARR_TYPE; }

rgbs            { parse_print("Found %s\n", yytext); return RGB_ARR_TYPE; }
xyzs            { parse_print("Found %s\n", yytext); return XYZ_ARR_TYPE; }
spds            { parse_print("Found %s\n", yytext); return SPD_ARR_TYPE; }
spdsf            { parse_print("Found %s\n", yytext); return SPDF_ARR_TYPE; }

{str}            { parse_print("Found string: %s\n", yytext); store_token_string(); return STRING_VAL; }
{bool}            { parse_print("Found bool: %s\n", yytext); store_token_bool(); return BOOL_VAL; }
{int}            { parse_print("Found int: %s\n", yytext); store_token_int(); return INT_VAL; }
{float}            { parse_print("Found float: %s\n", yytext); store_token_double(); return FLOAT_VAL; }

{alph}+            { parse_print("Found alphanumeric: %s\n", yytext); store_token_name(); return ALPHANUMERIC_VAL; }

,                { parse_print("Found %s\n", yytext); return DELIM; }

\[                { parse_print("Found %s\n", yytext); return ARRAY_BEGIN; }
\]                { parse_print("Found %s\n", yytext); return ARRAY_END; }

\(                { parse_print("Found %s\n", yytext); return VEC_BEGIN; }
\)                { parse_print("Found %s\n", yytext); return VEC_END; }

\{                { parse_print("Found %s\n", yytext); return PARAM_BEGIN; }
\}                { parse_print("Found %s\n", yytext); return PARAM_END; }

;                { parse_print("Found %s\n", yytext); return LINE_END; }

.                { parse_print("Found other: %s\n", yytext); return INVALID; }
%%

int yywrap()
{
    return 1;
}