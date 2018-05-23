%{
#include "parsing.h"

int yylex();
void yyerror(char* s);
%}

%union {
    char s[YYLVAL_STR_LEN];
    int i;
    double f;
}

%token BOOL_TYPE
%token INT_TYPE
%token FLOAT_TYPE
%token STRING_TYPE

%token POINT2_TYPE
%token VECTOR2_TYPE
%token POINT3_TYPE
%token VECTOR3_TYPE
%token NORMAL3_TYPE

%token RGB_TYPE
%token XYZ_TYPE
%token SPD_TYPE
%token SPDF_TYPE

%token BOOL_ARR_TYPE
%token INT_ARR_TYPE
%token FLOAT_ARR_TYPE
%token STRING_ARR_TYPE

%token POINT2_ARR_TYPE
%token VECTOR2_ARR_TYPE
%token POINT3_ARR_TYPE
%token VECTOR3_ARR_TYPE
%token NORMAL3_ARR_TYPE

%token RGB_ARR_TYPE
%token XYZ_ARR_TYPE
%token SPD_ARR_TYPE
%token SPDF_ARR_TYPE

%token<s> ALPHANUMERIC_VAL
%token<i> BOOL_VAL
%token<i> INT_VAL
%token<f> FLOAT_VAL
%token<s> STRING_VAL

%token DELIM

%token ARRAY_BEGIN
%token ARRAY_END

%token VEC_BEGIN
%token VEC_END

%token PARAM_BEGIN
%token PARAM_END

%token LINE_END

%token INVALID

%%
input:
    input statement
  | statement
  ;

statement:
    ALPHANUMERIC_VAL arguments LINE_END
        { parse_print("Detected statement with arguments: %s\n", $1); callAPIFunction($1); }
  | ALPHANUMERIC_VAL LINE_END
        { parse_print("Detected statement: %s\n", $1); callAPIFunction($1); }
  ;

arguments:
    arguments argument
  | argument
  ;

argument:
    bool
        { addBoolParameter(nextPositionalArgumentID()); }
  | int
        { addIntParameter(nextPositionalArgumentID()); }
  | float
        { addFloatParameter(nextPositionalArgumentID()); }
  | num
        { addNumParameter(nextPositionalArgumentID()); }
  | string
        { addStringParameter(nextPositionalArgumentID()); }
  | vector2
        {  addPairParameter(nextPositionalArgumentID()); }
  | vector3
        {  addTripleParameter(nextPositionalArgumentID()); }
  | bool_array
        {  addBoolParameter(nextPositionalArgumentID()); }
  | int_array
        {  addIntParameter(nextPositionalArgumentID()); }
  | float_array
        {  addFloatParameter(nextPositionalArgumentID()); }
  | num_array
        {  addNumParameter(nextPositionalArgumentID()); }
  | string_array
        {  addStringParameter(nextPositionalArgumentID()); }
  | param
  ;

bool:
    BOOL_VAL
        { addBoolElement($1); }
  ;

int:
    INT_TYPE INT_VAL
        { parse_print("Detected int\n"); addIntElement($2); }
  ;

float:
    FLOAT_TYPE FLOAT_VAL
        { parse_print("Detected int\n"); addFloatElement($2); }
  ;

num:
	INT_VAL
        { parse_print("Detected int num\n"); addNumElement($1); }
  |	FLOAT_VAL
        { parse_print("Detected float num\n"); addNumElement($1); }
  ;

string:
    STRING_VAL
        { addStringElement($1); }
  ;

vector2:
    VEC_BEGIN num DELIM num VEC_END
        { parse_print("Detected vector2\n"); }
  ;

vector3:
    VEC_BEGIN num DELIM num DELIM num VEC_END
        { parse_print("Detected vector3\n"); }
  ;

bool_array:
    ARRAY_BEGIN bool_array_elements ARRAY_END
        { parse_print("Detected bool array\n"); }
  ;

int_array:
    ARRAY_BEGIN int_array_elements ARRAY_END
        { parse_print("Detected int array\n"); }
  ;

float_array:
    ARRAY_BEGIN float_array_elements ARRAY_END
        { parse_print("Detected float array\n"); }
  ;

num_array:
    ARRAY_BEGIN num_array_elements ARRAY_END
        { parse_print("Detected num array\n"); }
  ;

string_array:
    ARRAY_BEGIN string_array_elements ARRAY_END
        { parse_print("Detected string array\n"); }
  ;

vector2_array:
    ARRAY_BEGIN vector2_array_elements ARRAY_END
        { parse_print("Detected vector2 array\n"); }
  ;

vector3_array:
    ARRAY_BEGIN vector3_array_elements ARRAY_END
        { parse_print("Detected vector3 array\n"); }
  ;

bool_array_elements:
    bool_array_elements DELIM bool
  | bool
  ;

int_array_elements:
    int_array_elements DELIM int
  | int
  ;

float_array_elements:
    float_array_elements DELIM float
  | float
  ;
  
num_array_elements:
    num_array_elements DELIM num
  | num
  ;

string_array_elements:
    string_array_elements DELIM string
  | string
  ;

vector2_array_elements:
    vector2_array_elements DELIM vector2
  | vector2
  ;

vector3_array_elements:
    vector3_array_elements DELIM vector3
  | vector3
  ;

param:
    bool_param
  | int_param
  | float_param
  | num_param
  | string_param
  | point2_param
  | vector2_param
  | pair_param
  | point3_param
  | vector3_param
  | normal3_param
  | rgb_param
  | xyz_param
  | spd_param
  | spdf_param
  | triple_param
  | bool_array_param
  | int_array_param
  | float_array_param
  | num_array_param
  | string_array_param
  | point2_array_param
  | vector2_array_param
  | pair_array_param
  | point3_array_param
  | vector3_array_param
  | normal3_array_param
  | rgb_array_param
  | xyz_array_param
  | triple_array_param
  | spd_array_param
  | spdf_array_param
  ;

bool_param:
    PARAM_BEGIN BOOL_TYPE STRING_VAL bool PARAM_END
        { parse_print("Detected bool param\n"); addBoolParameter($3); }
  | PARAM_BEGIN STRING_VAL bool PARAM_END
        { parse_print("Detected bool param\n"); addBoolParameter($2); }
  ;

int_param:
    PARAM_BEGIN INT_TYPE STRING_VAL int PARAM_END
        { parse_print("Detected int param\n"); addIntParameter($3); }
  ;

float_param:
    PARAM_BEGIN FLOAT_TYPE STRING_VAL float PARAM_END
        { parse_print("Detected float param\n"); addFloatParameter($3); }
  | PARAM_BEGIN STRING_VAL float PARAM_END
        { parse_print("Detected float param\n"); addFloatParameter($2); }
  ;

num_param:
    PARAM_BEGIN STRING_VAL num PARAM_END
        { parse_print("Detected num param\n"); addNumParameter($2); }
  ;

string_param:
    PARAM_BEGIN STRING_TYPE STRING_VAL string PARAM_END
        { parse_print("Detected string param\n"); addStringParameter($3); }
  | PARAM_BEGIN STRING_VAL string PARAM_END
        { parse_print("Detected string param\n"); addStringParameter($2); }
  ;

point2_param:
    PARAM_BEGIN POINT2_TYPE STRING_VAL vector2 PARAM_END
        { parse_print("Detected point2 param\n"); addPairParameter($3); }
  ;

vector2_param:
    PARAM_BEGIN VECTOR2_TYPE STRING_VAL vector2 PARAM_END
        { parse_print("Detected vector2 param\n"); addPairParameter($3); }
  ;

pair_param:
    PARAM_BEGIN STRING_VAL vector2 PARAM_END
        { parse_print("Detected pair param\n"); addPairParameter($2); }
  ;

point3_param:
    PARAM_BEGIN POINT3_TYPE STRING_VAL vector3 PARAM_END
        { parse_print("Detected point3 param\n"); addTripleParameter($3); }
  ;

vector3_param:
    PARAM_BEGIN VECTOR3_TYPE STRING_VAL vector3 PARAM_END
        { parse_print("Detected vector3 param\n"); addTripleParameter($3); }
  ;

normal3_param:
    PARAM_BEGIN NORMAL3_TYPE STRING_VAL vector3 PARAM_END
        { parse_print("Detected normal3 param\n"); addTripleParameter($3); }
  ;

rgb_param:
    PARAM_BEGIN RGB_TYPE STRING_VAL vector3 PARAM_END
        { parse_print("Detected rgb param\n"); addRGBSpectrumParameter($3); }
  ;

xyz_param:
    PARAM_BEGIN XYZ_TYPE STRING_VAL vector3 PARAM_END
        { parse_print("Detected xyz param\n"); addTristimulusSpectrumParameter($3); }
  ;

spd_param:
    PARAM_BEGIN SPD_TYPE STRING_VAL vector2_array PARAM_END
        { parse_print("Detected spd param\n"); addSampledSpectrumParameter($3); }
  ;

spdf_param:
    PARAM_BEGIN SPDF_TYPE STRING_VAL string PARAM_END
        { parse_print("Detected spdf param\n"); }
  ;

triple_param:
    PARAM_BEGIN STRING_VAL vector3 PARAM_END
        { parse_print("Detected triple param\n"); addTripleParameter($2); }
  ;

bool_array_param:
    PARAM_BEGIN BOOL_ARR_TYPE STRING_VAL bool_array PARAM_END
        { parse_print("Detected bool arr param\n"); addBoolParameter($3); }
  | PARAM_BEGIN STRING_VAL bool_array PARAM_END
        { parse_print("Detected bool arr param\n"); addBoolParameter($2); }
  ;

int_array_param:
    PARAM_BEGIN INT_ARR_TYPE STRING_VAL int_array PARAM_END
        { parse_print("Detected int arr param\n"); addIntParameter($3); }
  | PARAM_BEGIN STRING_VAL int_array PARAM_END
        { parse_print("Detected int arr param\n"); addIntParameter($2); }
  | PARAM_BEGIN INT_ARR_TYPE STRING_VAL num_array PARAM_END
        { parse_print("Detected int arr param\n"); addIntNumParameter($3); }
  ;

float_array_param:
    PARAM_BEGIN FLOAT_ARR_TYPE STRING_VAL float_array PARAM_END
        { parse_print("Detected float arr param\n"); addFloatParameter($3); }
  | PARAM_BEGIN STRING_VAL float_array PARAM_END
        { parse_print("Detected float arr param\n"); addFloatParameter($2); }
  | PARAM_BEGIN FLOAT_ARR_TYPE STRING_VAL num_array PARAM_END
        { parse_print("Detected float arr param\n"); addFloatParameter($3); }
  ;

num_array_param:
    PARAM_BEGIN STRING_VAL num_array PARAM_END
        { parse_print("Detected num arr param\n"); addNumParameter($2); }
  ;

string_array_param:
    PARAM_BEGIN STRING_ARR_TYPE STRING_VAL string_array PARAM_END
        { parse_print("Detected string arr param\n"); addStringParameter($3); }
  | PARAM_BEGIN STRING_VAL string_array PARAM_END
        { parse_print("Detected string arr param\n"); addStringParameter($2); }
  ;

point2_array_param:
    PARAM_BEGIN POINT2_ARR_TYPE STRING_VAL vector2_array PARAM_END
        { parse_print("Detected point2 arr param\n"); addPairParameter($3); }
  ;

vector2_array_param:
    PARAM_BEGIN VECTOR2_ARR_TYPE STRING_VAL vector2_array PARAM_END
        { parse_print("Detected vector2 arr param\n"); addPairParameter($3); }
  ;

pair_array_param:
    PARAM_BEGIN STRING_VAL vector2_array PARAM_END
        { parse_print("Detected pair arr param\n"); addPairParameter($2); }
  ;

point3_array_param:
    PARAM_BEGIN POINT3_ARR_TYPE STRING_VAL vector3_array PARAM_END
        { parse_print("Detected point3 arr param\n"); addTripleParameter($3); }
  ;

vector3_array_param:
    PARAM_BEGIN VECTOR3_ARR_TYPE STRING_VAL vector3_array PARAM_END
        { parse_print("Detected vector3 arr param\n"); addTripleParameter($3); }
  ;

normal3_array_param:
    PARAM_BEGIN NORMAL3_ARR_TYPE STRING_VAL vector3_array PARAM_END
        { parse_print("Detected normal3 arr param\n"); addTripleParameter($3); }
  ;

rgb_array_param:
    PARAM_BEGIN RGB_ARR_TYPE STRING_VAL vector3_array PARAM_END
        { parse_print("Detected rgb arr param\n"); addRGBSpectrumParameter($3); }
  ;

xyz_array_param:
    PARAM_BEGIN XYZ_ARR_TYPE STRING_VAL vector3_array PARAM_END
        { parse_print("Detected xyz arr param\n"); addTristimulusSpectrumParameter($3); }
  ;

triple_array_param:
    PARAM_BEGIN STRING_VAL vector3_array PARAM_END
        { parse_print("Detected triple arr param\n"); addTripleParameter($2); }
  ;

spd_array_param:
    PARAM_BEGIN SPD_ARR_TYPE STRING_VAL vector2_array int_array PARAM_END
        { parse_print("Detected spd arr param\n"); addSampledSpectrumParameter($3); }
  ;

spdf_array_param:
    PARAM_BEGIN SPDF_ARR_TYPE STRING_VAL string_array PARAM_END
        { parse_print("Detected spdf arr param\n"); }
  ;

%%

void yyerror(char* s)
{
    fprintf(stderr , "%s: token %d on line %d\n", s, yychar, yylineno);
}
