#pragma once

#ifdef _USRDLL
#define DLLEXPORT extern "C" __declspec( dllexport )
#else
#define DLLEXPORT extern "C"
#endif


interface IDWriteTextFormat;
interface IDWriteFactory;
interface IDWriteTextFormat1;
interface IDWriteFactory2;
interface ID2D1RenderTarget;
interface ID2D1Factory2;


struct D2DTextWrite
{
	IDWriteTextFormat* textformat;
	IDWriteFactory* factory;
};

namespace V4 {

DLLEXPORT void WINAPI DrawRect( ID2D1RenderTarget* cxt,const D2D1_RECT_F& rc, ID2D1Brush* br, float width );

DLLEXPORT float WINAPI DrawCenterText( ID2D1RenderTarget* cxt,D2DTextWrite& info, ID2D1Brush* clr, const D2D1_RECT_F& rc, LPCWSTR str, int len, int align );
DLLEXPORT void WINAPI DrawButton( ID2D1RenderTarget* cxt,D2DTextWrite& info, const D2D1_RECT_F& rc, LPCWSTR str, ID2D1Brush* br1 );

DLLEXPORT void WINAPI CreateVerticalGradation(ID2D1RenderTarget* target, const D2D1_SIZE_F& sz, D2D1_COLOR_F& clr1, D2D1_COLOR_F& clr2, ID2D1LinearGradientBrush** br );


struct D2DScrollbarInfo
{
	D2D1_RECT_F rc;	
	bool bVertical;
	float total_height;		// 全表示領域	
	float position;			// スクロールバーの位置

	float button_height;	// UP DOWNのボタン
	float thumb_step_c;
	bool auto_resize;
	int rowno;				// 行番号方式の場合の現在の先頭行番号

	enum STAT { NONE,ONTHUMB=0x1,ONBTN1=0x2,ONBTN2=0x4,ONSPC1=0x8,ONSPC2=0x10,CAPTURED=0x20 };
	int stat;

	D2D1_POINT_2F ptprv; // テンポラリのポインタ
	D2D1_RECT_F thumb_rc;	// 計算される
};

DLLEXPORT D2D1_RECT_F WINAPI ScrollbarRect( D2DScrollbarInfo& info, int typ );
DLLEXPORT void WINAPI DrawScrollbar( ID2D1RenderTarget* cxt, D2DScrollbarInfo& info );
DLLEXPORT void WINAPI FillArrow( ID2D1RenderTarget* cxt, const D2D1_POINT_2F& center, ID2D1Brush* br, int typ );

DLLEXPORT bool WINAPI CreatePathGeometry(ID2D1PathGeometry* pathg, const D2D1_POINT_2F* pt, int ptcount);

DLLEXPORT void WINAPI CreateTagButtomGeometry(ID2D1RenderTarget* cxt, const D2D1_RECT_F& rcbutton, const D2D1_RECT_F& rect, ID2D1PathGeometry** ret );


DLLEXPORT void DrawFill(ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc ,ID2D1Brush* br1 );

DLLEXPORT void WINAPI FillRoundRect( ID2D1RenderTarget* cxt,ID2D1Factory* factory_, const D2D1_RECT_F& rc, float radius, ID2D1Brush* br );
DLLEXPORT void WINAPI DrawFillRoundRect( ID2D1RenderTarget* cxt, ID2D1Factory* factory_, const D2D1_RECT_F& rc, float radius,ID2D1Brush* wakuclr,ID2D1Brush* fillclr );
DLLEXPORT void WINAPI DrawRoundRect( ID2D1RenderTarget* cxt,ID2D1Factory* factory_, const D2D1_RECT_F& rc, float radius, ID2D1Brush* br );
DLLEXPORT void WINAPI CreateTraiangle( ID2D1Factory* factory, int type, float cx, float cy, ID2D1PathGeometry** ret );

DLLEXPORT void WINAPI DrawResizMark( ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc );

DLLEXPORT void WINAPI DrawFillRectEx2( ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc, ID2D1Brush* wakuclr,ID2D1Brush* fillclr, float width, int md );

DLLEXPORT bool WINAPI ImageLoad( ID2D1DeviceContext* cxt, LPCWSTR filename, ID2D1Bitmap1** ret );

DLLEXPORT void WINAPI FillCircle( ID2D1RenderTarget* cxt, const D2D1_POINT_2F& center, float sz,ID2D1Brush* br );
DLLEXPORT void WINAPI DrawCircle( ID2D1RenderTarget* cxt, const D2D1_POINT_2F& center, float sz,ID2D1Brush* br );
//DLLEXPORT void WINAPI DrawCenterText( ID2D1RenderTarget* cxt,IDWriteFactory* wfactory, IDWriteTextFormat* fmt, ID2D1Brush* clr, const D2D1_RECT_F& rc, LPCWSTR str, int len, int align );


DLLEXPORT void WINAPI DrawFillRect(ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc, D2D1_COLOR_F frame, D2D1_COLOR_F fill );


//DLLEXPORT int  WINAPI FillRect(ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc ,ID2D1Brush* br ); 名前がぶつかると、エラーもでずに実行もされない
DLLEXPORT void WINAPI FillRectangle(ID2D1RenderTarget* cxt, const D2D1_RECT_F& rc, ID2D1Brush* br);


DLLEXPORT bool WINAPI CreateTextFormatL1(IDWriteFactory* wfac, LPCWSTR fontname, float height, IDWriteTextFormat** ret );
DLLEXPORT bool WINAPI CreateTextFormatL2(IDWriteFactory* wfac, LPCWSTR fontname, float height, int bold, IDWriteTextFormat** ret);

};