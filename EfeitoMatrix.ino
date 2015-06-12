/*
 Efeito Matrix on TVout
 by Gustavo Sampaio
 https://bitunico.wordpress.com/2012/08/07/conectando-o-arduino-a-tv-hackeando-a-tv/
 https://github.com/GustavoKatel

*/

#include <TVout.h>
#include <fontALL.h>
 
TVout tv;
 
char chs[]={'#','$','%','&','a','L'};
 
void setup()
{
   tv.begin(PAL,120,96);
   tv.select_font(font6x8);
   tv.println("nn   GustavoKatel");
   tv.delay(3000);
   tv.clear_screen();
   tv.fill(1);
}
 
void loop()
{
   tv.fill(0);
   randomSeed(analogRead(0));
   for(int j=0;j<=120;j++)
   {
      int col=  random(120);
      for(int i=0;i<=96;i++)
      {
         tv.print_char(col,i,chs[random(6)]);
         tv.delay(velocidade());
      }
   }
   tv.fill(1);
}
 
//
int velocidade()
{
   int x = analogRead(1); //map(analogRead(1), 0, 255, 1, 1000);
   return x;
}
