#include "../pico.h"

int main (void) {
    pico_open();

    SDL_Event e1;
    int ok = pico_input(&e1, (Pico_IO){ PICO_EVENT, .Event=SDL_KEYUP });
    assert(ok);

    // TITLE
    pico_output((Pico_IO) { PICO_SET_TITLE, .Set_Title="Hello World!" });

    // CLEAR
    pico_output((Pico_IO) { PICO_SET_COLOR_CLEAR,.Set_Color_Clear={0xFF,0xFF,0xFF,0xFF} });
    pico_output((Pico_IO) { PICO_CLEAR });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

    // DRAW_PIXEL
    pico_output((Pico_IO) { PICO_SET_COLOR_CLEAR,.Set_Color_Clear={0x00,0x00,0x00,0xFF} });
    pico_output((Pico_IO) { PICO_SET_COLOR_DRAW,.Set_Color_Draw={0xFF,0xFF,0xFF,0xFF} });
    pico_output((Pico_IO) { PICO_CLEAR });
    pico_output((Pico_IO) { PICO_DRAW_PIXEL,.Draw_Pixel={0,0} });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

    // DRAW_TEXT
    pico_output((Pico_IO) { PICO_DRAW_TEXT,.Draw_Text={{0,0},"Hello!"} });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });
    pico_output((Pico_IO) { PICO_CLEAR });

    // WRITE
    pico_output((Pico_IO) { PICO_SET_CURSOR,.Set_Cursor={-25,25} });
    pico_output((Pico_IO) { PICO_WRITE,   .Write="1 " });
    pico_output((Pico_IO) { PICO_WRITE,   .Write="2 " });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="3" });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="" });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="pico" });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

    // MOUSE
    SDL_Event e2;
    pico_input(&e2, (Pico_IO){ PICO_EVENT, .Event=SDL_MOUSEBUTTONDOWN });
    pico_output((Pico_IO) { PICO_DRAW_PIXEL, .Draw_Pixel={e2.button.x,e2.button.y} });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

    // EVENT
    SDL_Event e3;
    pico_input(&e3, (Pico_IO){ PICO_EVENT_TIMEOUT, .Event_Timeout={SDL_ANY,5000} });

    // GET SIZE
    Pico_2i size;
    pico_output((Pico_IO) { PICO_GET_SIZE,.Get_Size=&size });
    printf("SIZE=(%d,%d)\n", size._1,size._2);

    // GRID=0
    pico_output((Pico_IO){ PICO_SET_GRID,.Set_Grid=0 });

    // AUTO=0
    pico_output((Pico_IO){ PICO_CLEAR });
    pico_output((Pico_IO){ PICO_SET_AUTO,.Set_Auto=0 });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="no auto" });
    pico_output((Pico_IO) { PICO_PRESENT });

    pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=2000 });

    // AUTO=1
    pico_output((Pico_IO){ PICO_SET_AUTO,.Set_Auto=1 });
    pico_output((Pico_IO){ PICO_CLEAR });

    // ZOOM
    for (int i=0; i<19; i++) {
        int pct = 100 - i*5;
        pico_output((Pico_IO){ PICO_SET_SIZE,.Set_Size={_WIN_,_WIN_} });
        pico_output((Pico_IO){ PICO_SET_ZOOM,.Set_Zoom={pct,pct} });
        pico_output((Pico_IO){ PICO_CLEAR });
        //pico_output((Pico_IO) { PICO_DRAW_TEXT,.Draw_Text={{0,0},"X"} });
        pico_output((Pico_IO) { PICO_DRAW_RECT,.Draw_Rect={{0,0},{25,25}} });
        pico_output((Pico_IO) { PICO_SET_COLOR_DRAW,.Set_Color_Draw={0xFF,0x00,0x00,0xFF} });
        pico_output((Pico_IO) { PICO_DRAW_LINE,.Draw_Line={{0,0},{25,25}} });
        pico_output((Pico_IO) { PICO_SET_COLOR_DRAW,.Set_Color_Draw={0xFF,0xFF,0xFF,0xFF} });
        pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=200 });
    }

    // PAN

    for (int i=0; i<10; i++) {
        pico_output((Pico_IO){ PICO_SET_PAN,.Set_Pan={i,0} });
        pico_output((Pico_IO){ PICO_CLEAR });
        pico_output((Pico_IO) { PICO_DRAW_TEXT,.Draw_Text={{0,0},"Uma frase bem grande"} });
        pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=500 });
    }

    for (int i=0; i<20; i++) {
        pico_output((Pico_IO){ PICO_SET_PAN,.Set_Pan={10-i,-i} });
        pico_output((Pico_IO){ PICO_CLEAR });
        pico_output((Pico_IO) { PICO_DRAW_TEXT,.Draw_Text={{0,0},"Uma frase bem grande"} });
        pico_input(NULL, (Pico_IO){ PICO_DELAY, .Delay=500 });
    }

    pico_close();
    return 0;
}
