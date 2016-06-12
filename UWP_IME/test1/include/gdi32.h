#pragma once 

namespace GDI32
{
	struct FPoint: public POINT
	{
		public :
			FPoint(){ x=y=0; }		
			FPoint( long xa, long ya ){ x=xa; y=ya; }
			FPoint(LPARAM lp ){ x = LOWORD(lp); y = HIWORD(lp);}
	};
	struct FSize : public SIZE
	{
		public :
			FSize(){ cx=cy=0; }
			FSize(long cxx, long cyy ){ cx=cxx; cy=cyy; }
			FSize(LPARAM lp ){ cx = LOWORD(lp); cy = HIWORD(lp);}
	};
	struct FRect : public RECT
	{
		public :
			FRect(){left=right=top=bottom = 0;}
			FRect(long l, long t, long r, long b ){left=l;right=r;top=t;bottom=b;}
			FRect(FPoint pt, FSize sz){ left=pt.x; top=pt.y;right=pt.x+sz.cx;bottom=top+sz.cy; }

			bool PtInRect( FPoint& p ) const;
			FSize Size() const{ return FSize(right-left,bottom-top); }
			void Size( FSize& sz ) { right=left+sz.cx; bottom=top+sz.cy; }
			FPoint LeftTop() const { return FPoint(left,top); }
			FPoint RightBottom() const { return FPoint(right,bottom); }
			int Width() const { return right-left; }
			int Height() const { return bottom-top; }
			void Normalize();

			void Offset( int cx, int cy );
			void Inflate( int cx, int cy );
			FRect ZRect(){ return FRect( 0,0,right-left,bottom-top); }
	};

	void LPtoDP( HDC hdc, FRect& rc );
	void DPtoLP( HDC hdc, FRect& rc );

	struct Font
	{
		Font( int height, LPCWSTR fontnm );
		~Font();

		HFONT Select( HDC hdc );
	
		HFONT hfont;
	};

	struct Brush
	{
		Brush( int stockobject );
		Brush( COLORREF clr );
		~Brush();

		HBRUSH hBrush;
	};
	struct Region
	{
		Region( const FRect& rc );
		~Region();

		HRGN hRegion;
	};


};
