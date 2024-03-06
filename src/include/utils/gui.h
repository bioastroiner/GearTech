/*
   Drawing API

   Uses a function pointer to redirect calls from
   main opengl context (in gameloop and cleanup phase)

   methods allocate the required text and sceduel
   them to be shown in the app for easy rendering

   this API ment for debuging but hosts primitive our GUI fundation
*/
#define GUI_HANDLE unsigned int

GUI_HANDLE drawTexto(const char *string);
GUI_HANDLE drawText(const char *string, int x, int y);