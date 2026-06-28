#include <SDL.h>
#include <SDL_ttf.h>

#include "Constants.h"
#include "GameState.h"

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_Window* window = SDL_CreateWindow(
      "Living Hell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      Constants::kWindowWidth, Constants::kWindowHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  bool restart = true;
  while (restart) {
    GameState game;
    restart = game.Run(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
