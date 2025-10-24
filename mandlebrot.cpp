#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <unordered_map>
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
	std::pair<int, int> current = {0, 0};
	std::unordered_map<long, int> escaped;

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
		if (escaped.count((long)x | ((long)y << 32)))
		{
			return escaped[(long)x | ((long)y << 32)];
		}

		using namespace std::complex_literals;
		int width = this->width;
		int height = this->height;

		double zx = (long double)x * (xb - xa) / width + xa;
		double zy = (long double)y * (yb - ya) / height + ya;

		std::complex<double> z(zx, zy);
		std::complex<double> c = z;

		for (int i = 0; i < max_iterations; i++)
		{
			z = pow(z, multibrotPower) + c;

			if (abs(z) > 2.0f)
			{
				escaped[(long)x | ((long)y << 32)] = i;
				return i;
			}
		}

		return max_iterations;
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
		target_pixel[1] = (val >> 8) & 0xff;
		target_pixel[2] = (val >> 16) & 0xff;
	}

	void RestartRender()
	{
		SDL_GetWindowSize(this->window, &width, &height);
		surface = SDL_CreateRGBSurface(0, this->width, this->height, 32, 0, 0, 0, 0);
		this->current = {0, 0};
		this->max_iterations = 1;
		escaped.clear();
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
				RestartRender();
			}
		}

		auto start = std::chrono::system_clock::now();

		int pixels_drawn = 0;
		int j;
		int i;
		for (j = this->current.second; j < this->height; j++)
		{
			for (i = this->current.first; i < this->width; i++)
			{
				int iters = escapeIterations(i, j, 2);
				int v = (iters * 255) / max_iterations;

				int pixel = 0;
				pixel |= v;	  // Blue
				pixel |= v << 8;  // Green
				pixel |= v << 16; // Red

				SetPixel(i, j, pixel);
				pixels_drawn++;

				if (pixels_drawn % 10000 == 0)
				{
					if (std::chrono::system_clock::now() > (start + std::chrono::milliseconds(16)))
						break;
				}
			}
		}

		if (i >= this->width - 1 && j >= this->height - 1)
		{
			this->max_iterations++;
		}

		this->current.first = i % this->width;
		this->current.second = j % this->height;

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
