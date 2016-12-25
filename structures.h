#ifndef STRUCTURES_H
#define STRUCTURES_H

#define DIM 3

typedef struct position
{
    int x;
    int y;
} position;

typedef struct pos_stack   //pilha que guarda as posições limpas de um robot
{
    int x;
    int y;
    struct pos_stack *next;  //apontados para a proxima posição limpa
} no_stack;

typedef no_stack * pstack;

typedef struct robot
{
    char nome[DIM];
    double bateria;
    position actual;  //posição atual do robot
    position target;  //posição a alcançar pelo robot
    int percorridos;  //variavel que vai guardar o numero de quadrados percorridos pelo robot
    struct robot *next;  //apontador para o robot seguinte
    struct pstack *limpos;  //apontador para a stack de posições limpas desse robot
} robot;

typedef struct square
{
    int estado;    //0 - sujo, 1 - limpo, 2 - obstáculo
    int estarobot; //0 - vazio, 1 - com robot
    int target; //0 - não é destino, 1 - a posição é destino de algum robot
    struct robot *probot;
} square;

#endif // STRUCTURES


