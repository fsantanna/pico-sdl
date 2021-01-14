#include "../pico.h"

int main (void) {
    pico_init();

    SDL_Event e1;
    int ok = pico_input((Pico_IO){ PICO_EVENT, .Event={SDL_KEYUP,&e1} });
    assert(ok);

    // TITLE
    pico_output((Pico_IO) { PICO_SET_TITLE, .Set_Title="Hello World!" });

    // CLEAR
    pico_output((Pico_IO) { PICO_SET_COLOR_CLEAR,.Set_Color_Clear={0xFF,0xFF,0xFF,0xFF} });
    pico_output((Pico_IO) { PICO_CLEAR });

    pico_input((Pico_IO){ PICO_DELAY, .Delay=2000 });

    // DRAW_PIXEL
    pico_output((Pico_IO) { PICO_SET_COLOR_CLEAR,.Set_Color_Clear={0x00,0x00,0x00,0xFF} });
    pico_output((Pico_IO) { PICO_SET_COLOR_DRAW,.Set_Color_Draw={0xFF,0xFF,0xFF,0xFF} });
    pico_output((Pico_IO) { PICO_CLEAR });
    pico_output((Pico_IO) { PICO_DRAW_PIXEL,.Draw_Pixel={0,0} });

    pico_input((Pico_IO){ PICO_DELAY, .Delay=2000 });

    // DRAW_TEXT
    pico_output((Pico_IO) { PICO_DRAW_TEXT,.Draw_Text={{0,0},"Hello!"} });

    pico_input((Pico_IO){ PICO_DELAY, .Delay=2000 });
    pico_output((Pico_IO) { PICO_CLEAR });

    // WRITE
    pico_output((Pico_IO) { PICO_SET_CURSOR,.Set_Cursor={-25,25} });
    pico_output((Pico_IO) { PICO_WRITE,   .Write="1 " });
    pico_output((Pico_IO) { PICO_WRITE,   .Write="2 " });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="3" });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="" });
    pico_output((Pico_IO) { PICO_WRITELN, .Write="pico" });

    pico_input((Pico_IO){ PICO_DELAY, .Delay=2000 });

    // MOUSE
    SDL_Event e2;
    pico_input((Pico_IO){ PICO_EVENT, .Event={SDL_MOUSEBUTTONDOWN,&e2} });
    pico_output((Pico_IO) { PICO_DRAW_PIXEL, .Draw_Pixel={e2.button.x,e2.button.y} });

    pico_input((Pico_IO){ PICO_DELAY, .Delay=2000 });

    // EVENT
    SDL_Event e3;
    pico_input((Pico_IO){ PICO_EVENT_TIMEOUT, .Event_Timeout={SDL_ANY,5000,&e3} });

    // GET SIZE
    Pico_2i phy;
    Pico_2i log;
    pico_output((Pico_IO) { PICO_GET_SIZE,.Get_Size={&phy,&log} });
    printf("PHY=(%d,%d) // LOG=(%d,%d)\n", phy._1,phy._2, log._1,log._2);

    return 0;
}
