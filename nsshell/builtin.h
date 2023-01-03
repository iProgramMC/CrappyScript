#pragma once

#include "shell.h"
#include "variant.h"

Variant* BuiltInHelp();
Variant* BuiltInVersion();
Variant* BuiltInGetVer();
Variant* BuiltInEcho(Variant* str);
Variant* BuiltInEquals(Variant* str1, Variant* str2);
Variant* BuiltInConcat(Variant* str1, Variant* str2);
