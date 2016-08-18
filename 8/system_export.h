/*8:2*/
/* object.h是给内部用的，system_export才是给外部用的 */

#ifndef _SYSTEM_EXPORT_H
#define _SYSTEM_EXPORT_H

#include <stdarg.h>

/* 未使用的参数 */
#define UNUSED(param) (void)(param)

struct Object;
typedef struct Object Object;
typedef struct Object *PObject;
struct Slot;
typedef struct Slot Slot;
typedef struct Slot *PSlot;
typedef PObject *Handle;

/* 这里把符号定义为整型 */
typedef int Symbol;

/* 
 * 消息处理函数，Block/Closure对象用
 * obj      : 消息的受体
 * msg_func : 代码块/闭包对象本身
 * args     : 所有参数
 */
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
* 这是我们正常编程最常用的方法
* 槽查找时会递归地查找原型
* 遇到对象槽就返回，遇到闭包就返回其执行结果
*/
Handle send(Handle obj, Symbol msg, ...);

/*
 * 用msg_func做成代码块/闭包
 * trap_cnt : 闭包捕获变量个数
 * 后面接slot1, val1, slot2, val2, ...
 * 表示每个捕获变量的槽位符号，以及句柄
 */
Handle Block_make(MsgFunc msg_func, int trap_cnt, ...);

/* 对象系统初始化 */
void System_init();

/* 销毁对象系统 */
void System_final();

/* 进入新的函数 */
void MH_enter();

/* 函数返回，void函数可以直接写NULL */
void MH_return(Handle handle);

/* g_全局对象 */
extern Handle g_Object;
extern Handle g_Block;

/* 设置Int型额外数据 */
void Obj_setExtra(Handle obj, int val);

/* 取得Int型额外数据 */
int Obj_getExtra(Handle obj);

#endif
