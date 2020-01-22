#ifndef PRINTER_H_INCLUDED
#define PRINTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

class Printer{
public:
    Printer(FILE* print);
    ~Printer();
    void put(const char* text, int textlen);
private:
    FILE* out;
};

#endif // PRINTER_H_INCLUDED
