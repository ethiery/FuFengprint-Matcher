#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "template.h"
#include <SDL2_gfxPrimitives.h>
#include <SDL.h>

#define WINDOW_NAME "Matcher demo"
#define TO_RAD 0.024639942381096400

#define BG_COLOR 255, 255, 255, 255
#define FG_COLOR 0, 0, 0, 255
#define HL_COLOR 255, 0, 0, 255


// SDL2 handles
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

void drawMinutia(T *temp, int i, int x0, int y0, char r, char g, char b, char a)
{
  double theta = temp->o[i] * TO_RAD;
  int x1 = x0 + temp->x[i];
  int y1 = y0 + temp->y[i];
  int x2 = x1 + ceil(5 * cos(theta));
  int y2 = y1 + ceil(5 * sin(theta));
  assert(lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a) == 0);
  assert(rectangleRGBA(renderer, x1-1, y1-1, x1+1, y1+1, r, g, b, a) == 0);
}

void drawTemplate(T *temp, int x0, int y0, char r, char g, char b, char a)
{
  // Draw frame
  assert(rectangleRGBA(renderer, x0, y0, x0 + temp->width, y0 + temp->height, r, g, b, a) == 0);
  // Draw minutiae
  for (int i = 0; i < temp->nbMinutiae; i++) drawMinutia(temp, i, x0, y0, r, g, b, a);
}

void displayTemplates(T *t1, T *t2, int highlighted1, int highlighted2)
{
  assert(SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) == 0);
  assert(SDL_RenderClear(renderer) == 0);

  drawTemplate(t1, 0, 0, FG_COLOR);
  drawTemplate(t2, t1->width, 0, FG_COLOR);

  if (highlighted1 > -1)
    drawMinutia(t1, highlighted1, 0, 0, HL_COLOR);

  if (highlighted2 > -1)
    drawMinutia(t2, highlighted2, t1->width, 0, HL_COLOR);

  SDL_RenderPresent(renderer);
}


void initDisplay(T *t1, T *t2)
{
  int width = t1->width + t2->width;
  int height = (t1->height < t2->height) ? t2->height : t1->height;

  // Initialize SDL
  assert(SDL_Init(SDL_INIT_VIDEO) == 0);

  // Initialize window
  window = SDL_CreateWindow(WINDOW_NAME,
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            width, height,
                            SDL_WINDOW_SHOWN);
  assert(window != NULL);

  // Initialize renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  assert(renderer != NULL);

  displayTemplates(t1, t2, -1, -1);    
}

void destroyDisplay()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void removeMinutia(T *temp, int minutiaNo)
{  
  temp->x[minutiaNo] = temp->x[temp->nbMinutiae - 1];
  temp->y[minutiaNo] = temp->y[temp->nbMinutiae - 1];
  temp->o[minutiaNo] = temp->o[temp->nbMinutiae - 1];
  temp->t[minutiaNo] = temp->t[temp->nbMinutiae - 1];
  temp->q[minutiaNo] = temp->q[temp->nbMinutiae - 1];
  temp->nbMinutiae--;
}

void selectClosest(T *t1, T *t2, int x, int y, int *select1, int *select2)
{
  T *t = t1;
  int *select = select1;
  if (x >= t1->width)
  {
    t = t2;
    x -= t1->width;
    select = select2;
  }
  
  int d, minD = INT_MAX;
  *select = -1;
  for (int i = 0; i < t->nbMinutiae; i++)
  {
    d = (t->x[i] - x)*(t->x[i] - x) + (t->y[i] - y)*(t->y[i] - y);
    *select = (d < minD) ? i : *select;
    minD = (d < minD) ? d : minD; 
  }
}


int main(int argc, char **argv)
{
  if (argc != 3)
  {
    printf("Usage: %s templateFile1 templateFil2\n", argv[0]);
    return EXIT_FAILURE;
  }

  T temp1, temp2;
  T_load(&temp1, argv[1]);
  T_load(&temp2, argv[2]);

  SDL_Event event;
  initDisplay(&temp1, &temp2);
  atexit(destroyDisplay);

  int select1 = -1;
  int select2 = -1;

  int nbPairs = 0;
  int maxNbPairs = temp1.nbMinutiae < temp2.nbMinutiae ? temp1.nbMinutiae : temp2.nbMinutiae;
  int pairs1[maxNbPairs], pairs2[maxNbPairs];
  
  int stop = 0;

  while(!stop)
  {
    SDL_WaitEvent(&event);
    switch(event.type)
    {
      case SDL_QUIT:
        stop = 1;
        break;
      
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) 
        {
          case SDLK_ESCAPE:
            stop = 1;
            break;
          
          case SDLK_KP_ENTER:
          case SDLK_RETURN:
            if (select1 != -1 && select2 != -1)
            {
              pairs1[nbPairs] = select1;
              pairs2[nbPairs] = select2;
              printf("New pair %d %d\n", pairs1[nbPairs], pairs2[nbPairs]);
              nbPairs++;
              removeMinutia(&temp1, select1);
              removeMinutia(&temp2, select2);
              select1 = select2 = -1;
              displayTemplates(&temp1, &temp2, select1, select2);
            }
            break;
        }
        break;

      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          selectClosest(&temp1, &temp2, event.button.x, event.button.y, &select1, &select2);
          printf("Selected %d %d\n", select1, select2);
          displayTemplates(&temp1, &temp2, select1, select2);
        }
        break;
    }
  }

  T_free(&temp1);
  T_free(&temp2);

  return EXIT_SUCCESS;
}
