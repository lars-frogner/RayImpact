#pragma once
#include <stdio.h>

#define YYLVAL_STR_LEN 256

#ifdef IMP_VERBOSE_PARSING
    #define parse_print(format, ...) \
		printf((format), ##__VA_ARGS__)
#else
    #define parse_print(format, ...) ((void)0)
#endif

extern FILE* yyin;
extern FILE* yyout;

extern int yylineno, yychar;

// Parsing function declarations

int parseFile(const char* filename);

const char* nextPositionalArgumentID();

void addBoolElement(int value);
void addIntElement(int value);
void addFloatElement(double value);
void addNumElement(double value);
void addStringElement(const char* value);

void addBoolParameter(const char* name);
void addIntParameter(const char* name);
void addFloatParameter(const char* name);
void addNumParameter(const char* name);
void addIntNumParameter(const char* name);
void addStringParameter(const char* name);
void addPoint2FParameter(const char* name);
void addVector2FParameter(const char* name);
void addPairParameter(const char* name);
void addPoint3FParameter(const char* name);
void addVector3FParameter(const char* name);
void addNormal3FParameter(const char* name);
void addTripleParameter(const char* name);
void addRGBSpectrumParameter(const char* name);
void addTristimulusSpectrumParameter(const char* name);
void addSampledSpectrumParameter(const char* name);

void callAPIFunction(const char* name);
