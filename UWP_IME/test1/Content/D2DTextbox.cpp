#include "pch.h"
#include "D2DUniversalControl.h"
#include "comptr.h"

using namespace V4;

#define KEY_SHIFT 0x1
#define KEY_CONTROL 0x2

// Caretはstaticとして、D2DMainWindow内で表示。

int RCfromPosition(V4::D2CoreTextBridge& br, int pos, int* prow, int* pcol);
bool RCToPosition(D2CoreTextBridge& br, int target_row, int target_col, int* ppos);


std::wstring str_append(const std::wstring& str, int s, WCHAR ch);
std::wstring str_remove(const std::wstring& str, int s, int e);

bool D2DTextbox::bMouseSelectMode_;


D2DTextbox::D2DTextbox(D2CoreTextBridge& bridge) :bridge_(bridge), back_(ColorF::White), fore_(ColorF::Black)
{
	typ_ = TYP::SINGLELINE;
	shift_control_key_ = 0;
}
D2DTextbox::D2DTextbox(D2CoreTextBridge& bridge, TYP ty) : bridge_(bridge), back_(ColorF::White), fore_(ColorF::Black)
{
	typ_ = ty;
	shift_control_key_ = 0;
}

static std::wstring RemoveStringByBACK(const std::wstring& str, int& spos, int& epos)
{
	_ASSERT(spos <= (int) str.length());

	int acp = spos;
	int len = str.length();

	if (0 < acp)
	{
		int rmlen = max(1, epos - spos);

		spos = epos - rmlen;

		acp = spos;

		auto s1 = str.substr(0, acp);

		if (0 < (len - (acp + rmlen)))
		{
			auto s2 = str.substr(acp + rmlen, (len - (acp + rmlen)));
			s1 += s2;
		}
		epos = spos;

		return s1;
	}
	return str;
}
static std::wstring RemoveStringByDELETE(const std::wstring& str, int& spos, int& epos)
{
	_ASSERT(spos <= (int) str.length());

	int acp = spos;
	int len = str.length();

	if (acp < len)
	{
		int rmlen = max(1, epos - spos);

		auto s1 = str.substr(0, acp);

		if (0 < (len - (acp + rmlen)))
		{
			auto s2 = str.substr(acp + rmlen, (len - (acp + rmlen)));
			s1 += s2;
		}

		epos = spos;
		return s1;
	}
	return str;
}

void D2DTextbox::Create(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int controlid)
{
	InnerCreateWindow(parent, pacontrol, rc, stat, name, controlid);


	ti_.fmt_ = parent->cxt()->cxtt.textformat;
	ti_.wfac_ = parent_->cxt()->cxtt.wfactory;



}
int D2DTextbox::WndProc(D2DWindow* d, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	int ret = 0;
	switch (message)
	{
	case WM_PAINT:
	{
		auto& cxt = *(d->cxt());

		D2DMatrix mat(cxt);
		mat_ = mat.PushTransform();
		mat.Offset(rc_.left, rc_.top);

		ComPTR<ID2D1SolidColorBrush> backbr;
		cxt.cxt->CreateSolidColorBrush(back_, &backbr);

		ComPTR<ID2D1SolidColorBrush> forebr;
		cxt.cxt->CreateSolidColorBrush(fore_, &forebr);


		FRectF rc = rc_.ZeroRect();
		cxt.cxt->DrawRectangle(rc, cxt.black);
		cxt.cxt->FillRectangle(rc, backbr);


		auto fmt = ti_.fmt_;

		if (ti_.decoration_typ == 0)
		{
			FRectF rca = rc_.GetContentRect().ZeroRect();
			cxt.cxt->DrawText(ti_.text.c_str(), ti_.text.length(), fmt, rca, forebr);
		}
		else if (ti_.decoration_start_pos < ti_.decoration_end_pos)
		{
			LPCWSTR str = ti_.text.c_str();
			int len = ti_.text.length();

			FRectF rca = rc_.GetContentRect().ZeroRect();
			cxt.cxt->DrawText(str, ti_.decoration_start_pos, fmt, rca, cxt.black);

			FRectF rcb(rca);

			FRectF* rcc = ti_.rcChar().get();

			FPointF pt = rcc[ti_.decoration_start_pos].LeftTop();

			rcb.Offset(pt.x, pt.y);

			auto clr = cxt.black;

			if (ti_.decoration_typ == 1)
				clr = cxt.gray;
			else if (ti_.decoration_typ == 10)
				clr = cxt.bluegray;
			else if (ti_.decoration_typ == 9)
				clr = cxt.black;


			cxt.cxt->DrawText(str + ti_.decoration_start_pos, ti_.decoration_end_pos - ti_.decoration_start_pos, fmt, rcb, clr);

			// next line
			int r, c;
			RCfromPosition(bridge_, ti_.decoration_start_pos, &r, &c);

			int npos;
			if (RCToPosition(bridge_, r + 1, 0, &npos))
			{
				pt = rcc[npos].LeftTop();
				rca.Offset(pt.x, pt.y);
				cxt.cxt->DrawText(str + npos, len - npos, fmt, rca, forebr);
			}
		}

		DrawSelectArea(cxt);


		mat.PopTransform();
	}
	break;
	case WM_LBUTTONDOWN:
	{
		FPointF pt(lp);
		bMouseSelectMode_ = false;

		FPointF pt3 = mat_.DPtoLP(pt);

		if (rc_.PtInRect(pt3))
		{
			if (this == d->GetCapture())
			{
				// mouse select mode


				FPointF pt(pt3.x - rc_.left, pt3.y - rc_.top);

				FRectF* rc = ti_.rcChar().get();

				for (int i = 0; i < ti_.rcCharCnt; i++)
				{
					if (rc[i].PtInRect(pt))
					{
						ti_.sel_start_pos = ti_.sel_end_pos = i;
						bMouseSelectMode_ = true;

						ret = 1;
						break;
					}
				}

			}


			Activate();
			ret = 1;
		}
		else
		{
			if (bridge_.GetTarget() == this)
			{
				UnActivate();
				ret = 0;
			}

		}

	}
	break;
	case WM_MOUSEMOVE:
	{
		if (this == d->GetCapture() && bMouseSelectMode_)
		{
			FPointF pt3 = mat_.DPtoLP(FPointF(lp));
			FPointF pt(pt3.x - rc_.left, pt3.y - rc_.top);

			FRectF* rc = ti_.rcChar().get();

			for (int i = 0; i < ti_.rcCharCnt; i++)
			{
				if (rc[i].PtInRect(pt))
				{
					ti_.sel_end_pos = i;
					bMouseSelectMode_ = true;

					bridge_.SetCaret(ti_.sel_start_pos, ti_.sel_end_pos);

					ret = 1;
					break;
				}
			}
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (this == d->GetCapture() && bMouseSelectMode_)
		{
			bMouseSelectMode_ = false;
			ret = 1;
		}
	}
	break;
	case WM_KEYDOWN:
	{
		Windows::UI::Core::KeyEventArgs^ arg = (Windows::UI::Core::KeyEventArgs^)lp;

		if (bridge_.GetTarget() == this)
		{
			ret = 1;

			switch (arg->VirtualKey)
			{
			case Windows::System::VirtualKey::Back:
			{
				ti_.text = RemoveStringByBACK(ti_.text, ti_.sel_start_pos, ti_.sel_end_pos);
				bridge_.SetCaret(ti_.sel_start_pos);
				bridge_.UpdateTextRect(rc_.Size());
			}
			break;
			case Windows::System::VirtualKey::Delete:
			{
				ti_.text = RemoveStringByDELETE(ti_.text, ti_.sel_start_pos, ti_.sel_end_pos);
				bridge_.SetCaret(ti_.sel_start_pos);
				bridge_.UpdateTextRect(rc_.Size());
			}
			break;

			case Windows::System::VirtualKey::Left:
			{
				int acp = ti_.sel_start_pos;


				if (shift_control_key_ & KEY_SHIFT)
				{
					if (Caret::GetCaret().is_start_change_ == 0)
						Caret::GetCaret().is_start_change_ = 1;

					{
						if (Caret::GetCaret().is_start_change_ == 1)
							ti_.sel_start_pos = max(0, acp - 1);
						else
							ti_.sel_end_pos = max(ti_.sel_end_pos - 1, ti_.sel_start_pos);

						bridge_.SetCaret(ti_.sel_start_pos, ti_.sel_end_pos);

						if (ti_.sel_start_pos == ti_.sel_end_pos)
							Caret::GetCaret().is_start_change_ = 0;
					}
				}
				else
				{
					ti_.sel_start_pos = max(0, acp - 1);
					Caret::GetCaret().is_start_change_ = 0;
					ti_.sel_end_pos = ti_.sel_start_pos;
					bridge_.SetCaret(ti_.sel_start_pos);
				}
			}
			break;
			case Windows::System::VirtualKey::Right:
			{
				int acp = ti_.sel_end_pos;


				if (shift_control_key_ & KEY_SHIFT)
				{
					if (Caret::GetCaret().is_start_change_ == 0)
						Caret::GetCaret().is_start_change_ = -1;

					if (Caret::GetCaret().is_start_change_ == -1)
						ti_.sel_end_pos = min((int) ti_.text.length(), max(0, acp + 1));
					else
						ti_.sel_start_pos++;

					bridge_.SetCaret(ti_.sel_start_pos, ti_.sel_end_pos);

					if (ti_.sel_start_pos == ti_.sel_end_pos)
						Caret::GetCaret().is_start_change_ = 0;
				}
				else
				{
					ti_.sel_end_pos = min((int) ti_.text.length(), max(0, acp + 1));

					Caret::GetCaret().is_start_change_ = 0;
					ti_.sel_start_pos = ti_.sel_end_pos;
					bridge_.SetCaret(ti_.sel_start_pos);
				}

			}
			break;
			case Windows::System::VirtualKey::Home:
			{
				int r, c, pos;
				RCfromPosition(bridge_, ti_.sel_start_pos, &r, &c);

				if (RCToPosition(bridge_, r, 0, &pos))
				{
					ti_.sel_end_pos = ti_.sel_start_pos = pos;
					bridge_.SetCaret(ti_.sel_start_pos);
				}
				else
				{
					ti_.sel_end_pos = ti_.sel_start_pos = 0;
					bridge_.SetCaret(ti_.sel_start_pos);
				}

			}
			break;
			case Windows::System::VirtualKey::End:
			{
				int r, c, pos;
				RCfromPosition(bridge_, ti_.sel_start_pos, &r, &c);

				if (RCToPosition(bridge_, r + 1, 0, &pos))
				{
					ti_.sel_end_pos = ti_.sel_start_pos = pos - 1;
					bridge_.SetCaret(ti_.sel_start_pos);
				}
				else
				{
					ti_.sel_end_pos = ti_.sel_start_pos = ti_.text.length();
					bridge_.SetCaret(ti_.sel_start_pos);
				}
			}
			break;
			case Windows::System::VirtualKey::Shift:
				shift_control_key_ |= KEY_SHIFT;
				break;
			case Windows::System::VirtualKey::Control:
				shift_control_key_ |= KEY_CONTROL;
				break;
			case Windows::System::VirtualKey::Escape:

				break;
			case Windows::System::VirtualKey::Enter:
			{
				if (typ_ == TYP::MULTILINE)
				{
					TRACE(L"Windows::System::VirtualKey::Enter pos=%d\n", ti_.sel_start_pos);

					ti_.text = str_append(ti_.text, ti_.sel_start_pos, L'\n');

					ti_.sel_start_pos++;
					ti_.sel_end_pos = ti_.sel_start_pos;

					bridge_.NotifyTextChanged(1);


					bridge_.UpdateTextRect(rc_.Size());
				}
			}
			break;
			case Windows::System::VirtualKey::Up:
			{
				if (ti_.sel_start_pos > 0)
				{
					int row = 0, col = 0;
					RCfromPosition(bridge_, ti_.sel_start_pos, &row, &col);

					if (row > 0)
					{
						int target_row = row - 1;
						int target_col = col;

						int newpos;

						bool bl = RCToPosition(bridge_, target_row, target_col, &newpos);

						if (bl)
						{
							ti_.sel_end_pos = ti_.sel_start_pos = newpos;
							bridge_.SetCaret(ti_.sel_start_pos);
						}
					}
				}
			}
			break;
			case Windows::System::VirtualKey::Down:
			{
				int row = 0, col = 0;
				RCfromPosition(bridge_, ti_.sel_start_pos, &row, &col);


				{
					int target_row = row + 1;
					int target_col = col;

					int newpos;

					bool bl = RCToPosition(bridge_, target_row, target_col, &newpos);

					if (bl)
					{
						ti_.sel_end_pos = ti_.sel_start_pos = newpos;
						bridge_.SetCaret(ti_.sel_start_pos);
					}
				}
			}
			break;
			}

		}
	}
	break;
	case WM_KEYUP:
	{
		Windows::UI::Core::KeyEventArgs^ arg = (Windows::UI::Core::KeyEventArgs^)lp;

		if (bridge_.GetTarget() == this)
		{
			ret = 1;

			switch (arg->VirtualKey)
			{
			case Windows::System::VirtualKey::Shift:
				shift_control_key_ &= ~KEY_SHIFT;
				break;
			case Windows::System::VirtualKey::Control:
				shift_control_key_ &= ~KEY_CONTROL;
				break;
			}
		}

	}
	break;

	}
	return ret;

}
void D2DTextbox::Activate()
{
	auto rc = mat_.LPtoDP(rc_);

	_ASSERT(ti_.sel_start_pos <= ti_.sel_end_pos);

	ti_.rcTextbox = rc;

	shift_control_key_ = 0;

	ti_.decoration_start_pos = ti_.decoration_end_pos = 0;
	ti_.decoration_typ = 0;



	Caret::GetCaret().Activate(ti_);

	bridge_.Activate(&ti_, this);

	parent_->SetCapture(this);

}
void D2DTextbox::SetText(LPCWSTR txt)
{
	ti_.text = txt;
	ti_.sel_end_pos = ti_.sel_start_pos = 0;

	Activate();

	bridge_.SetCaret(ti_.sel_start_pos);
	bridge_.UpdateTextRect(rc_.Size());

}
void D2DTextbox::SetFont(const FontInfo& cf)
{

	ti_.fmt_ = cf.CreateFormat(ti_.wfac_);

	back_ = cf.backcolor;
	fore_ = cf.forecolor;
}


void D2DTextbox::UnActivate()
{
	if (parent_->GetCapture() == this)
		parent_->ReleaseCapture();

	Caret::GetCaret().UnActivate();

	bridge_.UnActivate();
}
void D2DTextbox::DrawSelectArea(D2DContext& cxt)
{
	if (ti_.sel_start_pos >= ti_.sel_end_pos) return;

	D2DMatrix mat(cxt);
	mat.PushTransform();

	FRectF* prc = ti_.rcChar().get();

	for (int i = ti_.sel_start_pos; i < ti_.sel_end_pos; i++)
	{
		cxt.cxt->FillRectangle(prc[i], cxt.halftone);
	}

	mat.PopTransform();
}




int RCfromPosition(V4::D2CoreTextBridge& br, int pos, int* prow, int* pcol)
{
	FRectF* rc = br.info_->rcChar().get();

	int rccnt = br.info_->rcCharCnt;

	float prtop = 0;
	int row = 0, col = 0;
	for (int i = 0; i < rccnt; i++)
	{
		if (i == pos)
			break;

		col++;
		if (rc[i].top != prtop)
		{
			row++;
			prtop = rc[i].top;
			col = 0;
		}
	}

	*prow = row;
	*pcol = col;

	int row_last_col_pos = 0;
	for (int i = col; i < rccnt; i++)
	{
		if (rc[i].top != prtop)
		{
			row_last_col_pos = i;
			break;
		}
	}

	return row_last_col_pos;
}

bool RCToPosition(D2CoreTextBridge& br, int target_row, int target_col, int* ppos)
{
	bool ret = false;
	FRectF* rc = br.info_->rcChar().get();
	const int len = br.info_->text.length();

	int j = 0;
	int r = 0, c = 0;
	float prtop = 0;

	float cw = 0;
	int col = 0;
	for (j = 0; j < len; j++)
	{
		cw = rc[j].Width();
		if (rc[j].top != prtop && cw)
		{
			prtop = rc[j].top;
		}
		else if (rc[j].top != prtop && cw == 0)
		{
			r++;
		}

		if (target_row == r)
		{
			ret = true;
			prtop = rc[j].top;

			if (cw == 0)
				j++;
			break;
		}
	}

	for (; j < len; j++)
	{
		if (rc[j].top != prtop)
		{
			break;
		}

		if (target_col == col)
			break;

		if (rc[j].Width())
			col++;
	}

	*ppos = j;


	return ret;
}


