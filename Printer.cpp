#include "Printer.h"

Printer::Printer(FILE* out){
    this->out = out;
}
Printer::~Printer(){
}

void Printer::put(const char* text, int textlen){
    fwrite(text, 1, textlen, this->out);
    fflush(this->out);
}
