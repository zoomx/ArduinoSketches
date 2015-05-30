// A montagem consiste em:
// Uma resist�ncia de 1K entre o pino 9 e o centro do cabo RCA
// Uma resist�ncia de 470O entre o pino 7 e o centro do cabo RCA
// Um cabo RCA cujo centro estar� ligado �s resist�ncias e o externo ao ground
// Dois potenci�metros,dos quais uma das extremidades deles ser� ligado em 5v e a outra, no ground
// O centro de um dos potenci�metros estar� ligado ao pino A1 e o do outro ao A2, estes s�o os controles do jogo
// Utiliza a biblioteca TVout dispon�vel em https://code.google.com/p/arduino-tvout/
// Sejam livres para us�-lo e modific�-lo, ficarei feliz em saber sobre novas utilidades, upgrades e modifica��es em geral
// Cr�ditos ao �lvaro Carvalho (A11V1R15@Gmail.com)

#include "TVout.h"
#include "fontALL.h"

TVout TV;
int P1;
int P2;
int S1;
int S2;
int BallX;
int BallY;
int BallDirX;
int BallDirY;
int Vel;

void setup() {
  TV.begin(NTSC,120,96);
  TV.select_font(font8x8);
  Vel = 75;
  S1 = 0;
  S2 = 0;
  BallX = 55;
  BallY = 48;
  BallDirX = -1;
  BallDirY = 0;
}

void loop() {
  TV.clear_screen();
  for (int y=1; y<=96; y+=2){
    TV.draw_line(55,y,55,y,WHITE);
  }
  player();
  ball();
  delay(Vel);
}

void player() {
  P1 = round(analogRead(A2)/12)+1;
  P2 = round(analogRead(A1)/12)+1;
  TV.draw_rect(0,P1-1,2,11,WHITE,WHITE);
  TV.draw_rect(109,P2-1,2,11,WHITE,WHITE);
  TV.print(30,20,S1);
  TV.print(80,20,S2);
}

void ball() {
  ball1();
  ball2();
  if ((BallY > 80)||(BallY < 5)){
    BallDirY *=-1;
  }
  TV.draw_circle(BallX, BallY, 3, INVERT, INVERT);
  BallX += BallDirX;
  BallY += BallDirY;
}

void ball1(){
  if (BallX < 3){                      // Testa aproxima��o com o Jogador 1 
    if ((P1 < BallY)&&(P1+9 > BallY)){ // Testa se o jogador est� ali pra amparar a bola
      BallDirX = 1;                    // Reverte a dire��o da bola, levando-a em dire��o ao player 2
      if (Vel>5){                      // Caso o delay seja maior que 5 milisegundos,
        Vel-=2;                        // Diminui ele pro jogo ficar mais r�pido
      }
      if (BallY == P1){                // Se a bola bater na ponta de cima do jogador,
        BallDirY = -3;                 // Faz a bola ficar com o angulo m�ximo pra cima
      }else if (BallY < P1+3){         // E se quase isso,
        BallDirY = -2;                 // Faz a bola ficar com o angulo medio pra cima    
      }else if (BallY < P1+5){         // E se quase isso,
        BallDirY = -1;                 // Faz a bola ficar com o angulo menor pra cima    
      }else if (BallY == P1+9){        // Se a bola bater na ponta de baixo do jogador,
        BallDirY = 3;                  // Faz a bola ficar com o angulo m�ximo pra baixo
      }else if (BallY > P1+7){         // E se quase isso,
        BallDirY = 2;                  // Faz a bola ficar com o angulo medio pra baixo    
      }else if (BallY > P1+5){         // E se quase isso,
        BallDirY = 1;                  // Faz a bola ficar com o angulo menor pra baixo    
      }else{                           // E se n�o for nada disso, � bem no centro,
        BallDirY = 0;                  // Onde ser� rebatida reto
      }
    }else{                             // E se ele n�o estiver,
      S2+=1;                           // Aumenta o score do advers�rio,
      BallX = 55;                      // Reseta a posi��o da bola,
      BallY = 48;                      // Mas n�o sua dire��o,
      Vel = 75;                        // E volta pro valor inicial do Delay
    }
  }
}

void ball2(){
  if (BallX > 108){                    // Testa aproxima��o com o Jogador 2 
    if ((P2 < BallY)&&(P2+9 > BallY)){ // Testa se o jogador est� ali pra amparar a bola
      BallDirX = -1;                   // Reverte a dire��o da bola, levando-a em dire��o ao player 1
      if (Vel>5){                      // Caso o delay seja maior que 5 milisegundos,
        Vel-=2;                        // Diminui ele pro jogo ficar mais r�pido
      }
      if (BallY == P2){                // Se a bola bater na ponta de cima do jogador,
        BallDirY = -3;                 // Faz a bola ficar com o angulo m�ximo pra cima
      }else if (BallY < P2+3){         // E se quase isso,
        BallDirY = -2;                 // Faz a bola ficar com o angulo medio pra cima    
      }else if (BallY < P2+5){         // E se quase isso,
        BallDirY = -1;                 // Faz a bola ficar com o angulo menor pra cima    
      }else if (BallY == P2+9){        // Se a bola bater na ponta de baixo do jogador,
        BallDirY = 3;                  // Faz a bola ficar com o angulo m�ximo pra baixo
      }else if (BallY > P2+7){         // E se quase isso,
        BallDirY = 2;                  // Faz a bola ficar com o angulo medio pra baixo    
      }else if (BallY > P2+5){         // E se quase isso,
        BallDirY = 1;                  // Faz a bola ficar com o angulo menor pra baixo    
      }else{                           // E se n�o for nada disso, � bem no centro,
        BallDirY = 0;                  // Onde ser� rebatida reto
      }
    }else{                             // E se ele n�o estiver,
      S1+=1;                           // Aumenta o score do advers�rio,
      BallX = 55;                      // Reseta a posi��o da bola,
      BallY = 48;                      // Mas n�o sua dire��o,
      Vel = 75;                        // E volta pro valor inicial do Delay
    }
  }
}