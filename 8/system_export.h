#ifndef _SYSTEM_EXPORT_H
#define _SYSTEM_EXPORT_H

#include <stdarg.h>

struct Object;
typedef struct Object Object;
typedef struct Object *PObject;
struct Slot;
typedef struct Slot Slot;
typedef struct Slot *PSlot;
typedef PObject *Handle;

/* 这里把符号定义为整型 */
typedef int Symbol;

/* 消息处理函数 */
typedef Handle(*MsgFunc)(Handle obj, Handle msg_func, va_list args);

/* 
 * 这里定义了用到的符号
 * 更合理的做法是使用符号表或哈希算法等
 */
/* 默认槽位大小 */
#define DEFAULT_SLOT_CNT  20
/* 默认的符号 */
#define SYMBOL_PROTO    1
#define SYMBOL_TYPE     2
#define SYMBOL_SLOTCNT  3
#define SYMBOL_GETSLOT  4
#define SYMBOL_SETSLOT  5
#define SYMBOL_CLONE    6
#define SYMBOL_EXEC     7
#define SYMBOL_IDENT    8
#define SYMBOL_MAKE     9
/* 空白符号 */
#define NIL 0

/*
* 向对象obj发送msg消息，后面的是参数
*/
Handle send(Handle obj, Symbol msg, ...);

/*
 */
Handle Block_make(MsgFunc msg_func, int trap_cnt, ...);

void System_init();
void System_final();

/* 进入新的函数 */
void MH_enter();

/* 函数返回 */
void MH_return(Handle handle);

/* g_全局对象 */
extern Handle g_Object;
extern Handle g_Block;

void Obj_setExtra(Handle obj, int val);
int Obj_getExtra(Handle obj);

#endif