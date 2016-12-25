
#include "structures.h"
#include "functions.h"
#include "constants.h"
#include "includes.c"

const char myName1[] = "Jose Correia";
const char myNumber1[] = "81570";
const char myName2[] = "Antonio Castilho";
const char myNumber2[] = "80764";

/**read_mapa: lê do ficheiro as informações do mapa
*/
square** read_mapa(FILE *file, int *nSquareW, int *nSquareH, int *ocupados)
{
    int i, x, y, n_SquareW, n_SquareH, _ocupados;
    char str[BIG_DIM];    //string para guardar as posições dos obstáculos

    square** mapa = NULL;

    fscanf(file, "%d %d %d", &n_SquareW, &n_SquareH, &_ocupados);    //lẽ do ficheiro a largura, altura e numero de obstáculos

    mapa = create_matrix(n_SquareW, n_SquareH); //cria a matriz

    for(i = 0; i <= _ocupados; i++)     //ciclo que vai buscar ao ficheiro as coordenadas dos obstáculos
    {
        fgets(str, NORMAL_DIM, file);   //guarda na string cada uma das linhas das coordenadas dos obstaculos
        if(i > 0)
        {
            sscanf(str, "[%d, %d]", &x, &y);   //lê as posições dos obstaculos da string
            mapa[x][y].estado = OBSTACULO;
        }
    }

    *nSquareH = n_SquareH;
    *nSquareW = n_SquareW;
    *ocupados = _ocupados;

    return mapa;
}


/**read_robot: lê do ficheiro as informações dos robots e insere-as numa pilha
*/

robot* read_robot(FILE *file, int ocupados, int *number_robots, int nSquareW)
{
    int i, j, fim_nr, _number_robots, x, y;
    double bateria;
    char robot_name[DIM];

    robot * new_iclean = NULL;
    robot * head = NULL;

    fim_nr = ocupados + FILE_UNWANTED_LINES;

    while(i < fim_nr)
    {
        if(i>ocupados && i<ocupados + FILE_UNWANTED_LINES)
        {
            fscanf(file, "%d", &_number_robots);  //lê o número de robots
        }
        i++;
    }

    for(j=0; j<_number_robots; j++)
    {
        new_iclean = alocaRobot(_number_robots);
        fscanf(file, "%s [%d, %d] %lf", robot_name, &x, &y, &bateria);  //lê a informação sobre cada um dos robots

        //copt the name of the robot
        strcpy(new_iclean->nome, robot_name);   //copia o nome guardado na string para a estrutura

        new_iclean->bateria = bateria;

        new_iclean->actual.x = x;
        new_iclean->actual.y = y;

        new_iclean->percorridos = 1;   //inicialização do nr. de percorridos a 1 (quadrado inicial de cada robot)

        //caso em que a lista está vazia
        if(head == NULL)
        {
            head = new_iclean;
            new_iclean -> next = NULL;
            continue;
        }
        //caso em que o robot tem que ser inserido antes da head da lista
        if( head->nome[1] >= robot_name[1])
        {
            new_iclean->next = head;
            head = new_iclean;
        }

        //inserir no meio/fim da lista
        else
        {
            //inicialização de dois ponteiros, um para a seguir ao outro
            robot *aux1 = head;
            robot *aux2 = head->next;

            //encontra o sítio na lista onde o robot deve ser inserido
            while( aux2 != NULL && aux2->nome[1] <= robot_name[1])
            {
                aux1 = aux1->next;
                aux2 = aux2->next;
            }
            //insere o robot na lista
            aux1->next = new_iclean;
            new_iclean->next = aux2;

        }


    }

    *number_robots = _number_robots;

    return head;
}

/**create_matrix: cria a matriz do mapa, alocando memoria para a mesma
*/

square** create_matrix(int n, int m)  //Função que aloca memoria e cria a matriz do mapa; n:  largura, m: altura
{
    int i, j;
    int area = n * m;

    square** map = NULL;

    map = (square **)malloc(area*sizeof(square *));   //alocação de memória para todos os quadrados do mapa

    if(map == NULL) //Caso haja um erro de alocação de memória
    {
        printf("Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    for(i=0; i<n; i++)
    {
        map[i] = (square *)malloc(m*sizeof(square)); //alocação de memória dos apontadores para cada linha

        if(map[i] == NULL) //Caso haja um erro de alocação de memória
        {
            printf("Erro de alocação de memória\n");
            exit(EXIT_FAILURE);
        }

    }

    for(i=0; i < n; i++)  //ciclo que dá a todos os valores da matriz de estrutura, o valor 0
    {
        for(j=0; j < m ; j++)
        {
            map[i][j].estarobot = VAZIO;
            map[i][j].estado = SUJO;
            map[i][j].target = VAZIO;
        }
    }

    return map;
}

/**alocarobot : aloca a memoria para a estrutura de robots
*/
robot* alocaRobot(int nr)
{
    robot * iclean = NULL;

    iclean = (robot *)malloc(nr*sizeof(robot));  //aloca memória para os iclean

    if(iclean == NULL) //caso haja um erro de alocação
    {
        printf("Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    return iclean;
}

/** MoveRobot: movimentação do robot
*/

void MoveRobots(square **mapa, robot *head, int nSquareW, int nSquareH, int MAX_BATTERY_DECREASE)
{
    int next_x, next_y, i = 0;

    robot *aux;

    for (aux = head; aux!=NULL; aux = aux->next)
    {
        aux = clean_if_zero(mapa, aux, nSquareH, nSquareW);   //retira o robot da lista caso a bateria deste chegue a zero

        mapa[aux->target.x][aux->target.y].target = 0;

        if (aux->actual.x == aux->target.x && aux->actual.y == aux->target.y) //se o robot estiver na posição de destino gera uma nova
        {
            while(mapa[aux->target.x][aux->target.y].estado != SUJO) /**NAO POSSO FAZER ISTO!!*/ //gera uma posição de destino que ainda não esteja limpa
            {
                aux -> target.y = rand()%(nSquareH);
                aux -> target.x = rand()%(nSquareW);
            }
        }

        mapa[aux->actual.x][aux->actual.y].estarobot = VAZIO;

        next_x = aux->actual.x;
        next_y = aux->actual.y;

        next_position(&next_x, &next_y, aux->target.x, aux->target.y);    //gera a proxima posição

        i=0;

        while(mapa[next_x][next_y].estado == OBSTACULO || mapa[next_x][next_y].estarobot != VAZIO)    //se houver um obstáculo ou um robot em frente, o robot gera uma nova posição de destino
        {
            i++;
            next_x = aux->actual.x;
            next_y = aux->actual.y;
            aux -> target.y = rand()%(nSquareH);
            aux -> target.x = rand()%(nSquareW);

            if(i>=2)      //de modo a que os robots nao andem demasiado colados a objetos (optimização)
            {
                while(aux->target.x >= nSquareW && aux->target.y >= nSquareH)
                {
                    aux -> target.y = aux->actual.x - LIMITE_NEWPOS_ROBOTPRESO + rand()%(aux->actual.x + LIMITE_NEWPOS_ROBOTPRESO);
                    aux -> target.x = aux->actual.y - LIMITE_NEWPOS_ROBOTPRESO + rand()%(aux->actual.y + LIMITE_NEWPOS_ROBOTPRESO);
                }
            }

            next_position(&next_x, &next_y, aux->target.x, aux->target.y);

        }

        aux->actual.x = next_x;
        aux->actual.y = next_y;

        mapa[aux->target.x][aux->target.y].probot = aux;  //liga cada posição de destino ao robot que se dirige para esta
        mapa[aux->target.x][aux->target.y].target = DESTINO;  //insere no mapa a posição target do robot de modo a poder marcá-la na divisoria
        mapa[aux->actual.x][aux->actual.y].estarobot = OCUPADO;  //da o valor 1 ao local da matriz mapa onde se encontra cada um dos robots
        mapa[aux->actual.x][aux->actual.y].probot = aux;  //liga a posição em que está o robot à estrutura com a sua informação

        aux -> bateria -= BATTERY_MIN_DECREASE + (rand()%((MAX_BATTERY_DECREASE)*1000))/1000.0;  //diminui o valor da bateria entre 0.1 e MAX_BATTERY_DECREASE

        aux->percorridos += 1;

    }

    free(aux);
}

/**next_position: calcula a proxima posição a ocupar pelo robot
*/

void next_position(int *_xc, int *_yc, int xt, int yt)
{
    int xc, yc;
    double angle;

    // just copy the values
    xc = *_xc;
    yc = *_yc;

    // calculate the angle
    angle = atan2((double)(yc-yt), (double)(xt-xc));

    // calculate the new position
    *_xc = floor(xc + cos(angle)+0.5);
    *_yc = floor(yc - sin(angle)+0.5);
}

/**create_stack(): cria o stack que guarda as posições limpas de cada robot
*/

pstack create_stack()
{
    pstack novo;

    novo = (pstack)malloc(sizeof(no_stack));

    if (novo == NULL)
    {
        printf("Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }
    novo->next = NULL;
    return novo;
}


/** push: faz push no stack criado
*/

void push(pstack topo, int x, int y)
{
    pstack novo;
    novo = (pstack)malloc(sizeof(no_stack));
    if (novo == NULL)
    {
        printf("Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    novo->x = x;
    novo->y = y;
    novo->next = topo->next;
    topo->next = novo;
}

/**atualiza_mapa: insere na matriz do mapa a informação de onde se encontra cada um dos robots
*/

square** atualiza_mapa(square **mapa, robot *head, int number_robots)
{
    int xm, ym;

    robot *aux1;
    square **map_update = mapa;

    for(aux1 = head; aux1 != NULL; aux1 = aux1 -> next)
    {
        xm = aux1->actual.x;
        ym = aux1->actual.y;

        mapa[xm][ym].estarobot = OCUPADO;  //da o valor 1 ao local da matriz mapa onde se encontra cada um dos robots
        mapa[xm][ym].probot = aux1;  //liga a posição em que está o robot à estrutura com a sua informação

    }

    free(aux1);

    return map_update;
}

/** adicionaRobot: adiciona um robot ao mapa
*/

void adiciona_robot(square **mapa, robot *head, int posX, int posY)
{
    SDL_Event event;

    char robot_name[DIM];
    int key = 0, bateria, i, number_robots = 1;

    robot *new_iclean = NULL;

    new_iclean = alocaRobot(number_robots);

    fprintf(stderr, "Insira o nome do robot a adicionar.\n");

    robot_name[0] = 'R';    //insere na primeira letra do nome do nomo a letra 'R'

    while(key == 0)
    {
        while( SDL_PollEvent( &event ) )
        {
            if ( event.type == SDL_KEYDOWN )  //deteta a tecla que o utilizador pressiona e insere-a no segundo caracter do nome do robot
            {
                robot_name[1] = (char)event.key.keysym.sym ;

                key = 1;
            }
        }
    }

    bateria = 50 + rand()%(50);   //faz o random da bateria do robot inserido, entre 50 e 100

    robot_name[2] = '\0';

    fprintf(stderr, "Nome do robot: %s\n", robot_name);

    strcpy(new_iclean->nome, robot_name);

    new_iclean->bateria = bateria;

    new_iclean->actual.x = posX;
    new_iclean->actual.y = posY;

    new_iclean->percorridos = 1;   //inicialização do nr. de percorridos a 1 (quadrado inicial de cada robot)

    //Inserçao no local correto da lista:

    for(i=0; i<1; i++)
    {
        //caso em que a lista está vazia
        if(head == NULL)
        {
            head = new_iclean;
            new_iclean -> next = NULL;
            continue;
        }
        //caso em que o robot tem que ser inserido antes do head da lista
        if( head->nome[1] >= robot_name[1])
        {
            new_iclean->next = head;
            head = new_iclean;
        }

        //inserir no meio/fim da lista
        else
        {
            //inicialização de dois ponteiros, um a apontar para o seguinte do outro
            robot *aux1 = head;
            robot *aux2 = head->next;

            //encontrar o local na lista onde o robot deve ser inserido
            while( aux2 != NULL && aux2->nome[1] <= robot_name[1])
            {
                aux1 = aux1->next;
                aux2 = aux2->next;
            }
            //inserir o robot na lista
            aux1->next = new_iclean;
            new_iclean->next = aux2;

        }
    }

    mapa[posX][posY].estarobot = OCUPADO;

    mapa[posX][posY].probot = new_iclean;

    strcpy(mapa[posX][posY].probot->nome, robot_name);
}

/** clean_if_zero: retira um robot se a sua bateria for igual ou menos que zero
*/
robot* clean_if_zero(square **mapa, robot *iclean, int nSquareH, int nSquareW)
{
    int posX, posY, key = 0;

    robot *aux = iclean;
    SDL_Event event;

    if (aux->next != NULL)
    {

        if( aux->bateria <= ZERO )  //caso a bateria de um robot chegue a zero
        {
            mapa[aux->target.x][aux->target.y].target = VAZIO;
            mapa[aux->actual.x][aux->actual.y].estarobot = VAZIO;
            iclean = iclean->next;

        }

        if( aux->next->bateria <= ZERO ) //caso o robots a seguir tenha ficado também sem bateria
        {
            mapa[aux->next->actual.x][aux->next->actual.y].estarobot = VAZIO;
            mapa[aux->target.x][aux->target.y].target = VAZIO;
            iclean->next = iclean->next->next;
        }
    }

    else if (aux->next == NULL)  //caso só exista um robot a funcionar
    {
        if(aux->bateria <= ZERO) //se a bateria deste chegar a zero
        {
            fprintf(stderr, "\nTodos os robots ficaram sem bateria!!");  //pergunta ao utilizador onde pretende adicionar um robot
            fprintf(stderr, "\nSelecione onde pretende inserir um novo robot (a segunda letra do seu nome deve ser posterior à do robot anterior\n");

            mapa[aux->target.x][aux->target.y].target = VAZIO;
            mapa[aux->actual.x][aux->actual.y].estarobot = VAZIO;

            while(key != 1)
            {
                while( SDL_PollEvent( &event ) )
                {
                    if ( event.type == SDL_MOUSEBUTTONDOWN )   //deteta o uso do rato
                    {
                        SDL_GetMouseState( &posX, &posY); //deteta o local onde o utilizador clica para adicionar um robot
                        posX = posX / SQUARE_SIZE;
                        posY = posY / SQUARE_SIZE;

                        if(mapa[posX][posY].estado != OBSTACULO && mapa[posX][posY].estarobot != OCUPADO)  //se a posição tiver um robot ou obstaculo, nao faz nada
                        {
                            printf("\nPosição selecionada: [%d, %d]\n", posX, posY);

                            if(posX < nSquareW && posY <nSquareH)
                            {
                                adiciona_robot(mapa, aux, posX, posY);  //adiciona um robot na zona selecionada

                            }
                        }
                        key = 1;
                    }
                }
            }

            iclean = aux;
            iclean = aux->next;
        }
    }

    return iclean;
}

/**
 *File: Generates the estatísticas.txt file with the simulation information
 */

void File(robot *head, square** mapa)             /** MUDAR ESTA FUNÇÃO*/
{
    robot *aux;

    FILE *est;
    est = fopen("estatisticas.txt","w");

    for(aux = head; aux != NULL; aux = aux->next)
    {
        fprintf(est, "\n");
        fprintf(est, "Nome do robot:  %s      ", aux->nome);
        fprintf(est, "Quadrados percorridos:  %d", aux->percorridos);
    }

    fclose(est);
}

/**
 * RenderLogo function: Renders the IST Logo on the window screen
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _renderer renderer to handle all rendering in a window
 */

int RenderLogo(int x, int y, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Surface *img_IST;
    SDL_Rect boardPos;

    // renders IST Logo
    img_IST = SDL_LoadBMP("ist_logo.bmp");
    if (img_IST == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(-5);
    }
    // square where the Logo is placed
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = img_IST->w;
    boardPos.h = img_IST->h;

    // creates a texture and renders it in the screen
    text_IST = SDL_CreateTextureFromSurface(_renderer, img_IST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy texture and surface
    SDL_DestroyTexture(text_IST);
    SDL_FreeSurface(img_IST);
    return img_IST->h;
}

/**
 * RenderText function: Renders text on the window screen
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string where the text is written
 * \param font TTF font used to render the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *font, SDL_Renderer* _renderer)
{
    SDL_Color color = { 0, 0, 0 };
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;

    // creates a surface with some text
    text_surface = TTF_RenderText_Blended(font,text,color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(-5);
    }

    // creates a texture from the surface and renders it !
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);

    // destroy texture and surface
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * RenderMap function: Renders the map on the window screen according to their size
 * \param nSquareW number of squares to render (width)
 * \param nSquareH number of squares to render (height)
 * \param pos_robot position of the robot (ID of the square)
 * \param robot_name name of the robot (max. 2 letters)
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderMap(square **mapa, robot *head, int nSquareW, int nSquareH, int pausa, SDL_Renderer* _renderer)
{
    TTF_Font *sans;
    TTF_Font *serif;
    SDL_Rect gridPos;
    int i,j, height, id = 0;
    char battery[NORMAL_DIM];
    char pause[6] = "PAUSA";

    robot *aux1 = head;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    // opens a font style and sets a size
    sans = TTF_OpenFont("FreeSans.ttf", 15);
    serif = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!sans || !serif)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(-5);
    }
    // render the IST Logo
    height = RenderLogo(nSquareW*SQUARE_SIZE, 0, _renderer);

    // render the student name
    height += RenderText(nSquareW*SQUARE_SIZE+10*MARGIN, height, myName1, serif, _renderer);
    height += RenderText(nSquareW*SQUARE_SIZE+6*MARGIN, height, myName2, serif, _renderer);
    // render the student number
    RenderText(nSquareW*SQUARE_SIZE+8*MARGIN, height, myNumber1, serif, _renderer);
    height += RenderText(nSquareW*SQUARE_SIZE+25*MARGIN, height, myNumber2, serif, _renderer)+20;


    //render list of robots:
    while(aux1 != NULL)
    {
        if(aux1->bateria > 0)
        {
            sprintf(battery, "-  %f %%", aux1->bateria);
            battery[8] = '\0';
            RenderText(nSquareW*SQUARE_SIZE+12*MARGIN, height, aux1->nome, serif, _renderer);  //imprime o nome de cada robot
            height += RenderText(nSquareW*SQUARE_SIZE+22*MARGIN, height, battery, serif, _renderer);  //imprime o nível de bateria de cada robot
        }
        aux1 = aux1 -> next;
    }

    if(pausa == 1)
    {
        height = 460;
        RenderText(nSquareW*SQUARE_SIZE+16*MARGIN, height, pause, serif, _renderer);  //imprime a palavra "pausa" quando premido p
    }

    // grid position
    gridPos.w = SQUARE_SIZE;
    gridPos.h = SQUARE_SIZE;
    gridPos.y = 0;

    // iterate over all squares
    for (i = 0; i < nSquareH; i++)
    {

        gridPos.x = 0;
        for (j = 0; j < nSquareW; j++)
        {
            if (mapa[j][i].estado == SUJO)       //pinta os quadrados sujos
            {
                SDL_SetRenderDrawColor( _renderer, 225, 181, 155, 255 );
                SDL_RenderFillRect( _renderer, &gridPos );
                SDL_SetRenderDrawColor( _renderer, 0, 0, 0, 255 );
                SDL_RenderDrawRect( _renderer, &gridPos );
            }

            if(mapa[j][i].estado == OBSTACULO)   //Desenha os obstáculos
            {
                SDL_SetRenderDrawColor( _renderer, 140, 91, 80, 255 );
                SDL_RenderFillRect( _renderer, &gridPos );
                SDL_SetRenderDrawColor( _renderer, 0, 0, 0, 255 );
                SDL_RenderDrawRect( _renderer, &gridPos );
            }

            if(mapa[j][i].estado == LIMPO)   //os quadrados limpos ficam brancos
            {
                SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );
                SDL_RenderFillRect( _renderer, &gridPos );
                SDL_SetRenderDrawColor( _renderer, 0, 0, 0, 255 );
                SDL_RenderDrawRect( _renderer, &gridPos );
            }

            if(mapa[j][i].target == DESTINO)  //pinta a posição de destino de cada robot
            {
                SDL_SetRenderDrawColor( _renderer, 250, 90, 70, 255 );
                SDL_RenderFillRect( _renderer, &gridPos );
                SDL_SetRenderDrawColor( _renderer, 0, 0, 0, 255 );
                SDL_RenderDrawRect( _renderer, &gridPos );
            }

            if(mapa[j][i].estarobot == OCUPADO)   //desenha o robot na sua posição
            {
                mapa[j][i].estado = LIMPO;
                SDL_SetRenderDrawColor( _renderer, 160, 225, 255, 255 );
                SDL_RenderFillRect( _renderer, &gridPos );
                SDL_SetRenderDrawColor( _renderer, 0, 0, 0, 255 );
                SDL_RenderDrawRect( _renderer, &gridPos );
            }

            if(mapa[j][i].estarobot == OCUPADO || mapa[j][i].target == DESTINO)  //escreve os nomes do robot nos quadrados de destino e em que o robot se encontra
            {
                RenderText(gridPos.x+MARGIN, gridPos.y, mapa[j][i].probot->nome, sans, _renderer);
            }
            gridPos.x += SQUARE_SIZE;
            id++;

        }
        gridPos.y += SQUARE_SIZE;
    }




    // render the changes above
    SDL_RenderPresent( _renderer);

    // destroy everything
    TTF_CloseFont(sans);
    TTF_CloseFont(serif);

    free(aux1);
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */

int InitEverything(int width, int height, SDL_Window** _window, SDL_Renderer** _renderer)
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if ( !InitSDL() )
        return 0;

    if ( !InitFont() )
        return 0;

    window = CreateWindow(width, height);
    if ( window == NULL )
        return 0;

    renderer = CreateRenderer(width, height, window);
    if ( renderer == NULL )
        return 0;

    *_window = window;
    *_renderer = renderer;

    return 1;
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
int InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
int InitFont()
{
    // init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window* window = NULL;
    // init window
    window = SDL_CreateWindow( "iClean Simulation", WINDOW_POSX, WINDOW_POSY, width+EXTRASPACE, height, 0 );

    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        return NULL;
    }

    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window* _window)
{
    SDL_Renderer* renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        return NULL;
    }
    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width+EXTRASPACE, height );

    return renderer;
}

