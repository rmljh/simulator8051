#include "SDL.h"

class SegmentDigit
{
public:
	SegmentDigit();
	/*SegmentDigit( const RECT & rc, const int dgWide, COLORREF actColor, COLORREF inactColor, 
		COLORREF actLineColor, COLORREF inactLineColor );*/
	~SegmentDigit();

	void init(SDL_Renderer * render, const SDL_Rect &rect);
	void uninit();
	
	void draw();
	//void setDigitColor( COLORREF actColor, COLORREF inactColor );
	//void setLineColor( COLORREF actLineColor, COLORREF inactLineColor );
	//void drawDigit( HDC dc, LPCTSTR szDigit );
	//void drawDigit( HDC dc, const int iDigit );
	
	// 目前仅支持0-9
	void set_draw_mask(unsigned char mask);
	unsigned char get_draw_mask()
	{return m_mask;}
	
	void set_draw_width(int margin)
	{m_margin =margin;}

private:
	void calculate_pos(const SDL_Rect & rc);
	
	void draw_rect(const SDL_Rect & rc);
	/*
		 U
		 _
	LU	|_|	RU
	LD	|_|	RD

		 D
	*/
	// type digit elements
	enum DEType
	{
		U  = 0,
		RU = 1,
		RD = 2,
		D  = 3,
		LD = 4,
		LU = 5,
		M  = 6
	};
	
	enum SegState
	{
		ACTIVE = 0,
		INACTIVE = 1
	};

	void draw_segment(DEType seg_type, bool state);

	enum{
		SEGMENTS = 7,
		SEGMARGIN = 2,
	};
	
private:
	SDL_Renderer * m_render;
	int m_margin;
	unsigned char m_mask;
	SDL_Rect m_seg_rect[SEGMENTS];
};
