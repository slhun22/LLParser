#pragma once

#include "lexicalAnalyzer.h"
#include <unordered_map>

void printSubResult();
void printFinalResult();
void left_paren();
void right_paren();
void mult_op();
void add_op();
void semi_colon();
void assignment_op();
void ident();
void constant();
float factor();
float factor_tail(float);
float term();
float term_tail(float);
float expression();
void statement();
void statements();
void program();
