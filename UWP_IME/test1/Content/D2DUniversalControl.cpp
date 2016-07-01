#include "pch.h"
#include "D2DUniversalControl.h"

using namespace V4;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int D2DControl::WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	int ret = 0;

	return ret;
}

void D2DControl::InnerCreateWindow(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int controlid)
{
	_ASSERT(parent);

	parent_ = parent;
	rc_ = rc;
//	stat_ = stat;
	parent_control_ = pacontrol;
	name_ = name;
	id_ = controlid;
	target_ = nullptr; // このオブジェクトと関連付けるポインタ、通常はnull

	if (parent_control_)
	{
		parent_control_->controls_.push_back(std::shared_ptr<D2DControl>(this));
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int D2DControls::WndProc(D2DWindow* d, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	int ret = 0;
	for( auto& it : controls_ )
	{
		ret += it->WndProc(d,message,wp,lp);

	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void D2DButton::CreateButton(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int controlid)
{
	InnerCreateWindow(parent,pacontrol,rc,stat,name, controlid);
	mode_ = 0;
}

int D2DButton::WndProc(D2DWindow* d, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	int ret = 0;

	switch( message )
	{
		case WM_PAINT:
		{
			auto& cxt = *(d->cxt());

			D2DMatrix mat(cxt);
			mat_ = mat.PushTransform();
			mat.Offset(rc_.left, rc_.top);


			FRectF rc = rc_.ZeroRect();
			cxt.cxt->DrawRectangle( rc, cxt.black );

			if ( mode_ == 1 )
			{				
				FRectF rca(0,0,200,30);
				cxt.cxt->DrawText(L"pushed", 6, cxt.cxtt.textformat, rca, cxt.black );
			}


			mat.PopTransform();
		}
		break; 
		case WM_LBUTTONDOWN:
		{
			Windows::UI::Core::PointerEventArgs^ arg = (Windows::UI::Core::PointerEventArgs^)lp;
			
			FPointF pt;
			pt.x = arg->CurrentPoint->Position.X;
			pt.y = arg->CurrentPoint->Position.Y;
			
			FPointF pt3 = mat_.DPtoLP(pt);

			mode_ = 0;

			if ( rc_.PtInRect(pt3 ))
			{
				mode_ = 1;
				if (OnClick_)
					OnClick_(this);
			}

		}
		break;
		

	}
	return ret;
}
////////////////////////////////////
FontInfo::FontInfo() :forecolor(ColorF::Black), backcolor(ColorF::White)
{
	height = 12;
	fontname = L"メイリオ";
	weight = 400;
}

ComPTR<IDWriteTextFormat> FontInfo::CreateFormat(IDWriteFactory* wfac) const
{
	_ASSERT(wfac);

	ComPTR<IDWriteTextFormat> fmt;

	wfac->CreateTextFormat(
		fontname,
		nullptr,
		(DWRITE_FONT_WEIGHT) weight,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		height,
		DEFAULTLOCALE,
		&fmt
	);

	return fmt;
}