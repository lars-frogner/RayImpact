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
        { printf("Detected statement with arguments: %s\n", $1); callAPIFunction($1); }
  | ALPHANUMERIC_VAL LINE_END
        { printf("Detected statement: %s\n", $1); callAPIFunction($1); }
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
  |    float
        { addFloatParameter(nextPositionalArgumentID()); }
  | string
        { addStringParameter(nextPositionalArgumentID()); }
  | vector2
        {  addVector2FParameter(nextPositionalArgumentID()); }
  | vector3
        {  addVector3FParameter(nextPositionalArgumentID()); }
  | bool_array
        {  addBoolParameter(nextPositionalArgumentID()); }
  | int_array
        {  addIntParameter(nextPositionalArgumentID()); }
  | float_array
        {  addFloatParameter(nextPositionalArgumentID()); }
  | string_array
        {  addStringParameter(nextPositionalArgumentID()); }
  | param
  ;

bool:
    BOOL_VAL
        { addBoolElement($1); }
  ;

int:
    INT_VAL
        { addIntElement($1); }
  ;

float:
    FLOAT_VAL
        { addFloatElement($1); }
  ;

string:
    STRING_VAL
        { addStringElement($1); }
  ;

vector2:
    VEC_BEGIN float DELIM float VEC_END
        { printf("Detected vector2\n"); }
  ;

vector3:
    VEC_BEGIN float DELIM float DELIM float VEC_END
        { printf("Detected vector3\n"); }
  ;

bool_array:
    ARRAY_BEGIN bool_array_elements ARRAY_END
        { printf("Detected bool array\n"); }
  ;

int_array:
    ARRAY_BEGIN int_array_elements ARRAY_END
        { printf("Detected int array\n"); }
  ;

float_array:
    ARRAY_BEGIN float_array_elements ARRAY_END
        { printf("Detected float array\n"); }
  ;

string_array:
    ARRAY_BEGIN string_array_elements ARRAY_END
        { printf("Detected string array\n"); }
  ;

vector2_array:
    ARRAY_BEGIN vector2_array_elements ARRAY_END
        { printf("Detected vector2 array\n"); }
  ;

vector3_array:
    ARRAY_BEGIN vector3_array_elements ARRAY_END
        { printf("Detected vector3 array\n"); }
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
  | string_param
  | point2_param
  | vector2_param
  | point3_param
  | vector3_param
  | normal3_param
  | rgb_param
  | xyz_param
  | spd_param
  | spdf_param
  | bool_array_param
  | int_array_param
  | float_array_param
  | string_array_param
  | point2_array_param
  | vector2_array_param
  | point3_array_param
  | vector3_array_param
  | normal3_array_param
  | rgb_array_param
  | xyz_array_param
  | spd_array_param
  | spdf_array_param
  ;

bool_param:
    PARAM_BEGIN STRING_VAL BOOL_TYPE bool PARAM_END
        { printf("Detected bool param\n"); addBoolParameter($2); }
  ;

int_param:
    PARAM_BEGIN STRING_VAL INT_TYPE int PARAM_END
        { printf("Detected int param\n"); addIntParameter($2); }
  ;

float_param:
    PARAM_BEGIN STRING_VAL FLOAT_TYPE float PARAM_END
        { printf("Detected float param\n"); addFloatParameter($2); }
  ;

string_param:
    PARAM_BEGIN STRING_VAL STRING_TYPE string PARAM_END
        { printf("Detected string param\n"); addStringParameter($2); }
  ;

point2_param:
    PARAM_BEGIN STRING_VAL POINT2_TYPE vector2 PARAM_END
        { printf("Detected point2 param\n"); addPoint2FParameter($2); }
  ;

vector2_param:
    PARAM_BEGIN STRING_VAL VECTOR2_TYPE vector2 PARAM_END
        { printf("Detected vector2 param\n"); addVector2FParameter($2); }
  ;

point3_param:
    PARAM_BEGIN STRING_VAL POINT3_TYPE vector3 PARAM_END
        { printf("Detected point3 param\n"); addPoint3FParameter($2); }
  ;

vector3_param:
    PARAM_BEGIN STRING_VAL VECTOR3_TYPE vector3 PARAM_END
        { printf("Detected vector3 param\n"); addVector3FParameter($2); }
  ;

normal3_param:
    PARAM_BEGIN STRING_VAL NORMAL3_TYPE vector3 PARAM_END
        { printf("Detected normal3 param\n"); addNormal3FParameter($2); }
  ;

rgb_param:
    PARAM_BEGIN STRING_VAL RGB_TYPE vector3 PARAM_END
        { printf("Detected rgb param\n"); addRGBSpectrumParameter($2); }
  ;

xyz_param:
    PARAM_BEGIN STRING_VAL XYZ_TYPE vector3 PARAM_END
        { printf("Detected xyz param\n"); addTristimulusSpectrumParameter($2); }
  ;

spd_param:
    PARAM_BEGIN STRING_VAL SPD_TYPE vector2_array PARAM_END
        { printf("Detected spd param\n"); addSampledSpectrumParameter($2); }
  ;

spdf_param:
    PARAM_BEGIN STRING_VAL SPDF_TYPE string PARAM_END
        { printf("Detected spdf param\n"); }
  ;

bool_array_param:
    PARAM_BEGIN STRING_VAL BOOL_ARR_TYPE bool_array PARAM_END
        { printf("Detected bool arr param\n"); addBoolParameter($2); }
  ;

int_array_param:
    PARAM_BEGIN STRING_VAL INT_ARR_TYPE int_array PARAM_END
        { printf("Detected int arr param\n"); addIntParameter($2); }
  ;

float_array_param:
    PARAM_BEGIN STRING_VAL FLOAT_ARR_TYPE float_array PARAM_END
        { printf("Detected float arr param\n"); addFloatParameter($2); }
  ;

string_array_param:
    PARAM_BEGIN STRING_VAL STRING_ARR_TYPE string_array PARAM_END
        { printf("Detected string arr param\n"); addStringParameter($2); }
  ;

point2_array_param:
    PARAM_BEGIN STRING_VAL POINT2_ARR_TYPE vector2_array PARAM_END
        { printf("Detected point2 arr param\n"); addPoint2FParameter($2); }
  ;

vector2_array_param:
    PARAM_BEGIN STRING_VAL VECTOR2_ARR_TYPE vector2_array PARAM_END
        { printf("Detected vector2 arr param\n"); addVector2FParameter($2); }
  ;

point3_array_param:
    PARAM_BEGIN STRING_VAL POINT3_ARR_TYPE vector3_array PARAM_END
        { printf("Detected point3 arr param\n"); addPoint3FParameter($2); }
  ;

vector3_array_param:
    PARAM_BEGIN STRING_VAL VECTOR3_ARR_TYPE vector3_array PARAM_END
        { printf("Detected vector3 arr param\n"); addVector3FParameter($2); }
  ;

normal3_array_param:
    PARAM_BEGIN STRING_VAL NORMAL3_ARR_TYPE vector3_array PARAM_END
        { printf("Detected normal3 arr param\n"); addNormal3FParameter($2); }
  ;

rgb_array_param:
    PARAM_BEGIN STRING_VAL RGB_ARR_TYPE vector3_array PARAM_END
        { printf("Detected rgb arr param\n"); addRGBSpectrumParameter($2); }
  ;

xyz_array_param:
    PARAM_BEGIN STRING_VAL XYZ_ARR_TYPE vector3_array PARAM_END
        { printf("Detected xyz arr param\n"); addTristimulusSpectrumParameter($2); }
  ;

spd_array_param:
    PARAM_BEGIN STRING_VAL SPD_ARR_TYPE vector2_array int_array PARAM_END
        { printf("Detected spd arr param\n"); addSampledSpectrumParameter($2); }
  ;

spdf_array_param:
    PARAM_BEGIN STRING_VAL SPDF_ARR_TYPE string_array PARAM_END
        { printf("Detected spdf arr param\n"); }
  ;

%%

void yyerror(char* s)
{
    fprintf(stderr , "%s: token %d on line %d\n", s, yychar, yylineno);
}
