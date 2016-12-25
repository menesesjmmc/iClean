/**
Autores: José Correia e António Castilho
Número de aluno: 81570 e 80764

Projeto: Simulação da frota de robots de limpeza iClean
Destino: Cadeira de Programação

Data de finalização: 22 de maio de 2015

comentário adicionais no ficheiro 'comments.c'

*/

#include "functions.h"
#include "structures.h"
#include "constants.h"
#include "includes.c"

/**
 * main function: entry point of the program
 */

int main( int argc, char* argv[])
{
    int nSquareW = 0;   //largura da divisória
    int nSquareH = 0;   //altura da divisória
    int ocupados = 0;   //numero de obstáculos
    int number_robots = 0;    //número de robots
    SDL_Event event;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int width = 0;
    int height = 0;
    int delay = 1000;
    int fim=0;   //int fim para detetar quando esta tudo limpo;
    int posX, posY, a, b, key = 0, key2 = 0, key3 = 0;
    int MAX_BATTERY_DECREASE; //decrescimo máximo de bateria
    char str[NORMAL_DIM];

    square** mapa;   //nodo da estrutura do mapa da divisória
    robot* head;  //nodo da estrutura robot

    strcpy(str, argv[2]);
    sscanf(str, "%d", &MAX_BATTERY_DECREASE); //leitura do valor de decaimento da bateria

    /**Leitura do ficheiro: */

    FILE *map_file;

    map_file = fopen(argv[1], "r");

    if (map_file == NULL)   //caso em que o ficheiro é inexistente
    {
        printf("Ficheiro do mapa inexistente!!\n");
        exit(1);
    }

    mapa = read_mapa(map_file, &nSquareW, &nSquareH, &ocupados);   //aloca memória para o mapa da divisória

    head = read_robot(map_file, ocupados, &number_robots, nSquareW);  //aloca memória para a lista de robots

    fclose(map_file);

    printf("Ficheiro do mapa: %s.txt\n\n", argv[1]);
    printf("Largura: %d\nAltura: %d\n\n", nSquareW, nSquareH);
    printf("Número de robots: %d\n", number_robots);
    printf("Valor máximo de decaimento da bateria: %d\n\n", MAX_BATTERY_DECREASE);

    robot* aux1 = head;   //ponteiro auxiliar da lista de robots


    /**Impressão das informações dos robots iniciais na consola:
    */

    for(; aux1 != NULL; aux1 = aux1 -> next)
    {
        printf("%s     Posição inicial: [%d, %d]    Bateria: %lf\n", aux1->nome, aux1->actual.x, aux1->actual.y, aux1->bateria);

        aux1->target.x = aux1->actual.x;
        aux1->target.y = aux1->actual.y;

    }

    mapa = atualiza_mapa(mapa, head, number_robots); //insere as informações de cada robot no mapa;

    width = SQUARE_SIZE*nSquareW;
    height = SQUARE_SIZE*nSquareH;

    // initialize graphics
    if ( !InitEverything(width, height, &window, &renderer) )
        return -1;

    RenderMap(mapa, head, nSquareW, nSquareH, key, renderer);  //Render inicial da janela

    SDL_Delay( delay );

    while(fim!=1)
    {
        key = 0;
        key2 = 0;

        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                return(EXIT_SUCCESS);
            }
            else if ( event.type == SDL_KEYDOWN )  //Detecta o uso do teclado
            {
                switch ( event.key.keysym.sym )
                {
                case SDLK_q:                     //caso seja premido q, sai da aplicação
                    return(EXIT_SUCCESS);
                    break;
                case SDLK_DOWN:
                    if(delay<=1000)
                    {
                        delay=delay+200;  //menos velocidade
                    }
                    break;
                case SDLK_UP:
                    if(delay>=90)
                    {
                        delay=delay-80; //mais velocidade
                    }
                    break;
                case SDLK_p:   //caso seja premida a tecla p (pausa)
                    key = 1;
                    while(key != 0)
                    {
                        RenderMap(mapa, head, nSquareW, nSquareH, key, renderer);  //insere a palavra "PAUSA" no ecrã

                        while (SDL_PollEvent( &event ))
                        {
                            if ( event.type == SDL_KEYDOWN )
                            {
                                switch ( event.key.keysym.sym )
                                {
                                case SDLK_a:  //caso seja premida a tecla a, dentro da pausa
                                    while(key2 != 1)
                                    {
                                        while( SDL_PollEvent( &event ) )
                                        {
                                            if ( event.type == SDL_MOUSEBUTTONDOWN )   //deteta o uso do rato
                                            {
                                                SDL_GetMouseState( &posX, &posY); //deteta o local onde o utilizador clica para adicionar um robot
                                                posX = posX / SQUARE_SIZE;
                                                posY = posY / SQUARE_SIZE;

                                                if (posX >= nSquareW || posY >= nSquareH)   //para o caso de o utilizador tentar inserir o obot fora do mapa da divisória
                                                {
                                                    printf("\nInsira uma posição no interior do mapa!!\n");
                                                    while(key3 != 1)
                                                    {
                                                        while( SDL_PollEvent( &event ) )
                                                        {
                                                            if ( event.type == SDL_MOUSEBUTTONDOWN )
                                                            {
                                                                SDL_GetMouseState( &a, &b); //deteta o local onde o utilizador clica para adicionar um robot
                                                                posX = a / SQUARE_SIZE;
                                                                posY = b / SQUARE_SIZE;

                                                                key3 = 1;
                                                            }
                                                        }
                                                    }

                                                }

                                                key3 = 0;

                                                if(mapa[posX][posY].estado != 2 && mapa[posX][posY].estarobot != 1)  //se a posição escolhida tiver um robot ou obstaculo, nao faz nada
                                                {
                                                    printf("\nPosição selecionada: [%d, %d]\n", posX, posY);

                                                    if(posX < nSquareW && posY <nSquareH)
                                                    {
                                                        adiciona_robot(mapa, head, posX, posY);     //adiciona um robot na zona selecionada
                                                        RenderMap(mapa, head, nSquareW, nSquareH, key, renderer);
                                                    }
                                                }
                                                key2 = 1;
                                            }
                                        }
                                    }
                                    key2 = 0;
                                    break;
                                case SDLK_p: //caso seja premido de novo a tecla p, sai da pausa
                                    key = 0;
                                    break;
                                case SDLK_q:           //caso seja premido q, sai da aplicação
                                    return(EXIT_SUCCESS);
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case SDLK_i:                          //caso seja pressionado i, reinicia

                    map_file = fopen("Map1", "r");

                    if (map_file == NULL)
                    {
                        printf("Ficheiro do mapa inexistente!!\n");
                        exit(1);
                    }

                    mapa = read_mapa(map_file, &nSquareW, &nSquareH, &ocupados);

                    head = read_robot(map_file, ocupados, &number_robots, nSquareW);

                    mapa = atualiza_mapa(mapa, head, number_robots);

                    fclose(map_file);

                    RenderMap(mapa, head, nSquareW, nSquareH, key, renderer);
                    break;

                case SDLK_e:                           //caso seja pressionado e, escreve o ficheiro das estatísticas
                    File(head, mapa);
                    fprintf(stderr, "\nO ficheiro estatisticas.txt foi gerado com sucesso!\n");
                default:
                    break;
                }
            }
        }

        //GERAÇÃO DE NOVAS POSIÇÕES E MOVIMENTO DOS ROBOTS:

        MoveRobots(mapa, head, nSquareW, nSquareH, MAX_BATTERY_DECREASE);    //move cada um dos robots

        RenderMap(mapa, head, nSquareW, nSquareH, key, renderer);

        SDL_Delay( delay );

    }

    printf("\nA simulação dos robots iClean foi terminada com sucesso!\n\n");

    SDL_Delay(1500);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(mapa);
    free(aux1);
    free(head);

    return 1;
}
