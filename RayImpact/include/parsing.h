#pragma once
#include <stdio.h>

#define YYLVAL_STR_LEN 256

extern FILE* yyin;
extern FILE* yyout;

extern int yylineno, yychar;

int parseFile(const char* filename);

const char* nextPositionalArgumentID();

void addBoolElement(int value);
void addIntElement(int value);
void addFloatElement(double value);
void addStringElement(const char* value);

void addBoolParameter(const char* name);
void addIntParameter(const char* name);
void addFloatParameter(const char* name);
void addStringParameter(const char* name);
void addPoint2FParameter(const char* name);
void addVector2FParameter(const char* name);
void addPoint3FParameter(const char* name);
void addVector3FParameter(const char* name);
void addNormal3FParameter(const char* name);
void addRGBSpectrumParameter(const char* name);
void addTristimulusSpectrumParameter(const char* name);
void addSampledSpectrumParameter(const char* name);

void callAPIFunction(const char* name);
