#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>

#define WIN_H 384
#define WIN_W 384

#define CELL_SIZE 16

typedef struct node
{
	struct node* next;
	struct node* prev;
	SDL_Rect block;
	int prev_x;
	int prev_y;
}node_t;

void draw_snake(node_t* head, SDL_Renderer* renderer)
{
	node_t* current;

	SDL_SetRenderDrawColor(renderer, 0, 120, 0, 255);
	SDL_RenderFillRect(renderer, &(head->block));

	for(current = head->prev; current != NULL; current = current->prev)
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &(current->block));
	}
}

void move(node_t* head, int* run, int dir_x, int dir_y)
{
	if( head->block.x > WIN_W-head->block.w ||
		head->block.x < 0 ||
		head->block.y > WIN_H-head->block.h ||
		head->block.y < 0 )
	{
		*run = 0;
	}

	node_t* current;

	head->prev_x = head->block.x;
	head->prev_y = head->block.y;
	head->block.x += dir_x;
	head->block.y += dir_y;

	for(current = head; current != NULL; current = current->prev)
	{
		if(current->prev != NULL)
		{
			current->prev->prev_x = current->prev->block.x;
			current->prev->prev_y = current->prev->block.y;
			current->prev->block.x = current->prev_x;
			current->prev->block.y = current->prev_y;

			if( head->block.x == current->prev->block.x &&
				head->block.y == current->prev->block.y )
			{
				*run = 0;
			}
		}
	}
}

void increase_snake(node_t* head)
{
	node_t* current;
	for(current = head; current->prev != NULL; current = current->prev);

	current->prev = malloc(sizeof(node_t));

	current->prev->block.h = current->block.h;
	current->prev->block.w = current->block.w;

	current->prev->block.x = current->prev_x;
	current->prev->block.y = current->prev_y;

	current->prev->prev_x = current->prev_x;
	current->prev->prev_y = current->prev_y;
}

void food_recalc(int* x, int* y, node_t* head)
{
	int nx, ny;
	node_t* current;
	int flag = 1;

	while(flag)
	{
		nx = 1.0+(int)(23.0*rand()/(RAND_MAX+1.0));
		ny = 1.0+(int)(23.0*rand()/(RAND_MAX+1.0));

		*x = nx * CELL_SIZE;
		*y = ny * CELL_SIZE;

		for(current = head; current != NULL; current = current->prev)
		{
			if(*x == current->block.x && *y == current->block.y)
			{
				break;
			}
			else if(current->prev == NULL)
			{
				flag = 0;
				break;
			}
		}
	}
}


int main()
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event e;
	int run = 1;
	
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow
		(
		 "Mini snake",
		 SDL_WINDOWPOS_UNDEFINED,
		 SDL_WINDOWPOS_UNDEFINED,
		 WIN_W,
		 WIN_H,
		 SDL_WINDOW_SHOWN
		);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


	node_t* head = malloc(sizeof(node_t));
	head->next = NULL;
	head->prev = NULL;
	head->block.x = 0;
	head->block.y = 0;
	head->block.h = CELL_SIZE;
	head->block.w = CELL_SIZE;
	head->prev_x = 0;
	head->prev_y = 0;

	node_t* tail = head;

	int dir_y = CELL_SIZE;
	int dir_x = 0;

	srand(time(NULL));

	SDL_Rect food;
	food_recalc(&food.x, &food.y, head);
	food.h = CELL_SIZE;
	food.w = CELL_SIZE;

	int SCREEN_FPS = 2; 
	uint32_t SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

	int countedFrames = 0;
	float avgFPS = 0;
	uint32_t time_tmp = 0;
	uint32_t startedTime = 0;
	startedTime = SDL_GetTicks();

	uint32_t timeCap = 0;
	uint32_t startedTimeCap = 0;


	int score = 0;
	while(run)
	{
		startedTimeCap = SDL_GetTicks();

		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
			{
				run = 0;
			}

			else if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_UP:
					case SDLK_w:
						if(dir_y != CELL_SIZE)
						{
							dir_y = -CELL_SIZE;
							dir_x = 0;
						}
						break;

					case SDLK_DOWN:
					case SDLK_s:
						if(dir_y != -CELL_SIZE)
						{
							dir_y = CELL_SIZE;
							dir_x = 0;
						}
						break;

					case SDLK_LEFT:
					case SDLK_a:
						if(dir_x != CELL_SIZE)
						{
							dir_x = -CELL_SIZE;
							dir_y = 0;
						}
						break;

					case SDLK_RIGHT:
					case SDLK_d:
						if(dir_x != -CELL_SIZE)
						{
							dir_x = CELL_SIZE;
							dir_y = 0;
						}
						break;
				}
			}
		}

		move(head, &run, dir_x, dir_y);

		if(head->block.x == food.x && head->block.y == food.y)
		{
			score++;
			increase_snake(tail);
			food_recalc(&food.x, &food.y, head);
			SCREEN_FPS++;
			SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
		}
		
		time_tmp = SDL_GetTicks() - startedTime;
		avgFPS = countedFrames / (time_tmp / 1000.f);
		if(avgFPS > 2000000)
			avgFPS = 0;
		/* printf("%.f\n", avgFPS); */

		SDL_RenderClear(renderer);


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &food);

		draw_snake(head, renderer);

		SDL_SetRenderDrawColor(renderer, 215, 255, 245, 255);
		SDL_RenderCopy(renderer, NULL, NULL, NULL);
		SDL_RenderPresent(renderer);
		countedFrames++;

		timeCap = SDL_GetTicks() - startedTimeCap;
		if(timeCap < SCREEN_TICKS_PER_FRAME)
		{
			SDL_Delay(SCREEN_TICKS_PER_FRAME - timeCap);
		}
	}

	printf("Score: %d\n", score);
	return 0;
}
