/*8:6*/

#ifndef _INTEGER_H
#define _INTEGER_H

#include "system_export.h"

/* 这里我们从黑盒外部定义了一个整数类 */

#define SYMBOL_ADD     21
#define SYMBOL_PRINT   22

extern Handle g_Int;

void Int_loadInt();

#endif
