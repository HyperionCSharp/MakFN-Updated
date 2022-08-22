#pragma once
#pragma once
#include <string>
#include <thread>

#include <Windows.h>

#include "../lazy_importer.hpp"
#include "../skStr.h"


#include <d3d9.h>
#include "../D3DX\d3dx9.h"
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

namespace forceinline {
	class dx_renderer {
	private:
		struct D3DTLVERTEX {
			float x, y, z, rhw;
			unsigned long color;
		};

		struct Vertex
		{
			int x, y, z, ht;
			DWORD Color;
		} Va[768];

		int countt = 0;

	public:


		dx_renderer() { }
		dx_renderer(IDirect3DDevice9* device);
		~dx_renderer();

		void begin_rendering();
		void end_rendering();

		int get_fps();

		void draw_line(int x0, int y0, int x1, int y1, unsigned long color);
		void draw_rect(int x, int y, int w, int h, unsigned long color);
		void draw_filled_rect(int x, int y, int w, int h, unsigned long color);
		void draw_outlined_rect(int x, int y, int w, int h, unsigned long color);
		void draw_health_bar(int x, int y, int w, int h, float health, float max);
		void draw_text(std::wstring_view text, int x, int y, unsigned long color, bool center = true, bool outline = true);
		void draw_circle(D3DXVECTOR2 center, float radius, int sides, float width, unsigned long color);
		RECT get_text_dimensions(std::wstring_view text);

	private:
		int m_fps = 0;

		IDirect3DDevice9* m_device = nullptr;

		ID3DXFont* m_font = nullptr;
		ID3DXLine* m_line = nullptr;
	};
}