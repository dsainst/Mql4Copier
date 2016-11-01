#pragma once
#include "ffcTypes.h"

#define TIM_COOK        0
#define BILL_GATES      1
#define V_PUTIN			2

namespace ffc {
int getMagic(wchar_t* comment);
int getMasterTicket(wchar_t* comment);
void writeMqlString(MqlString dest, wchar_t* source);
bool getCocktails(int provider, int name);
}