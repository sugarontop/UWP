#include "pch.h"
#include "D2DUniversalControl.h"
#include "comptr.h"

using namespace V4;

#define KEY_SHIFT 0x1
#define KEY_CONTROL 0x2


D2DTextbox::D2DTextbox(D2CoreTextBridge& bridge):bridge_(bridge)
{
	typ_ = TYP::SINGLELINE;
	shift_control_key_ = 0;
}

static std::wstring RemoveStringByBACK( const std::wstring& str, int& spos, int& epos )
{
	_ASSERT( spos <= (int)str.length() );
					
	int acp = spos;
	int len = str.length();
				
	if ( 0 < acp )
	{
		int rmlen = max(1, epos - spos );

		spos = epos - rmlen;

		acp = spos;
														
		auto s1 = str.substr( 0, acp );

		if ( 0 < (len-(acp+rmlen)) )
		{
			auto s2 = str.substr( acp+rmlen, (len-(acp+rmlen)) );
			s1 += s2;
		}
		epos = spos;

		return s1;
	}
	return str;
}
static std::wstring RemoveStringByDELETE( const std::wstring& str, int& spos, int& epos )
{
	_ASSERT( spos <= (int)str.length() );
					
	int acp = spos;
	int len = str.length(); 
				
	if ( acp < len )
	{
		int rmlen = max(1, epos-spos );

		auto s1 = str.substr( 0, acp );
							
		if ( 0 < (len-(acp+rmlen)) )
		{
			auto s2 = str.substr( acp+rmlen, (len-(acp+rmlen)) );
			s1 += s2;
		}						
		
		epos = spos;
		return s1;
	}
	return str;
}

void D2DTextbox::Create(D2DWindow* parent, D2DControls* pacontrol, const FRectFBoxModel& rc, int stat, LPCWSTR name, int controlid)
{
	InnerCreateWindow(parent,pacontrol,rc,stat,name, controlid);
}
int D2DTextbox::WndProc(D2DWindow* d, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
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

			if ( ti_.decoration_typ == 0 )
			{				
				FRectF rca = rc_.GetContentRect().ZeroRect();
				cxt.cxt->DrawText( ti_.text.c_str(), ti_.text.length(), cxt.cxtt.textformat, rca, cxt.black );
			}
			else if ( ti_.decoration_start_pos < ti_.decoration_end_pos )
			{
				LPCWSTR str = ti_.text.c_str();
				int len = ti_.text.length();

				FRectF rca = rc_.GetContentRect().ZeroRect();
				cxt.cxt->DrawText( str,  ti_.decoration_start_pos, cxt.cxtt.textformat, rca, cxt.black );

				FRectF rcb(rca);
				FRectF* rcc = ti_.rcChar.get();
				rcb.Offset(rcc[ti_.decoration_start_pos].left, 0);

				

				auto clr = cxt.red;
				
				if ( ti_.decoration_typ == 1 )
					clr = cxt.gray;
				else if ( ti_.decoration_typ == 10 )
					clr = cxt.red;
				else if ( ti_.decoration_typ == 9 )
					clr = cxt.halftoneRed;


				cxt.cxt->DrawText( str+ti_.decoration_start_pos,  ti_.decoration_end_pos-ti_.decoration_start_pos, cxt.cxtt.textformat, rcb, clr );
				rca.Offset(rcc[ti_.decoration_end_pos].left, 0);

				cxt.cxt->DrawText( str+ti_.decoration_end_pos,  len-ti_.decoration_end_pos, cxt.cxtt.textformat, rca, cxt.black );
			}

			DrawSelectArea( cxt );


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

			if ( rc_.PtInRect(pt3 ))
			{
				Activate();
				ret = 1;
			}
			else
			{
				if (bridge_.GetTarget() == this)
					UnActivate();
	
			}

		} 
		break;
		case WM_KEYDOWN:
		{
			Windows::UI::Core::KeyEventArgs^ arg = (Windows::UI::Core::KeyEventArgs^)lp;

			if ( bridge_.GetTarget() == this )
			{
				ret = 1;
				
				switch ( arg->VirtualKey )
				{
					case Windows::System::VirtualKey::Back:
					{							
						ti_.text = RemoveStringByBACK( ti_.text, ti_.sel_start_pos, ti_.sel_end_pos );
						bridge_.SetCaret(ti_.sel_start_pos);
					}
					break;
					case Windows::System::VirtualKey::Delete:
					{
						ti_.text = RemoveStringByDELETE( ti_.text, ti_.sel_start_pos, ti_.sel_end_pos );
						bridge_.SetCaret(ti_.sel_start_pos);
					}
					break;

					case Windows::System::VirtualKey::Left:
					{
						int acp = ti_.sel_start_pos;
						ti_.sel_start_pos = max( 0, acp-1);

						if ( shift_control_key_ & KEY_SHIFT )
						{
							bridge_.SetCaret(ti_.sel_start_pos, ti_.sel_end_pos);
						}
						else
						{
							ti_.sel_end_pos = ti_.sel_start_pos;
							bridge_.SetCaret(ti_.sel_start_pos);
						}
					}
					break;
					case Windows::System::VirtualKey::Right:
					{
						int acp = ti_.sel_end_pos;
						ti_.sel_end_pos = min( (int)ti_.text.length(), max( 0, acp+1));

						if ( shift_control_key_ & KEY_SHIFT )
						{
							bridge_.SetCaret(ti_.sel_start_pos, ti_.sel_end_pos);
						}
						else
						{
							ti_.sel_start_pos = ti_.sel_end_pos;
							bridge_.SetCaret(ti_.sel_start_pos);
						}
					}
					break;
					case Windows::System::VirtualKey::Home:
					{
						ti_.sel_start_pos = 0;
						ti_.sel_end_pos = ti_.sel_start_pos;
						bridge_.SetCaret(ti_.sel_start_pos);
					}
					break;
					case Windows::System::VirtualKey::End:
					{
						ti_.sel_start_pos = ti_.text.length();
						ti_.sel_end_pos = ti_.sel_start_pos;
						bridge_.SetCaret(ti_.sel_start_pos);
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
				}
				
			}
		}
		break;
		case WM_KEYUP:
		{
			Windows::UI::Core::KeyEventArgs^ arg = (Windows::UI::Core::KeyEventArgs^)lp;

			if ( bridge_.GetTarget() == this )
			{
				ret = 1;
				
				switch ( arg->VirtualKey )
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

	_ASSERT( ti_.sel_start_pos <= ti_.sel_end_pos);

	ti_.rcTextbox = rc;

	shift_control_key_ = 0;

	ti_.decoration_start_pos = ti_.decoration_end_pos = 0;
	ti_.decoration_typ = 0;


	ti_.wfac_ = parent_->cxt()->cxtt.wfactory;
	ti_.fmt_ = parent_->cxt()->cxtt.textformat;


	bridge_.Activate( &ti_, this );
}
void D2DTextbox::UnActivate()
{
	bridge_.UnActivate();
}
void D2DTextbox::DrawSelectArea(D2DContext& cxt)
{
	if ( ti_.text.length() == 0 )
	{
		FRectF rcb(0,0,2,20);
		cxt.cxt->FillRectangle(rcb, cxt.black);
		return;
	}

	FRectF rc1 = ti_.rcChar.get()[ti_.sel_start_pos];
	FRectF rc2 = ti_.rcChar.get()[ti_.sel_end_pos - 1];


	D2DMatrix mat(cxt);
	mat.PushTransform();

	FRectF rca(rc1.left, rc1.top, rc2.right, rc2.bottom);

	if (ti_.sel_start_pos < ti_.sel_end_pos) 
		cxt.cxt->FillRectangle(rca, cxt.halftone);

	rca.left = rca.right -2;
	cxt.cxt->FillRectangle(rca, cxt.black);


	mat.PopTransform();
}


