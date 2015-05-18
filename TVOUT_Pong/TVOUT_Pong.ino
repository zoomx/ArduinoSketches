/*

TVOUT_Pong

PAL version

*/

#include <TVout.h>
#include "Paddle.h"
#include "Controller.h"

TVout out;
unsigned char ox, oy;
unsigned char tx, ty;
boolean game = false;

void setup() {
  ox = 0;
  oy = 0;
  tx = 0;
  ty = 0;
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  
  out.start_render(_PAL);
  out.clear_screen();
}

boolean credscrn = false;

void loop() { 
   if (!game) { 
     intro();
     if (Controller::hasBeenPressed()) { 
       credscrn = true;
     }
     if (credscrn) { 
       out.clear_screen();
       out.print_str(out.horz_res() / 3, out.vert_res() / 2, "Credits: 1");
       out.delay(90);
       out.print_str((out.horz_res() / 3) - 10, out.vert_res() / 3, "Ready to Play");
       out.delay(90);
       out.clear_screen();
       game = true;
     }
   } else { 
     credscrn = false;
   }
}

void intro() {
  out.print_str(out.horz_res() / 3, 10, "PONG TV");
  out.print_str(0, 30, "Press to start");
  out.print_str(out.horz_res() / 5, out.vert_res() - 30, "by Juan Mendez");
  out.print_str(2, out.vert_res() - 20, "Credits: 0");
}

