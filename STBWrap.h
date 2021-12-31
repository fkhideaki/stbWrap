#pragma once


#include <iostream>
#include <vector>
#include <functional>

#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>


class STBCol
{
public:
	stbi_uc r, g, b, a;

public:
	STBCol(void) :
		r(0), g(0), b(0), a(0)
	{
	}
	STBCol(stbi_uc r_, stbi_uc g_, stbi_uc b_, stbi_uc a_) :
		r(r_), g(g_), b(b_), a(a_)
	{
	}

	stbi_uc* v(void)
	{
		return &r;
	}
	const stbi_uc* v(void) const
	{
		return &r;
	}

	void set(stbi_uc r_, stbi_uc g_, stbi_uc b_, stbi_uc a_)
	{
		r = r_;
		g = g_;
		b = b_;
		a = a_;
	}

	void swapRB(void)
	{
		std::swap(r, b);
	}
};


class STBImage
{
public:
	std::vector<STBCol> buf;
	int w, h;

public:
	STBImage(void) :
		w(0),
		h(0)
	{
	}
	STBImage(int w_, int h_) :
		buf(w_* h_),
		w(w_),
		h(h_)
	{
	}

	virtual ~STBImage(void)
	{
		clear();
	}

	void clear(void)
	{
		buf.clear();
		w = 0;
		h = 0;
	}

	void swap(STBImage& i)
	{
		std::swap(i.w, w);
		std::swap(i.h, h);
		i.buf.swap(buf);
	}

	bool loadFile(const char* filename)
	{
		clear();

		int comp = 0;
		stbi_uc* b = stbi_load(filename, &w, &h, &comp, 4);
		if (!b)
			return false;

		int n = w * h;
		buf.resize(n);
		memcpy(buf.front().v(), b, n * 4 * sizeof(stbi_uc));

		stbi_image_free(b);

		return true;
	}

	bool savePng(const char* filename)
	{
		return stbi_write_png(filename, w, h, 4, buf.front().v(), 0) == 1;
	}

	int getIdx(int x, int y) const
	{
		return x + y * w;
	}

	STBCol& at(int x, int y)
	{
		return buf[getIdx(x, y)];
	}
	const STBCol& at(int x, int y) const
	{
		return buf[getIdx(x, y)];
	}

	void visitPx(std::function<void(STBCol&, int, int)> f)
	{
		for (int x = 0; x < w; ++x)
		{
			for (int y = 0; y < h; ++y)
			{
				f(at(x, y), x, y);
			}
		}
	}

	// wrap : wrap or clamp
	void resize(int w_, int h_, bool wrap = false)
	{
		stbir_edge repeatMode = wrap ? STBIR_EDGE_WRAP : STBIR_EDGE_CLAMP;

		STBImage r(w_, h_);
		stbir_resize(
			buf.front().v(), w, h, 0,
			r.buf.front().v(), r.w, r.h, 0,
			STBIR_TYPE_UINT8, 4,
			0, 0,
			repeatMode, repeatMode,
			STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
			STBIR_COLORSPACE_SRGB,
			nullptr);

		swap(r);
	}

	void scale(double s, bool wrap = false)
	{
		resize(w * s, h * s, wrap);
	}
};
