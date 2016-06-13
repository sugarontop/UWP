#pragma once

#include "d2dcontext.h"

namespace V4 {

class D2DWindow
{
	public:
		

		virtual D2DContext* cxt()=0;
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






class D2DButton : public D2DControl
{
	public :
		D2DButton(){}

		void CreateButton(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int local_id = -1);
		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);

		int mode_;

};

class D2DTextbox : public D2DControl
{
	public :
		enum TYP { SINGLELINE,MULTILINE,PASSWORD };
		D2DTextbox(D2CoreTextBridge& bridge);

		void Create(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int local_id = -1);
		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);

		

		
	protected :
		void Activate();
		void UnActivate();
		void DrawSelectArea(D2DContext& cxt);

		
	protected :
		TYP typ_;
		D2CoreTextBridge& bridge_;
		
		TextInfo ti_;
		int shift_control_key_;

};

};
