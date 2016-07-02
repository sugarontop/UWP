#include "pch.h"
#include "D2DUniversalControl.h"
#include "comptr.h"

using namespace Windows::System::Threading;

namespace V4
{

	void Blink(Windows::System::Threading::ThreadPoolTimer^ timer)
	{
		int a = Caret::GetCaret().bShow_;

		if (a == 1)
			a = -1;
		else if (a == -1)
			a = 1;
		else if (a == 0)
			timer->Cancel();

		Caret::GetCaret().bShow_ = a;


	}

	Caret::Caret()
	{
		ti_ = nullptr;
		bShow_ = 0;
		is_start_change_ = 0;

	}


	//static 
	Caret& Caret::GetCaret()
	{
		static Caret ca;


		return ca;
	}

	void Caret::Activate(TextInfo& ti)
	{
		ti_ = &ti;
		bShow_ = 1;
		is_start_change_ = false;

		Windows::Foundation::TimeSpan tm;
		tm.Duration = 10 * 1000 * 500;

		if (timer_ == nullptr)
			timer_ = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(Blink), tm);





	}
	void Caret::UnActivate()
	{
		ti_ = false;
		bShow_ = 0;

		if (timer_)
			timer_->Cancel();
		is_start_change_ = 0;
		timer_ = nullptr;
	}



	void Caret::ShowCaret()
	{
		bShow_ = 1;
	}
	void Caret::HideCaret()
	{
		bShow_ = 0;
	}

	bool Caret::Draw(D2DContext& cxt)
	{
		if (bShow_ == 0 || ti_ == nullptr) return false;
		if (bShow_ == -1) return true;

		auto rc = GetCaretRect();

		cxt.cxt->FillRectangle(rc, cxt.black);


		return true;
	}
	FRectF Caret::GetCaretRect()
	{
		FRectF rc(0, 0, 2, 60);
		int pos = (is_start_change_ == 1 ? ti_->sel_start_pos : ti_->sel_end_pos);

		if (pos > 0)
		{
			rc = ti_->rcChar().get()[pos - 1];

			/*if ( rc.left==rc.right )
			{
			rc.Offset( 0, rc.Height() );
			rc.left = 0;
			}
			else*/
			{
				rc.Offset(rc.Width(), 0);
			}
			rc.right = rc.left + 2;
		}
		else if (pos == 0)
		{
			rc.SetHeight(ti_->fmt_->GetFontSize()); // 合わない?
		}

		rc.Offset(ti_->rcTextbox.left, ti_->rcTextbox.top);
		return rc;


	}
}; 

