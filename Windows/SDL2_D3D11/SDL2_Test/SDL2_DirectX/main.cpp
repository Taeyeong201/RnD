
#include <SDL.h>

#include <string>

int main(int argc, char** argv) {
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_USE_D3D9EX, "1");
	SDL_Init(SDL_INIT_VIDEO);
	std::string choicetype = "direct3d11";

	//int driver_count = SDL_GetNumRenderDrivers();
	//int renderer_flags = SDL_RENDERER_SOFTWARE;
	//int driver_index = -1;

	//for (int i = 0; i < driver_count; i++)
	//{
	//	SDL_RendererInfo info;
	//	if (SDL_GetRenderDriverInfo(i, &info) < 0)
	//	{
	//		continue;
	//	}

	//	if (strcmp(info.name, choicetype.c_str()) == 0) {
	//		driver_index = i;
	//		if (info.flags & SDL_RENDERER_ACCELERATED)
	//		{
	//			renderer_flags =
	//				SDL_RENDERER_ACCELERATED
	//				| SDL_RENDERER_TARGETTEXTURE
	//				| SDL_RENDERER_PRESENTVSYNC;
	//		}
	//	}
	//}


	SDL_Window* w = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Renderer* r = SDL_CreateRenderer(w, -1, SDL_RENDERER_PRESENTVSYNC);
	bool running = true;
	SDL_FRect box = { 20, 20, 460, 460 };
	Uint8 colour = 0;
	while (running) {
		SDL_PumpEvents();
		SDL_Event event;
		while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, 0, SDL_LASTEVENT) == 1) {
			switch (event.type) {
			case SDL_QUIT:
				running = SDL_FALSE;
				break;
			}
		}
		SDL_SetRenderDrawColor(r, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(r);
		SDL_SetRenderDrawColor(r, colour++, 0xFF, 0xFF, 0xFF);
		SDL_RenderFillRectF(r, &box);
		SDL_RenderPresent(r);
		if (SDL_GetError()) {
			SDL_SetWindowTitle(w, SDL_GetError());
			SDL_ClearError();
		}
	}
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}