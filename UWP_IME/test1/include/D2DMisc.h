///////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2015, admin@sugarontop.net
//
///////////////////////////////////////////////////////////////////////

#pragma once

#include <d2d1.h>

using namespace D2D1;

#pragma warning(disable: 4482)

#define ROUND(x) ((int)(x+0.5f))
#define D2RGBA(r,g,b,a) ColorF(r/255.0f, g/255.0f, b/255.0f, a/255.0f ) //  薄い(0) <- A <- 濃い(255)
#define D2RGB(r,g,b) ColorF(r/255.0f, g/255.0f, b/255.0f, 1.0f )

//
// D2DMisc.h helper library
// 2009.10.04

namespace V4 
{

class FSizeF : public D2D1_SIZE_F
{
	public :
		FSizeF()
		{
			width = 0; height=0;
		}
		
		FSizeF( float cx, float cy )
		{
			width = cx; height=cy;
		}
		FSizeF( const SIZE& sz )
		{
			width = (FLOAT)sz.cx; height=(FLOAT)sz.cy;
		}
		FSizeF( const D2D1_SIZE_U& sz )
		{
			width = (FLOAT)sz.width; height=(FLOAT)sz.height;
		}	
		FSizeF( const D2D1_SIZE_F& sz )
		{
			width = (FLOAT)sz.width; height=(FLOAT)sz.height;
		}	
#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP )
		FSizeF(const Windows::Foundation::Size& sz)
		{
			width = sz.Width;
			height = sz.Height;
		}
#endif	
		FSizeF( LPARAM lParam )
		{
			width = (FLOAT)LOWORD(lParam); height = (FLOAT)HIWORD(lParam);
		}	
		SIZE GetSIZE()
		{
			SIZE sz;
			sz.cx = ROUND(width);
			sz.cy = ROUND(height);
			return sz;
		}
		void SetSize( float w, float h )
		{
			width = w; height = h;
		}
		
};
class FPointF : public D2D1_POINT_2F
{
	public :
		FPointF()
		{
			x = y = 0;
		}
		FPointF( float x1, float y1 )
		{
			x = x1; y = y1;
		}
		FPointF( const POINT& pt )
		{
			x = (FLOAT)pt.x; y = (FLOAT)pt.y;
		}
		FPointF( const D2D1_POINT_2F& pt )
		{
			x = pt.x; y = pt.y;
		}
		FPointF( LPARAM lParam )
		{
			x = LOWORD(lParam); y = HIWORD(lParam);
		}
#if (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP )
		FPointF(Windows::Foundation::Point pt)
		{
			x = pt.X;
			y = pt.Y;
		}
		FPointF(Windows::UI::Core::ICoreWindowEventArgs^ lp)
		{
			Windows::UI::Core::PointerEventArgs^ arg = (Windows::UI::Core::PointerEventArgs^)lp;
			x = arg->CurrentPoint->Position.X;
			y = arg->CurrentPoint->Position.Y;

		}
#endif
		
		POINT GetPOINT()
		{
			POINT pt;
			pt.x = ROUND(x);
			pt.y = ROUND(y);
			return pt;
		}
		bool operator ==( const FPointF& pt )
		{
			return ( pt.x == x && pt.y== y );
		}

		FPointF& operator += ( const FSizeF& cs )
		{
			x += cs.width;
			y += cs.height;
			return *this;
		}
		FPointF& operator -= ( const FSizeF& cs )
		{
			x -= cs.width;
			y -= cs.height;
			return *this;
		}
		FPointF& operator -= ( const FPointF& p1 )
		{
			x -= p1.x;
			y -= p1.y;
			return *this;
		}
		void Offset( float offx, float offy )
		{
			x += offx;
			y += offy;
		}
};
class FRectF : public D2D1_RECT_F
{
	public :
		FRectF(){ left=top=bottom=right=0; }
		FRectF( const RECT& rc )
		{
			SetRect(rc);
		}
		FRectF( const D2D1_RECT_F& rc )
		{
			SetRect(rc);
		}

		FRectF ( const FPointF& pt, const FSizeF sz )
		{			
			left=pt.x; top=pt.y; right=pt.x + sz.width; bottom= pt.y + sz.height;
		}
		FRectF ( float l, float t, const FSizeF sz )
		{			
			left=l; top=t; right=l + sz.width; bottom= t + sz.height;
		}
		FRectF( const FPointF& p1, const FPointF& p2 )
		{
			SetRect(p1,p2);
		}
		FRectF( float _left, float _top, float _right, float _bottom )
		{
			left=_left;top=_top;right=_right;bottom=_bottom;
		}
		FRectF( const FRectF& rc )
		{
			SetRect(rc);
		}
		FRectF( float cx, float cy )
		{
			left=top=0;
			right=cx; bottom=cy;
		}
		FRectF(FRectF&& rc)
		{
			*this = std::move(rc);
		}
		FRectF& operator=(FRectF&& rc )
		{
			if ( this != &rc )
			{
				left = rc.left;
				right = rc.right;
				top = rc.top;
				bottom = rc.bottom;
			}
			return *this;
		}
		FRectF& operator=(const FRectF& rc)
		{
			if (this != &rc)
			{
				left = rc.left;
				right = rc.right;
				top = rc.top;
				bottom = rc.bottom;
			}
			return *this;
		}

		
		
		float Width()  const{ return right-left; }
		float Height()  const{ return bottom-top; }
		
		FSizeF Size() const{ return FSizeF( right-left, bottom-top ); }
		
		void SetRect( const RECT& rc )
		{
			left=(FLOAT)rc.left;
			right=(FLOAT)rc.right;
			bottom=(FLOAT)rc.bottom;
			top=(FLOAT)rc.top;		
		}
		RECT GetRECT() const
		{
			RECT rc;
			rc.left = ROUND(left);
			rc.right = ROUND(right);
			rc.top  = ROUND(top);
			rc.bottom = ROUND(bottom);
			return rc;		
		}
		void SetRect( const D2D1_RECT_F& rc )
		{
			left=rc.left;
			right=rc.right;
			bottom=rc.bottom;
			top=rc.top;	
		}
		void SetRect( const FPointF& p1, const FPointF& p2 )
		{
			left   = min( p1.x, p2.x );
			right  = max( p1.x, p2.x );
			top    = min( p1.y, p2.y );
			bottom = max( p1.y, p2.y );	
		}
		void SetRect( const FPointF& p1, const FSizeF& sz )
		{
			left   = p1.x;
			right  = left + sz.width;
			top    = p1.y;
			bottom = top + sz.height;
		}
		void SetRect( float l, float t, const FSizeF& sz )
		{
			left   = l;
			right  = left + sz.width;
			top    = t;
			bottom = top + sz.height;
		}
		void SetRect( float l,float t,float r ,float b )
		{
			left = l;
			right = r;
			top = t;
			bottom = b;

		}
		void InflateRect( float cx, float cy )
		{
			left -= cx; top -= cy;
			right += cx; bottom += cy;
		}
		void SetEmpty()
		{
			left = right = top = bottom = 0;		
		}
		FRectF& Offset( float cx, float cy )
		{
			left += cx;
			right += cx;
			top += cy;
			bottom += cy;
			return *this;
		}
		void Offset( const FSizeF& sz )
		{
			Offset(sz.width, sz.height);
		}
		
		BOOL PtInRect( const FPointF& pt ) const
		{
			if ( pt.x < left || right < pt.x ) return FALSE;
			if ( pt.y < top || bottom < pt.y ) return FALSE;			
			
			return TRUE;
		}
		bool PtInRect2( const FPointF& pt ) const
		{
			if ( pt.x < left || right < pt.x ) return false;
			if ( pt.y < top || bottom < pt.y ) return false;			
			
			return true;
		}
		bool RectInRect( const FRectF& rc ) const
		{
			return PtInRect2( rc.LeftTop() ) && PtInRect2( rc.RightBottom() );
		}

		FRectF ZeroRect() const
		{
			return FRectF( 0,0,right-left,bottom-top );
		}
		FPointF LeftTop() const
		{
			return FPointF( left,top );
		}
		FPointF LeftBottom() const
		{
			return FPointF( left,bottom );
		}
		FPointF RightTop() const
		{
			return FPointF( right,top );
		}
		FPointF RightBottom() const
		{
			return FPointF( right,bottom );
		}
		FPointF CenterPt() const
		{
			return FPointF( (left+right)/2.0f, (top+bottom)/2.0f );
		}
		void SetSize( const FSizeF& sz )
		{
			right = left + sz.width;
			bottom = top + sz.height;
		}
		void SetSize( float cx, float cy )
		{
			right = left + cx;
			bottom = top + cy;
		}
		void SetPoint( const FPointF& pt )
		{
			SetPoint(pt.x, pt.y);
		}
		void SetPoint( float x, float y )
		{
			right += (x - left);
			bottom += (y - top);

			left = x;
			top = y;
		}
		void SetHeight( float cy )
		{
			bottom = top + cy;
		}
		void SetWidth( float cx )
		{
			right = left + cx;
		}
		FSizeF GetSize() const
		{
			return FSizeF( right-left, bottom-top );
		}

		void CenterRect( const FRectF& rc )
		{	
			FPointF pt = rc.CenterPt();
			FSizeF sz = Size();

			left = pt.x - sz.width/2;
			right= pt.x + sz.width/2;
			top = pt.y - sz.height/2;
			bottom= pt.y + sz.height/2;
		}

		FRectF CenterRect() const
		{
			FPointF pt = CenterPt();
			FSizeF sz = Size();
			FRectF r;
			r.left = pt.x - sz.width/2;
			r.right= pt.x + sz.width/2;
			r.top = pt.y - sz.height/2;
			r.bottom= pt.y + sz.height/2;
			return r;

		}

		static FRectF InflateRect( const FRectF& rc, float cx, float cy )
		{
			FRectF rc1(rc);
			rc1.InflateRect( cx, cy );
			return rc1;
		}

		void MoveCenter( const FRectF& rc )
		{
			FPointF dpt = rc.CenterPt();
			FPointF spt = CenterPt();

			Offset( dpt.x-spt.x, dpt.y-spt.y );
		}
		FRectF Round()
		{
			FRectF rc;
			rc.left = (float)(int)(0.5f+left);
			rc.right = (float)(int)(0.5f+right);
			rc.top = (float)(int)(0.5f+top);
			rc.bottom = (float)(int)(0.5f+bottom);
			return rc;

		}

		bool ZeroPtInRect(const FPointF& pt ) const
		{
			// faster than "rc_.ZeroRect().PtInRect( pt ) ".

			return ( pt.x < (right-left) && pt.y < (bottom-top) && 0 < pt.x && 0 < pt.y );
		}
		void Clear()
		{
			left=0;
			right=0;
			bottom=0;
			top=0;
		}
};


// 線幅が１つに制限している以外は、HTMLのボックスモデルと同じ。
class FRectFBoxModel : public FRectF
{
	public :
		FRectFBoxModel():FRectF((float)0,(float)0,(float)0,(float)0)
		{
			BoderWidth_ = 0;			
		}
		FRectFBoxModel( float l,float t,float r,float b ):FRectF(l,t,r,b)
		{
			BoderWidth_ = 0;
		}
		FRectFBoxModel(const FRectF& rc)
		{
			BoderWidth_ = 0;
			SetFRectF(rc);
		}
		FRectFBoxModel(float l, float t, const FSizeF& sz) :FRectF(l, t, l+sz.width, t+sz.height)
		{
			BoderWidth_ = 0;
		}
		FRectFBoxModel(const FRectFBoxModel& rc)
		{
			*this = rc;
		}
		FRectFBoxModel& operator=(FRectFBoxModel&& rc )
		{
			left = rc.left;
			right = rc.right;
			top = rc.top;
			bottom = rc.bottom;
			Padding_ = rc.Padding_;
			Margin_ = rc.Margin_;
			BoderWidth_ = rc.BoderWidth_;
			
			return *this;
		}
		FRectFBoxModel& operator=(const FRectFBoxModel& rc )
		{
			if ( this != &rc )
			{
				left = rc.left;
				right = rc.right;
				top = rc.top;
				bottom = rc.bottom;
				Padding_ = rc.Padding_;
				Margin_ = rc.Margin_;
				BoderWidth_ = rc.BoderWidth_;
			}
			return *this;
		}
		void SetFRectF(const FRectF& rc )
		{
			left = rc.left;
			right = rc.right;
			top = rc.top;
			bottom = rc.bottom;


		}
		FRectFBoxModel& operator=( const FRectF& rc )
		{
			SetFRectF( rc );
			return *this;
		}

		struct dif 
		{ 
			dif(){ l=r=b=t=0.0; }
			float l,r,b,t; 
			void Set( float v ){ l=r=b=t=v; }
			void SetTB( float v ){ b=t=v; }
			void SetRL( float v ){ l=r=v; }
		};

		FRectF GetContentRect() const
		{
			FRectF rc;
			rc.left = left+Margin_.l+BoderWidth_+Padding_.l;
			rc.top = top+Margin_.t+BoderWidth_+Padding_.t;
			rc.right = right-Margin_.r-BoderWidth_-Padding_.r;
			rc.bottom = bottom-Margin_.b-BoderWidth_-Padding_.b;
			return rc;
		}
		FRectF GetPaddingRect() const
		{
			FRectF rc;
			rc.left = left+Margin_.l+BoderWidth_;
			rc.top = top+Margin_.t+BoderWidth_;
			rc.right = right-Margin_.r-BoderWidth_;
			rc.bottom = bottom-Margin_.b-BoderWidth_;
			return rc;
		}
		FRectF GetBorderRect() const
		{
			FRectF rc;
			rc.left = left+Margin_.l;
			rc.top = top+Margin_.t;
			rc.right = right-Margin_.r;
			rc.bottom = bottom-Margin_.b;
			return rc;
		}
		FRectF GetMarginRect() const
		{
			return *this;
		}

		FRectF GetContentRectZero() const
		{
			FRectF rc;
			rc.left = Margin_.l+BoderWidth_+Padding_.l;
			rc.top = Margin_.t+BoderWidth_+Padding_.t;
			rc.right = (right-left)-Margin_.r-BoderWidth_-Padding_.r;
			rc.bottom = (bottom-top)-Margin_.b-BoderWidth_-Padding_.b;
			return rc;
		}
		FRectF GetPaddingRectZero() const
		{
			FRectF rc;
			rc.left = Margin_.l+BoderWidth_;
			rc.top = Margin_.t+BoderWidth_;
			rc.right = (right-left)-Margin_.r-BoderWidth_;
			rc.bottom = (bottom-top)-Margin_.b-BoderWidth_;
			return rc;
		}
		FRectF GetBorderRectZero() const
		{
			FRectF rc;
			rc.left = Margin_.l;
			rc.top = Margin_.t;
			rc.right = (right-left)-Margin_.r;
			rc.bottom = (bottom-top)-Margin_.b;
			return rc;
		}
		FRectFBoxModel ZeroRect() const
		{
			FRectFBoxModel rc(*this);			
			rc.Offset( -left, -top );
			return rc;			
		}
		void SetContentSize( const FSizeF& sz )
		{					
			float width = Padding_.l + BoderWidth_ + Margin_.l + sz.width + Padding_.r + BoderWidth_ + Margin_.r;
			float height = Padding_.t + BoderWidth_ + Margin_.t + sz.height + Padding_.b + BoderWidth_ + Margin_.b;

						
			right = left + width;
			bottom = top + height;
		}
		
		dif Padding_;
		dif Margin_;
		float BoderWidth_;
};


};

typedef V4::FRectFBoxModel FRectFBM;