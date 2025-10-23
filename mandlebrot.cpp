#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <complex>
#include <SDL2/SDL.h>

class MandelbrotRenderer
{
public:
	SDL_Window *window;
	SDL_Surface *surface;

	double xa = -2.0;
	double xb = 2.0;
	double ya = -1.125;
	double yb = 1.125;

	int width;
	int height;

	int max_iterations = 1;
	int current = 0;

	MandelbrotRenderer()
	{
		if (SDL_Init(SDL_INIT_VIDEO))
		{
			std::cout << "Failed to init SDL\n";
			std::exit(1);
		};
		this->window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
		SDL_GetWindowSize(this->window, &width, &height);
		surface = SDL_CreateRGBSurface(0, this->width, this->height, 32, 0, 0, 0, 0);
	}

	~MandelbrotRenderer()
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	int escapeIterations(int x, int y, double multibrotPower)
	{
		using namespace std::complex_literals;
		int width = this->width;
		int height = this->height;

		// Convert x and y to mathematical
		double zx = (double)x * (xb - xa) / width + xa;
		double zy = (double)y * (yb - ya) / height + ya;

		std::complex<double> z(zx, zy);
		std::complex<double> c = z;

		for (int i = 0; i < max_iterations; i++)
		{
			z = pow(z, multibrotPower) + c;

			if (abs(z) > 2.0f)
			{
				return i;
			}
		}

		return -1;
	}

	void BlitSurface()
	{
		SDL_Surface *wsurface = SDL_GetWindowSurface(this->window);
		SDL_BlitSurface(this->surface, NULL, wsurface, NULL);
		SDL_UpdateWindowSurface(this->window);
	}

	inline void SetPixel(int x, int y, Uint32 val)
	{

		Uint8 *target_pixel = (Uint8 *)this->surface->pixels + y * this->surface->pitch + x * 4;
		target_pixel[0] = val & 0xff;
		target_pixel[1] = (val << 8) & 0xff;
		target_pixel[2] = (val << 16) & 0xff;
	}

	void Update()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				std::exit(0);
			}

			if (event.type == SDL_WINDOWEVENT)
			{
				// Reset surface and sizes
				SDL_GetWindowSize(this->window, &width, &height);
				surface = SDL_CreateRGBSurface(0, this->width, this->height, 32, 0, 0, 0, 0);
			}
		}

		for (int i = 0; i < this->width; i++)
		{
			for (int j = 0; j < this->height; j++)
			{
				int iters = escapeIterations(i, j, 2);
				SetPixel(i, j, (iters * 10));
			}
		}

		BlitSurface();
	}
};

int main()
{
	MandelbrotRenderer renderer;
	while (true)
	{
		renderer.Update();
	}

	return 0;
}
