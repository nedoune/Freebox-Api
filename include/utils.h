#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;

void stringtochar(string &str, char* &chaine);

void stringtoUchar(string &str, unsigned char* &chaine);

void chartostring(char* chaine, string &str);

void chartohexstring(char* chaine, string &str);

void Uchartohexstring(unsigned char* chaine,unsigned int lentgh, string &str);

void inttochar(int i, char* &chaine);

#endif // UTILS_H_INCLUDED
