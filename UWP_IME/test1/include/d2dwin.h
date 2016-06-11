#pragma once

#ifdef _USRDLL
#define DLLEXPORT extern "C" __declspec( dllexport )
#else
#define DLLEXPORT extern "C"
#endif

#include "D2DMisc.h"


struct D2DWin
{
	HANDLE win;
};
struct D2Ctrls
{
	HANDLE ctrls;
};
struct D2Ctrl
{
	HANDLE ctrl;
};

struct DDRowInfo
{
	int row;
	bool is_selected_row;
	
	int float_row;
	
	V4::FSizeF sz_row;
	const float* col_xpos;
};
struct DDColor
{
	byte r;
	byte g;
	byte b;
	byte a;
};

struct DDImage
{
	V4::FSizeF sz;

	ComPTR<IUnknown> p;
	LPVOID LockedResource;
	DWORD Length;
	
	LPVOID parent;
};

typedef std::function<void(D2DWin)> D2DWinOnEntry;
typedef LPVOID DDContext;


DLLEXPORT D2DWin WINAPI DDMkWindow(HWND hWnd, D2DWinOnEntry entry );
DLLEXPORT void WINAPI DDResizeWindow(D2DWin win, int cx, int cy );

DLLEXPORT D2Ctrl Ctrl(D2Ctrls ctrl);

DLLEXPORT void WINAPI DDDestroyWindow(D2DWin win );

// constrols
DLLEXPORT D2Ctrls WINAPI DDMkTopControls( D2DWin win, const FRectFBM& rc, LPCWSTR name );
DLLEXPORT D2Ctrls WINAPI DDMkControls( D2Ctrls ctrls, const FRectFBM& rc, LPCWSTR classnm, LPCWSTR name);
DLLEXPORT D2Ctrls WINAPI DDMkControlsWithTitle( D2Ctrls ctrls, const FRectFBM& rc, LPCWSTR classnm, LPCWSTR name);



// TAB controls
DLLEXPORT D2Ctrls WINAPI DDGetPage( D2Ctrls ctrl, int page ); // 0 start
DLLEXPORT D2Ctrls WINAPI DDAddPage( D2Ctrls ctrl, int page ); // newpage : page=-1, insert: page > 0, 



// ----------------------------------------------------------------------------------------------------------------------
// control

DLLEXPORT D2Ctrl WINAPI DDMkButton( D2Ctrls parent, const FRectFBM& rc, LPCWSTR name );
DLLEXPORT D2Ctrl WINAPI DDMkTextbox( D2Ctrls parent, const FRectFBM& rc, int typ, LPCWSTR name );
DLLEXPORT D2Ctrl WINAPI DDMkListbox( D2Ctrls parent, const FRectFBM& rc, int typ, LPCWSTR name );
DLLEXPORT D2Ctrl WINAPI DDMkStatic(D2Ctrls parent, const FRectFBM& rc, int , LPCWSTR text);


// datagrid
DLLEXPORT D2Ctrl WINAPI DDMkDataGrid(D2Ctrls ctrls, const FRectFBM& rc, int colum_cnt, float row_height, float title_height, LPCWSTR name);
DLLEXPORT void WINAPI DDDataGridAllocbuffer(D2Ctrl ctrl, int total_rowcnt);
DLLEXPORT int  WINAPI DDDGGetSelectedIdx(D2Ctrl ctrl, int* idx);
DLLEXPORT LPCWSTR  WINAPI DDDGGetKey(D2Ctrl ctrl, int idx);
DLLEXPORT LPCWSTR  WINAPI DDDGGetValue(D2Ctrl ctrl, LPCWSTR key);





DLLEXPORT D2Ctrl WINAPI DDMkFRectFBM(D2Ctrls ctrls, const FRectFBM& rc); // デバッグ用表示だけ



DLLEXPORT void WINAPI DDDestroyControl( D2Ctrl ctrl );
DLLEXPORT D2Ctrls WINAPI DDGetParent(D2Ctrl ctrl);


DLLEXPORT void WINAPI DDShow(D2Ctrl ctrl, bool bShow);



DLLEXPORT LONG_PTR* WINAPI DDGetListBuffer(D2Ctrl ctrl, int* cnt);

// listbox

DLLEXPORT void WINAPI DDListAddItem( D2Ctrl ctrl, LPCWSTR s );
DLLEXPORT void WINAPI DDListInsertItem( D2Ctrl ctrl, int idx, LPCWSTR s );
DLLEXPORT void WINAPI DDListDelItem( D2Ctrl ctrl, int idx );
DLLEXPORT int WINAPI DDListCurrentIdx( D2Ctrl ctrl );
DLLEXPORT void WINAPI DDListClear( D2Ctrl ctrl );
DLLEXPORT int WINAPI DDListItemCount( D2Ctrl ctrl, int idx );
DLLEXPORT void WINAPI DDListSetCurrentIdx( D2Ctrl ctrl, int idx );
DLLEXPORT LONG_PTR* WINAPI DDListGetBuffer(D2Ctrl ctrl);




// Textbox
DLLEXPORT void WINAPI DDSetText( D2Ctrl ctrl, LPCWSTR text );
DLLEXPORT BSTR WINAPI DDGetText( D2Ctrl ctrl );
DLLEXPORT void WINAPI DDInsertText(D2Ctrl ctrl, int pos, LPCWSTR text);


DLLEXPORT void WINAPI DDSetParameter(D2Ctrl ctrl, const std::map<std::wstring,VARIANT>& prms );




// misc
DLLEXPORT D2Ctrl WINAPI DDImageLoadFile( D2Ctrls ctrls, const FRectFBM& rc, LPCWSTR filename );
DLLEXPORT D2Ctrl WINAPI DDMkWaiter( D2Ctrls ctrls, const FRectFBM& rc, LPCWSTR name );




// drawing function
DLLEXPORT void WINAPI DDDrawFillRect(DDContext p, float cx, float cy, const DDColor& color );
DLLEXPORT void WINAPI DDDrawStringEx(DDContext p, const V4::FRectF& rc, LPCWSTR str, int center);
DLLEXPORT void WINAPI DDDrawString(DDContext p, float offx, float offy, LPCWSTR str);
DLLEXPORT void WINAPI DDDrawWhiteString(DDContext p, float offx, float offy, LPCWSTR str);
DLLEXPORT void WINAPI DDDrawString2(DDContext p, float offx, float offy, LPCWSTR str, DDColor color);


DLLEXPORT void WINAPI DDDrawLine(DDContext p, V4::FPointF& pt1, V4::FPointF& pt2, int typ );


DLLEXPORT void WINAPI DDSetAlias(DDContext p, bool bOff );

// image -----------------------------------------------------------------------------
DLLEXPORT bool WINAPI DDLoadImage(DDContext p, DDImage& img);
DLLEXPORT void WINAPI DDDrawImage(DDContext p, const V4::FRectF& rc, DDImage& img);


// datagridを拡張したリストボックス
DLLEXPORT D2Ctrl WINAPI DDMkDGListbox(D2Ctrls parent, const FRectFBM& rc, float row_height,bool multiselect, LPCWSTR name);
DLLEXPORT void  WINAPI DDAddKeyValue(D2Ctrl parent, LPCWSTR key, LPCWSTR value );


DLLEXPORT D2Ctrl WINAPI DDMkDropdownList(D2Ctrls parent, const FRectFBM& rc, float row_height, LPCWSTR name);

// table controls-------------------------------------------------------------------------
DLLEXPORT D2Ctrls WINAPI DDMkTableControls(D2Ctrls ctrls, const FRectFBM& rc, int rowcnt, int colcnt, LPCWSTR name);
DLLEXPORT void WINAPI DDSetControlPlace(D2Ctrls ctrls, D2Ctrl ctrl, int row, int col);




/////////////////////////////////////////////////////////////////////////////////////////
class V4::FRectFBoxModel;

typedef std::function<void(D2Ctrl)> D2OnClick;

enum D2EVENT0_MODE { CLICK,DESTROY };
enum D2EVENT1_MODE { SELECTED,BIND,RESULT };
enum D2EVENT2_MODE { MOUSE_LBUTTONDOWN,MOUSE_LBUTTONUP,MOUSEMOVE,DRAW };

typedef std::function<void(D2EVENT0_MODE ev, D2Ctrl)> D2Event0Delegate;
typedef std::function<void(D2EVENT1_MODE ev, D2Ctrl, int idx)> D2Event1Delegate;
typedef std::function<void(D2EVENT2_MODE ev, D2Ctrl, float x, float y)> D2Event2Delegate;

typedef std::function<void(D2EVENT2_MODE , D2Ctrl, DDContext, DDRowInfo& row)> D2Event2DrawDelegate;


DLLEXPORT void WINAPI DDEvent0( D2EVENT0_MODE ev, D2Ctrl ctrl, D2Event0Delegate func );
DLLEXPORT void WINAPI DDEvent1( D2EVENT1_MODE ev, D2Ctrl ctrl, D2Event1Delegate func );
DLLEXPORT void WINAPI DDEvent2( D2EVENT2_MODE ev, D2Ctrl ctrl, D2Event2Delegate func );

DLLEXPORT void WINAPI DDEvent2Draw(D2EVENT2_MODE ev, D2Ctrl ctrl, D2Event2DrawDelegate func);


// listbox


DLLEXPORT void WINAPI DDDTestResourceDestroy( D2DWin w, int layer );
DLLEXPORT void DebugTrace(LPCWSTR msg, D2D1_RECT_F& rc);
DLLEXPORT bool DDOpenTextFile(LPCWSTR msg, BSTR* ret);

typedef std::function<void(int result)> D2MessageBoxDelegate;
DLLEXPORT D2Ctrl WINAPI DDMkMessageBox(D2Ctrls parent, const FRectFBM& rc, int typ, LPCWSTR text, LPCWSTR title, D2MessageBoxDelegate ret);


DLLEXPORT void WINAPI DDMove(D2Ctrl ctrl, int typ, float offx, float offy, float millisecond );
DLLEXPORT void WINAPI DDMoveResize(D2Ctrl ctrl, int typ, V4::FRectF dstrc, float millisecond);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// TR Series

/*

rowcount行、colcount列のテキストボックス入力欄を作成
DLLEXPORT D2Ctrl WINAPI DDTRCreateCellBox(D2Ctrls ctrls, const FRectFBM& rc, int rowcount, int colcount );
parameter
m["edit"] = 1;
m["color"]= xxxx;
m["cx" ] = 100;
m["cy" ] =200;



*/