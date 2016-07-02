#pragma once

#include "d2dcontext.h"

namespace V4 {
	enum STAT { VISIBLE = 0x1, MOUSEMOVE = 0x2, CLICK = 0x4, CAPTURED = 0x8, SELECTED = 0x10, DISABLE = 0x20, READONLY = 0x40, DROPACCEPTOR = 0x80, DROPPING = 0x100, BORDER = 0x200, ALWAYSDRAW = 0x400, DEAD = 0x800, FOCUS = 0x1000, DEBUG1 = 0x2000 };

	class D2DCaptureObject;
	class D2DControl;
class D2DWindow
{
	public:
		virtual void SetCapture(D2DCaptureObject* p, FPointF* pt = NULL, D2DMat* mat = NULL) = 0;
		virtual D2DCaptureObject* ReleaseCapture() = 0;
		virtual D2DContext* cxt() = 0;
		virtual D2DCaptureObject* GetCapture() = 0;
		virtual void redraw() = 0;

		virtual void AddDeath(std::shared_ptr<D2DControl> obj) = 0;
};

class D2DCaptureObject
{
public:
	virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)=0;
	virtual void OnCaptureActive(bool bActive) {};
};

class D2DControls;

class D2DControl : public D2DCaptureObject
{
	public :
		virtual ~D2DControl(){}

		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);

		void InnerCreateWindow(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int controlid);
	protected :
		D2DMat mat_;
		FRectFBoxModel rc_;
		D2DWindow* parent_;
		D2DControls* parent_control_;
		FString name_;
		int id_;
		void* target_;
};

class D2DControls : public D2DControl
{
	friend class D2DControl;
	public :
		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);

	protected :
		std::vector<std::shared_ptr<D2DControl>> controls_;
		
};

class D2DMainWindow : public D2DWindow, public D2DControls
{
public:
	D2DMainWindow();

	virtual void SetCapture(D2DCaptureObject* p, FPointF* pt = NULL, D2DMat* mat = NULL);
	virtual D2DCaptureObject* ReleaseCapture();
	virtual D2DCaptureObject* GetCapture() { return (!capture_obj_.empty() ? capture_obj_.top() : nullptr); }
	FPointF CapturePoint(const FPointF& pt)
	{
		FPointF p = capture_pt_;
		capture_pt_ = pt;
		return p;
	}
	FRectFBoxModel CaptureRect(const FRectFBoxModel& rc)
	{
		FRectFBoxModel p = capture_rect_;
		capture_rect_ = rc;
		return p;
	}
	D2DMat CaptureMat() { return capture_matrix_; }
	virtual void redraw() { redraw_ = true; }

	virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);
	virtual DX::DeviceResources* GetResource() = 0;
	virtual void AddDeath(std::shared_ptr<D2DControl> obj);
	void AliveMetor(Windows::System::Threading::ThreadPoolTimer^ timer);
protected:
	int NomarlWndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);


protected:
	std::stack<D2DCaptureObject*> capture_obj_;
	FPointF capture_pt_;
	FRectFBoxModel capture_rect_;

	D2DMat capture_matrix_;
	D2DContext cxt_;

	typedef std::vector<D2DControl*> vector_controls;
	vector_controls view_controls_;
	std::stack<vector_controls> view_temp_;

	std::vector<std::shared_ptr<D2DControl>> pre_death_objects_;
public:
	bool redraw_;
	ColorF back_color_;

};




class D2DButton : public D2DControl
{
	public :
		D2DButton(){}

		void CreateButton(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int local_id = -1);
		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);

		int mode_;
		std::function<void(D2DButton*)> OnClick_;

};
class Caret
{
private:
	Caret();

public:
	static Caret& GetCaret();

	void Activate(TextInfo& ti);
	void UnActivate();

	void ShowCaret();
	void HideCaret();

	bool Draw(D2DContext& cxt);
	FRectF GetCaretRect();

	int bShow_;

	int is_start_change_;
private:
	TextInfo* ti_;
	Windows::System::Threading::ThreadPoolTimer^ timer_;


};
class FontInfo
{
public:
	FontInfo();

	ComPTR<IDWriteTextFormat> CreateFormat(IDWriteFactory* wfac) const;

public:
	float height;
	FString fontname;
	int weight;

	ColorF forecolor;
	ColorF backcolor;
};
class D2DTextbox : public D2DControl
{
public:
	enum TYP { SINGLELINE, MULTILINE, PASSWORD };
	D2DTextbox(D2CoreTextBridge& bridge);
	D2DTextbox(D2CoreTextBridge& bridge, TYP ty);

	void Create(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int local_id = -1);
	virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);



	void SetFont(const FontInfo& cf);
	void SetText(LPCWSTR txt);

protected:
	void Activate();
	void UnActivate();
	void DrawSelectArea(D2DContext& cxt);

protected:
	TYP typ_;
	D2CoreTextBridge& bridge_;

	TextInfo ti_;
	int shift_control_key_;
	ColorF back_, fore_;

	static bool bMouseSelectMode_;
};

};
