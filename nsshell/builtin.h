// CrappyScript (C) 2023 iProgramInCpp

#pragma once

#include "shell.h"
#include "variant.h"

Variant* BuiltInHelp();
Variant* BuiltInVersion();
Variant* BuiltInGetVer();
Variant* BuiltInEcho(Variant* str);
Variant* BuiltInEquals(Variant* str1, Variant* str2);
Variant* BuiltInConcat(Variant* str1, Variant* str2);
Variant* BuiltInToString(Variant* var);
Variant* BuiltInToInt(Variant* var);
Variant* BuiltInAdd(Variant* var1, Variant* var2);
Variant* BuiltInSub(Variant* var1, Variant* var2);
Variant* BuiltInMul(Variant* var1, Variant* var2);
Variant* BuiltInDiv(Variant* var1, Variant* var2);
Variant* BuiltInLessThan(Variant* var1, Variant* var2);
Variant* BuiltInMoreThan(Variant* var1, Variant* var2);
Variant* BuiltInAnd(Variant* var1, Variant* var2);
Variant* BuiltInOr(Variant* var1, Variant* var2);
Variant* BuiltInXor(Variant* var1, Variant* var2);
Variant* BuiltInNot(Variant* var);
Variant* BuiltInBinNot(Variant* var);
Variant* BuiltInLshift(Variant* var1, Variant* var2);
Variant* BuiltInRshift(Variant* var1, Variant* var2);
Variant* BuiltInLessEqual(Variant* var1, Variant* var2);
Variant* BuiltInMoreEqual(Variant* var1, Variant* var2);

void RunnerAddStandardFunctions();
