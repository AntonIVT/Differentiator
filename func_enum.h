#pragma once

#define DEF_FUN(enum, num, str, len, deriv, latex_code, count)\
    enum = num,

enum func_enum
{
    #include "functions.dsl"
    #undef DEF_FUN
};