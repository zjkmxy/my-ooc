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

/* ����ѷ��Ŷ���Ϊ���� */
typedef int Symbol;

/* ��Ϣ������ */
typedef Handle(*MsgFunc)(Handle obj, Handle msg_func, va_list args);

/* 
 * ���ﶨ�����õ��ķ���
 * �������������ʹ�÷��ű���ϣ�㷨��
 */
/* Ĭ�ϲ�λ��С */
#define DEFAULT_SLOT_CNT  20
/* Ĭ�ϵķ��� */
#define SYMBOL_PROTO    1
#define SYMBOL_TYPE     2
#define SYMBOL_SLOTCNT  3
#define SYMBOL_GETSLOT  4
#define SYMBOL_SETSLOT  5
#define SYMBOL_CLONE    6
#define SYMBOL_EXEC     7
#define SYMBOL_IDENT    8
#define SYMBOL_MAKE     9
/* �հ׷��� */
#define NIL 0

/*
* �����obj����msg��Ϣ��������ǲ���
*/
Handle send(Handle obj, Symbol msg, ...);

/*
 */
Handle Block_make(MsgFunc msg_func, int trap_cnt, ...);

void System_init();
void System_final();

/* �����µĺ��� */
void MH_enter();

/* �������� */
void MH_return(Handle handle);

/* g_ȫ�ֶ��� */
extern Handle g_Object;
extern Handle g_Block;

void Obj_setExtra(Handle obj, int val);
int Obj_getExtra(Handle obj);

#endif