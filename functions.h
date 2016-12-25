#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structures.h"
#include "includes.c"

int InitEverything(int , int , SDL_Window** , SDL_Renderer** );
int InitSDL();
int InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window* );
void RenderMap(square **mapa, robot *head, int nSquareW, int nSquareH, int pausa, SDL_Renderer* _renderer);
int RenderText(int , int , const char* , TTF_Font *, SDL_Renderer* );
int RenderNumber(int , int , int* , TTF_Font *, SDL_Renderer* );
int RenderLogo(int , int , SDL_Renderer* );
void File(robot *head, square** mapa); //Gera o ficheiro estatísticas.txt

pstack create_stack();  //cria o stack
void push(pstack topo, int x, int y);  //push no stack


void MoveRobots(square **mapa, robot *head, int nSquareW, int nSquareH, int MAX_BATTERY_DECREASE); //movimenta o robot
void adiciona_robot(square **mapa, robot *head, int posX, int posY);   //adiciona um robot ao mapa
void next_position(int *_xc, int *_yc, int xt, int yt);    //calcula a proxima posição de cada um dos robots

square** read_mapa(FILE *file, int *nSquareW, int *nSquareH, int *ocupados); //retorna uma matriz de estruturas com a informação no ficheiro
square** create_matrix(int, int); //criação da matriz do mapa
square** atualiza_mapa(square **mapa, robot *iclean, int number_robots); //insere as informações dos robots no mapa
robot* alocaRobot(int nr);    //aloca memória para a lista de robots
robot* read_robot(FILE *file, int ocupados, int *number_robots, int nSquareW);   //lê a informação acerca dos robots do ficheiro txt
robot* clean_if_zero(square **mapa, robot *iclean, int nSquareH, int nSquareW);    //retira robots caso este fiquem com a bateria <= 0


#endif // FUNCTIONS
