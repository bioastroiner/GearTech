#include "utils/gui.h"

struct GUI_Text
{
    GUI_HANDLE handle;
    char *string;
    unsigned int size;
};

GUI_HANDLE drawTexto(const char *string)
{
    return drawText(string, 0, 0);
}

GUI_HANDLE drawText(const char *string, int x, int y)
{
    return 0;
}
