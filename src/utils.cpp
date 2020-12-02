#include"utils.h"


void stringtochar(string &str, char* &chaine){
        chaine = strcpy((char*)malloc(str.size()+1),str.c_str());

}

void stringtoUchar(string &str, unsigned char* &chaine){
    chaine = (unsigned char*)realloc(chaine,str.size()+1);
    for(int i=0;i<(int)str.size();i++){
        chaine[i]=str.at(i);
    }
    chaine[str.size()]=0x00;
}

void chartostring(char* chaine, string &str){
    ostringstream oss;
    for(int i=0; i< (int)strlen(chaine);i++){
        oss<<chaine[i];
    }
    str = oss.str();
}

void chartohexstring(char* chaine, string &str){
    ostringstream oss;
    for(int i=0; i< (int)strlen(chaine);i++){
        oss<<setfill('0') << setw(2)<<hex<<chaine[i];
    }
    str = oss.str();
}

void Uchartohexstring(unsigned char* chaine,unsigned int lentgh, string &str){
    ostringstream oss;
    for(int i=0; i< (int)lentgh;i++){
        oss<<setfill('0') << setw(2)<<hex<<(int)chaine[i]; //setfill et setw permette de ne pas perdre les 0 a l'intérieur de la chaîne
    }
    str = oss.str();
}

void inttochar(int i, char* &chaine){

    stringstream str;
    str << i;
    string s = str.str();
    chaine=(char*)realloc(chaine,sizeof(char)*s.size()+1);
    stringtochar(s,chaine);
    chaine[s.size()]=0x00;
}

