#include "sdlscreen.h"

/* ����ʾ����̨����,GCC����ʱ��-mwindows */
#ifdef _WIN32
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

int main(void)
{
    sdl_init();
    return 0;
}
