#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define Gravidade 6.0f
#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2

typedef struct {
  float x, y;
  float dx, dy;
  short lives;
  int pontos;
  int chave;
  char *name;
  int onPlat;

  int animFrame, facingLeft, slowingDown;
} Alice;


typedef struct {
  float x, y, w, h;
} PLAT;

typedef struct {
  float x, y;
} INIMIGO;

typedef struct { //personagem
  float scrollX;

  Alice alice; //players
  PLAT plat[100];
  INIMIGO inim;

  SDL_Texture *aliceFrames[5]; //alterar pra não dar erro de seg
  SDL_Texture *plataforma;
  SDL_Texture *inimFrames[4];
  SDL_Texture *label;
  int labelW, labelH;

  TTF_Font *font;

  int time;
  int statusState;

  SDL_Renderer *renderer;
} GameState;

const int LARG = 1280; 
const int ALT = 720;

bool inicializador();
bool eventos(SDL_Window*, GameState*);
void RenderNivel(SDL_Renderer*, GameState*);
void RenderObjetos(SDL_Renderer*, GameState*);
void saida();
bool telainicial(SDL_Renderer*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*, SDL_Texture*);
void trocar(SDL_Renderer*, int);
void loadGame(GameState*);
bool nivel1 (SDL_Renderer*);
void colisaoplat(GameState*);
void colisaoinim(GameState*);
void processo(GameState*);
void init_status_lives(GameState*);
void draw_status_lives(GameState*);
void shutdown_status_lives(GameState*);


void init_status_lives(GameState* game) {
  SDL_Color white = {255, 255, 255, 255};

  /*SDL_Surface *tmp = TTF_RenderText_Blended(game->font, "Words", white);
  game->labelW = tmp->w;
  game->labelH = tmp->h;
  game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
  SDL_FreeSurface(tmp);*/

}

void draw_status_lives(GameState* game) {
  //SDL_Renderer *renderer = game->renderer;

  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);
  SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
  SDL_Rect textRect = {320, 240-game->labelH, game->labelW, game->labelH};
  SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);

}

void shutdown_status_lives(GameState* game) {
  SDL_DestroyTexture(game->label);
  game->label = NULL;
}

SDL_Texture* loadTextura (const char *path); 

SDL_Window* janela = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture* background = NULL;
SDL_Texture* jogar = NULL;
SDL_Texture* niveis = NULL;
SDL_Texture* ranking = NULL;
SDL_Texture* creditos = NULL;
SDL_Texture* sair = NULL;


int main(int argc, char *argv[]) {
  GameState game;
  bool jogando = false; 
  int nivel;

  if ( !inicializador() ) {
    printf("Falha ao inicializar!\n");
  }
  else {
    //srandom((int)time(NULL));
    game.renderer = renderer;
    if ( !telainicial(renderer, background, jogar, niveis, ranking, creditos, sair)) {
      saida();
      IMG_Quit();
      SDL_Quit();
      exit(1);
    }

    jogando = true;
    nivel = 1;
    nivel1(renderer);

}

  saida();

  return 0;
}

void telapause(SDL_Renderer *renderer) {
  int num = 0, stop;
  bool gameloop = true;
  SDL_Event event;
  SDL_Texture *telaPause = NULL;
  SDL_Texture *menuPause = NULL;

  telaPause = loadTextura("media/menu_pausa.png");
  SDL_RenderCopy(renderer, telaPause, NULL, NULL);
  SDL_RenderPresent(renderer);

  while (gameloop == true) {
  while(SDL_PollEvent(&event)) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 372 && event.motion.x < 520 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              num = 6;
              trocar(renderer, num);
              SDL_Delay(1000);
              telainicial(renderer, background, jogar, niveis, ranking, creditos, sair);
              gameloop = false;
            }
            if (event.motion.x > 565 && event.motion.x < 714 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              num = 7;
              trocar(renderer, num);
              SDL_Delay(500);
              gameloop = false;
            }
            if (event.motion.x > 780 && event.motion.x < 889 && event.motion.y > 372 && event.motion.y < 552 && event.button.button == SDL_BUTTON_LEFT) {
              num = 8;
              trocar(renderer, num);
              SDL_Delay(500);
              gameloop = false;
            }
          break;
       }
      if (event.type == SDL_QUIT) {
        exit(1);
      }
    }
  }

}

bool eventos(SDL_Window *janela, GameState *game) {
  SDL_Event event;
  bool jogando = true;
  while(SDL_PollEvent(&event)) {
    //printf ("%d\n", event.motion.x);
    switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
            if (event.motion.x > 1105 && event.motion.x < 1170 && event.motion.y > 24 && event.motion.y < 84 && event.button.button == SDL_BUTTON_LEFT) {
                telapause(renderer);
            }
    }
    if (event.type == SDL_QUIT) {
      jogando = false;
      break;
    }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case (SDLK_LALT && SDLK_F4):
          jogando = false;
          break;
        case SDLK_ESCAPE:
          telapause(renderer);
          break;
        case SDLK_SPACE:
          if (game->alice.onPlat == 1) { //só se tiver no chão
            game->alice.dy = -27;
            game->alice.onPlat = 0;
          }
          break;
        //default:
          //break;
      }
    }
  }
  
const Uint8 *state = SDL_GetKeyboardState(NULL); //arrumar
  if(state[SDL_SCANCODE_LEFT]) {
    if (game->alice.x - 6 < 0) {
      game->alice.x = 0;
    }
    else {
      game->alice.x -= 5;
    }

    game->alice.dx -= 0.5; //aceleração

    if(game->alice.dx < -5) {
      game->alice.dx = -5;
    }
    game->alice.facingLeft = 1;
    game->alice.slowingDown = 0;
  }

  else if(state[SDL_SCANCODE_RIGHT]) {
    if ((game->alice.x + 68 + 6) > 2560) {
      game->alice.x = 2560 - 68;
    }
    else {
      game->alice.x += 10;
    }

    game->alice.dx += 0.5; //aceleração
    if(game->alice.dx > 5) {
      game->alice.dx = 5;
    }
    game->alice.facingLeft = 0;
    game->alice.slowingDown = 0;
  }

  else {
    game->alice.animFrame = 0;
    game->alice.dx *= 0.8f;
    game->alice.slowingDown = 1;
    if (fabsf(game->alice.dx) < 0.1f) {
      game->alice.dx = 0;
    }
  }

  return jogando;
}


bool inicializador () {
  bool sucesso = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) { //-1 dá problema
    printf("O programa não pode ser inicializado! SDL_ERROR: %s\n", SDL_GetError());
    sucesso = false;
  }
  else {
    srandom((int)time(NULL));
    janela = SDL_CreateWindow("7 letters from my love", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARG, ALT, SDL_WINDOW_SHOWN);
    if (janela == NULL) {
      printf("A janela não pode ser criada! SDL_ERROR: %s\n", SDL_GetError());
      sucesso = false;
    }
    else {
      renderer = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (renderer == NULL) {
        printf("O renderer não pode ser criado! SDL Error: %s\n", SDL_GetError() );
        sucesso = false;
      }
      else {
        TTF_Init();
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int imgFlags = IMG_INIT_PNG;
        if ( !(IMG_Init(imgFlags) & imgFlags) ) {
          printf( "SDL_image não pode inicializar! SDL_image Error: %s\n", IMG_GetError() );
          sucesso = false;
        }
      }
    }
  }

  return sucesso;
}

void saida () {

  SDL_DestroyWindow(janela);
  SDL_DestroyRenderer(renderer);

  janela = NULL;
  renderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

}


bool telainicial (SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *jogar, SDL_Texture *niveis, SDL_Texture *ranking, SDL_Texture *creditos, SDL_Texture *sair) {
  bool sucesso = true;
  int num = 0;
  bool apertou = false;
  SDL_Texture *imgNiveis = NULL;
  SDL_Texture *imgCreditos = NULL;
  SDL_Texture *imgRanking = NULL;
  SDL_Texture *imgAjuda = NULL;
  SDL_Texture *retornar = NULL;
  SDL_Texture *ajuda = NULL;

    background = loadTextura("media/menu.png");
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);

    jogar = loadTextura("media/jogar1.png");
    SDL_Rect jogarRect = {378, 397, 124, 50};
    SDL_RenderCopy(renderer, jogar, NULL, &jogarRect);

    niveis = loadTextura("media/niveis1.png");
    SDL_Rect niveisRect = {385, 490, 146, 53};
    SDL_RenderCopy(renderer, niveis, NULL, &niveisRect);

    creditos = loadTextura("media/creditos1.png");
    SDL_Rect credRect = {360, 587, 196, 57};
    SDL_RenderCopy(renderer, creditos, NULL, &credRect);

    ranking = loadTextura("media/ranking1.png");
    SDL_Rect ranRect = {740, 392, 196, 57};
    SDL_RenderCopy(renderer, ranking, NULL, &ranRect);

    sair = loadTextura("media/sair1.png");
    SDL_Rect sairRect = {780, 490, 98, 41};
    SDL_RenderCopy(renderer, sair, NULL, &sairRect);

    ajuda = loadTextura("media/interrogacao.png");
    SDL_Rect ajudaRect = {1150, 600, 100, 98};
    SDL_RenderCopy(renderer, ajuda, NULL, &ajudaRect);

    retornar = loadTextura("media/retornar1.png");
    SDL_Rect retRect = {30, 600, 100, 100};

    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool gameloop = true;
  while (gameloop == true) {
    while ( SDL_PollEvent (&event) ) {
      if (event.type == SDL_QUIT) {
        gameloop = false;
        sucesso = false;
        exit(1);
        break;
      }
      else {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == SDL_BUTTON_LEFT) {
            apertou = true;
          }
            if (apertou == true && event.motion.x > 287 && event.motion.x < 600 && event.motion.y > 382 && event.motion.y < 460 && event.button.button == SDL_BUTTON_LEFT) {
                num = 1;
                trocar(renderer, num);
                SDL_Delay(300);
                gameloop = false;
                apertou = false;
            }
            else if (apertou == true && event.motion.x > 300 && event.motion.x < 610 && event.motion.y > 484 && event.motion.y < 546 && event.button.button == SDL_BUTTON_LEFT) {
              num = 2;
              trocar(renderer, num);
              SDL_Delay(300);
              imgNiveis = loadTextura("media/menu_niveis.png"); //
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgNiveis, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false;
              //gameloop = false;
            }

            else if (apertou == true && event.motion.x > 304 && event.motion.x < 613 && event.motion.y > 571 && event.motion.y < 660 && event.button.button == SDL_BUTTON_LEFT) {
              num = 3;
              trocar(renderer, num);
              SDL_Delay(300);
              imgCreditos = loadTextura("media/creditos_teste.png"); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgCreditos, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false;
              //gameloop = false;
            }

            else if (apertou == true && event.motion.x > 686 && event.motion.x < 1000 && event.motion.y > 380 && event.motion.y < 459 && event.button.button == SDL_BUTTON_LEFT) {
              num = 4;
              trocar(renderer, num);
              SDL_Delay(300);
              imgRanking = loadTextura("media/ranking.png"); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgRanking, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false; 
              //gameloop = false;
            }

            else if (apertou == true && event.motion.x > 676 && event.motion.x < 992 && event.motion.y > 475 && event.motion.y < 537 && event.button.button == SDL_BUTTON_LEFT) {
              num = 5;
              trocar(renderer, num);
              SDL_Delay(300);               
              gameloop = false;
              sucesso = false;
              apertou = false; 
              exit(1);
            }

            else if (apertou == true && event.motion.x > 1158 && event.motion.x < 1245 && event.motion.y > 611 && event.motion.y < 695 && event.button.button == SDL_BUTTON_LEFT) {
              SDL_Delay(300);
              imgAjuda = loadTextura("media/ranking.png"); 
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, imgAjuda, NULL, NULL);
              SDL_RenderCopy(renderer, retornar, NULL, &retRect);
              SDL_RenderPresent(renderer);
              apertou = false; 
            }

            else if(apertou == true && event.motion.x > 40 && event.motion.x < 120 && event.motion.y > 608 && event.motion.y < 694 && event.button.button == SDL_BUTTON_LEFT) { //botao retornar
              gameloop = false;
              telainicial(renderer, background, jogar, niveis, ranking, creditos, sair);
              apertou = false; 
            }

            break;
        }
      }
    }
  }

    SDL_Delay(300);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(jogar);
    SDL_DestroyTexture(niveis);
    SDL_DestroyTexture(ranking);
    SDL_DestroyTexture(creditos);
    SDL_DestroyTexture(sair);
    SDL_DestroyTexture(ajuda);
    SDL_DestroyTexture(imgNiveis);
    SDL_DestroyTexture(imgCreditos);
    SDL_DestroyTexture(imgRanking);
    SDL_DestroyTexture(imgAjuda);
    SDL_DestroyTexture(retornar);
    SDL_RenderClear(renderer);

  return sucesso;

}


SDL_Texture* loadTextura(const char *path) {
  SDL_Surface* loadedSurface = IMG_Load(path);
  SDL_Texture* textura = NULL;

  if(loadedSurface == NULL) {
    printf( "Não foi possível carregar a imagem %s! SDL Error: %s\n", path, IMG_GetError() );
  }
  else {
    textura = SDL_CreateTextureFromSurface (renderer, loadedSurface);
    if (textura == NULL) {
      printf( "Não foi possível carregar a textura %s! SDL Error: %s\n", path, IMG_GetError() );
    }
  }

  SDL_FreeSurface(loadedSurface);

  return textura;

}

void RenderNivel(SDL_Renderer *renderer, GameState *game) {
  SDL_Color preto = {0, 0, 0, 0};
  SDL_Texture *porta = NULL;
  SDL_Texture *chave = NULL;
  SDL_Texture *fundo = NULL;
  SDL_Texture *barra = NULL;
  SDL_Texture *vida = NULL;
  SDL_Texture *placa = NULL;
  SDL_Texture *pause = NULL;
  SDL_Texture *Nivel1 = NULL;

    if (game->statusState == STATUS_STATE_LIVES) {
      draw_status_lives(game);
    }
    else if (game->statusState == STATUS_STATE_GAME) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    fundo = loadTextura("media/mapa.png");
    SDL_Rect fundoRect = {game->scrollX + 0, 0, 2560+155, 720};
    SDL_RenderCopy(renderer, fundo, NULL, &fundoRect);

    porta = loadTextura("media/porta.png");
    SDL_Rect portRect = { game->scrollX + 2432, 67, 113, 133};
    SDL_RenderCopy(renderer, porta, NULL, &portRect);

    barra = loadTextura("media/base_barra.png");
    SDL_Rect barraRect = {0, 0, 1280, 109};
    SDL_RenderCopy(renderer, barra, NULL, &barraRect);

    //caso queira inserir algum texto
    char str[128] = "";
    sprintf (str, "%d", (int)game->alice.pontos);
    SDL_Surface *tmp = TTF_RenderText_Blended(game->font, str, preto); 
    game->labelW = tmp->w;
    game->labelH = tmp->h;
    game->label = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect textRect = {965, 60-game->labelH, game->labelW, game->labelH};
    SDL_RenderCopy(renderer, game->label, NULL, &textRect);
    //game->alice.pontos++; 

    Nivel1 = loadTextura("media/nivel1.png");
    SDL_Rect nivel1Rect = {560, 35, 153, 41};
    SDL_RenderCopy(renderer, Nivel1, NULL, &nivel1Rect);

    //if (alice.vida == 1)
    vida = loadTextura("media/coracao_vida.png");
    SDL_Rect vidaRect = {180, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vidaRect);

    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { game->scrollX + 120, 570, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);

    chave = loadTextura("media/chave.png");
    SDL_Rect chaveRect = { game->scrollX + 39, 325, 50, 25};
    SDL_RenderCopy(renderer, chave, NULL, &chaveRect);

    pause = loadTextura("media/botao_pausa.png");
    SDL_Rect pauseRect = { 1100, 15, 74, 75 };
    SDL_RenderCopy(renderer, pause, NULL, &pauseRect);

    if ()

    //if (alice.vida == 2)
    //SDL_RenderCopy(renderer, vida, NULL, &vidaRect);
    SDL_Rect vida2Rect = {245, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida2Rect);

    //if (alice.vida == 3) 
    SDL_Rect vida3Rect = {310, 10, 49, 39};
    SDL_RenderCopy(renderer, vida, NULL, &vida3Rect);


    SDL_DestroyTexture(fundo);
    SDL_DestroyTexture(barra);
    SDL_DestroyTexture(vida);
    SDL_DestroyTexture(placa);
    SDL_DestroyTexture(pause);
    SDL_DestroyTexture(Nivel1);

  }
    
}

void RenderObjetos(SDL_Renderer *renderer, GameState *game) {
  int i;
  if (game->statusState == STATUS_STATE_LIVES) {
    draw_status_lives(game);
  }
  else if (game->statusState == STATUS_STATE_GAME) {
    SDL_Texture *placa = NULL;

    placa = loadTextura("media/signRight.png");
    SDL_Rect placRect = { game->scrollX + 2500, 570, 70, 100 };
    SDL_RenderCopy(renderer, placa, NULL, &placRect);

  for (i = 0; i < 51; i++) { //quantidades de plataformas que existirão
    SDL_Rect platRect = { game->scrollX + game->plat[i].x, game->plat[i].y, 128, 30 };
    SDL_RenderCopy(renderer, game->plataforma, NULL, &platRect);
  }

  //inimigo arrumar
  SDL_Rect inimRect = { game->scrollX + game->inim.x, game->inim.y, 50, 28 };
  SDL_RenderCopyEx(renderer, game->inimFrames[0], NULL, &inimRect, 0, NULL, 0);

  //Alice
  SDL_Rect rect = { game->scrollX + game->alice.x, game->alice.y, 68, 118 };
  SDL_RenderCopyEx(renderer, game->aliceFrames[game->alice.animFrame], NULL, &rect, 0, NULL, (game->alice.facingLeft == 1));
  }

}


void loadGame(GameState *game) {
  int i;
  //posição da alice;
  game->label = NULL;

  game->alice.x = 30;
  game->alice.y = 500;
  game->alice.dx = 0;
  game->alice.dy = 0;
  game->alice.onPlat = 0;
  game->alice.animFrame = 0;
  game->alice.facingLeft = 0; //0 pq o boneco tá virado pra direita
  game->alice.slowingDown = 0;
  game->alice.lives = 3;
  game->alice.pontos = 0;
  game->statusState = STATUS_STATE_LIVES;

  init_status_lives(game);

  game->time = 0;
  game->scrollX = 0;

  //posição das plataformas 
  for (i = 0; i < 20; i++) { //do chão
    game->plat[i].x = 0+(128*i);
    game->plat[i].y = 675;
  }

  game->plat[20].x = 0;
  game->plat[20].y = 500;

  game->plat[21].x = 128;
  game->plat[21].y = 500;

  game->plat[22].x = 256;
  game->plat[22].y = 500;

  game->plat[23].x = 470;
  game->plat[23].y = 600;

  game->plat[24].x = 520;
  game->plat[24].y = 450;

  game->plat[25].x = 648;
  game->plat[25].y = 450;

  game->plat[26].x = 620;
  game->plat[26].y = 520;

  game->plat[27].x = 748;
  game->plat[27].y = 520;

  game->plat[28].x = 876;
  game->plat[28].y = 520;

  // Plataformas saída

  game->plat[29].x = 2304;
  game->plat[29].y = 200;

  game->plat[30].x = 2432;
  game->plat[30].y = 200;

  //-----------------------

  game->plat[31].x = 256;
  game->plat[31].y = 300;

  game->plat[32].x = 384;
  game->plat[32].y = 300;

  game->plat[33].x = 512;
  game->plat[33].y = 300;

  game->plat[34].x = 0;
  game->plat[34].y = 400;

  game->plat[35].x = 128;
  game->plat[35].y = 400;

  game->plat[36].x = 648;
  game->plat[36].y = 250;

  game->plat[37].x = 776;
  game->plat[37].y = 250;

  game->plat[38].x = 1032;
  game->plat[38].y = 450;

  game->plat[39].x = 1160;
  game->plat[39].y = 450;

  game->plat[40].x = 1288;
  game->plat[40].y = 450;

  game->plat[41].x = 1416;
  game->plat[41].y = 450;

  game->plat[42].x = 950;
  game->plat[42].y = 220;

  game->plat[43].x = 1078;
  game->plat[43].y = 220;

  game->plat[44].x = 1226;
  game->plat[44].y = 290;

  game->plat[45].x = 1566;
  game->plat[45].y = 350;

  game->plat[46].x = 1694;
  game->plat[46].y = 350;

  game->plat[47].x = 1822;
  game->plat[47].y = 350;

  game->plat[48].x = 1744;
  game->plat[48].y = 220;

  game->plat[49].x = 1872;
  game->plat[49].y = 220;

  game->plat[50].x = 2000;
  game->plat[50].y = 220;


  game->inim.x = 700;
  game->inim.y = 644;
  //game->inim.w = 50;
  //game->inim.h = 28;


  /*posição aleatoria de stars, caso queira inserir algo aleatório
  for (i = 0; i < 5; i++) {
    game->stars[i].x = random()%LARG;
    game->stars[i].y = random()%ALT;
  }*/

}


void trocar (SDL_Renderer *renderer, int num) {

  if (num == 1) {
    SDL_Texture *jogar2 = NULL;
    SDL_Rect jogarRect = {378, 397, 124, 50};
    jogar2 = loadTextura("media/jogar3.png");
    SDL_RenderCopy(renderer, jogar2, NULL, &jogarRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(jogar2);
  }

  if (num == 2) {
    SDL_Texture *niveis2 = NULL;
    SDL_Rect niveisRect = {385, 490, 146, 53};
    niveis2 = loadTextura("media/niveis3.png");
    SDL_RenderCopy(renderer, niveis2, NULL, &niveisRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(niveis2);
  }

   if (num == 3) {
    SDL_Texture *cred2 = NULL;
    SDL_Rect credRect = {360, 587, 196, 57};
    cred2 = loadTextura("media/creditos3.png");
    SDL_RenderCopy(renderer, cred2, NULL, &credRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(cred2);
  }

  if (num == 4) {
    SDL_Texture *ranking2 = NULL;
    SDL_Rect ranRect = {740, 392, 196, 57};
    ranking2 = loadTextura("media/ranking3.png");
    SDL_RenderCopy(renderer, ranking2, NULL, &ranRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(ranking2);
  }

  if (num == 5) {
    SDL_Texture *sair2 = NULL;
    SDL_Rect sairRect = {780, 490, 98, 41};
    sair2 = loadTextura("media/sair3.png");
    SDL_RenderCopy(renderer, sair2, NULL, &sairRect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(sair2);
  }

  if (num == 6) {
    SDL_Texture *menu2 = NULL;
    SDL_Rect menu2Rect = {356, 365, 175, 200};
    menu2 = loadTextura("media/menu3.png");
    SDL_RenderCopy(renderer, menu2, NULL, &menu2Rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(menu2);
  }

    if (num == 7) {
    SDL_Texture *cont2 = NULL;
    SDL_Rect cont2Rect = {552, 364, 175, 200};
    cont2 = loadTextura("media/continuar3.png");
    SDL_RenderCopy(renderer, cont2, NULL, &cont2Rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(cont2);
  }

    if (num == 8) {
    SDL_Texture *pause2 = NULL;
    SDL_Rect pause2Rect = {746, 364, 175, 200};
    pause2 = loadTextura("media/opcoes_pausa3.png");
    SDL_RenderCopy(renderer, pause2, NULL, &pause2Rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(pause2);
  }

}



bool nivel1(SDL_Renderer *renderer) {
  bool sucesso = true;
  bool jogando = true;
  GameState game;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  //tempo para carregar o jogo
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  SDL_Delay(100);

  loadGame(&game);
  game.font = TTF_OpenFont("media/TravelingTypewriter.ttf", 30);
  game.plataforma = loadTextura("media/plataforma.png");
  game.aliceFrames[0] = loadTextura("media/alice1.png");
  game.aliceFrames[1] = loadTextura("media/alice2.png");
  game.inimFrames[0] = loadTextura("media/slimeWalk1.png");
  game.inimFrames[1] = loadTextura("media/slimeWalk2.png");

    while(jogando != false) {
      SDL_RenderClear(renderer);
      RenderNivel(renderer, &game);
      RenderObjetos(renderer, &game);
      jogando = eventos(janela, &game);
      processo(&game);
      colisaoplat(&game);
      SDL_RenderPresent(renderer);
      SDL_Delay(1000/60);

    }

    SDL_Delay(500);

  SDL_DestroyTexture(game.plataforma);
  SDL_DestroyTexture(game.aliceFrames[0]);
  SDL_DestroyTexture(game.aliceFrames[1]); 
  SDL_DestroyTexture(game.inimFrames[0]);
  SDL_DestroyTexture(game.inimFrames[1]); 
  if(game.label != NULL) {
    SDL_DestroyTexture(game.label);
  }
  TTF_CloseFont(game.font);


    return sucesso;
    
}

void processo(GameState *game) {
  //add time
  game->time++;

  if(game->time > 120) {
    shutdown_status_lives(game);
    game->statusState = STATUS_STATE_GAME;
  }

  if (game->statusState == STATUS_STATE_GAME) {
  //movimento da Alice
  Alice *alice = &game->alice;
  alice->x += alice->dx;
  alice->y += alice->dy;

  alice->dy += Gravidade;

  if (alice->dx != 0 && alice->onPlat && !alice->slowingDown) {
    if (game->time % 5 == 0) {
      if (alice->animFrame == 0) {
        alice->animFrame = 1;
      }
      else {
        alice->animFrame = 0;
      }
    }
  }

  //movimento do inimigo
  int velX = 3;
  INIMIGO *inim = &game->inim;

  inim->x -= velX;
  if (inim->x + 3 < 0) {  //vai só pra esquerda
    inim->x = LARG + 3;
  }

  game->scrollX = -game->alice.x + 598;
  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -2580 + 598) {
    game->scrollX = -2580 + 598;
  }

  }

 //vai só pra esquerda
  /*if (i == 0) {
  if (inim->x - 3 < 0) {
      inim->x = 0;
      i = 1;
  }
  else {
      inim->x -= velX;
  }
}
  if (i == 1) {
    if (inim->x + 50 + 3 > LARG) {
      inim->x = LARG - 50;
      i = 0;
    }
    else {
      inim->x -= velX;
    }
  }*/


}


void colisaoplat(GameState *game) {
  int i;

  for (i = 0; i < 100; i++) {
  float aw = 68, ah = 112; //largura e altura -3 pra ficar no chao da alice;
  float ax = game->alice.x, ay = game->alice.y; //posição da alice

  float pw = 128, ph = 29; //largura e altura da plat;
  float px = game->plat[i].x, py = game->plat[i].y; //posição da plat;

  if (px < ax+aw/2 && ax+aw/2 < px+pw) {
      //are we bumping our head?
    if (ay < py+ph && ay > py && game->alice.dy < 0) {
        //correct y
        game->alice.y = py + ph;
        ay = py + ph; 
        //bumped our head, stop any jump velocity
        game->alice.dy = 0;
        game->alice.onPlat = 1;
    }
  }

  if (px < ax+aw && ax < px+pw) {
    if (ay+ah > py && ay < py && game->alice.dy > 0) {
      //correct y
      game->alice.y = py - ah;
      ay = py - ah;
      //landed on this ledge, stop any jump velocity
      game->alice.dy = 0;
      game->alice.onPlat = 1;
    }
  }

      if(ay+ah > py && ay<py+ph) {
      //rubbing against right edge
      if(ax < px+pw && ax+aw > px+pw && game->alice.dx < 0) {
        //correct x
        game->alice.x = px+pw;
        ax = pw+pw;

        game->alice.dx = 0;
      }
      //rubbing against left edge
      else if(ax+aw > px && ax < px && game->alice.dx > 0)
      {
        //correct x
        game->alice.x = px-aw;
        ax = px-aw;
        
        game->alice.dx = 0;
      }
    }
  }
}

int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
  return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}
