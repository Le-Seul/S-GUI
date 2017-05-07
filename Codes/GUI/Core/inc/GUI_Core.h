#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI_Config.h"
#include "GUI_Typedef.h"
#include "GUI_Bottom.h"
#include "GUI_Queue.h"
#include "GUI_Font.h"

#define GUI_ALIGN_TOP     0x01 /* �������� */
#define GUI_ALIGN_BOTTOM  0x02 /* �ײ����� */
#define GUI_ALIGN_VCENTER 0x04 /* ��ֱ���Ķ��� */
#define GUI_ALIGN_LEFT    0x08 /* ����� */
#define GUI_ALIGN_RIGHT   0x10 /* �Ҷ��� */
#define GUI_ALIGN_HCENTER 0x20 /* ˮƽ���� */

#define GUI_HEAP_FAST     0 /* ���ٵĶ�(MCU�ڲ���SRAM)ҳ�� */
#define GUI_HEAP_HCAP     1 /* �������Ķ�(����SDRAM)ҳ�� */

#define GUI_DEBUG_OUT(s) GUI_DebugOut(s);

GUI_RESULT GUI_Init(void);
void GUI_Unload(void);
void GUI_Delay(GUI_TIME tms);
void GUI_LOCK(void);
void GUI_UNLOCK(void);
void GUI_DebugOut(const char *s);

#endif /* __GUI_CORE_H */
