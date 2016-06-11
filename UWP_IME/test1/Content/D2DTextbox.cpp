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
	//mode_ = 0;
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
				rcb.Offset( MesureSingleText(cxt, ti_.decoration_start_pos ), 0 );

				auto clr = cxt.red;
				
				if ( ti_.decoration_typ == 1 )
					clr = cxt.gray;
				else if ( ti_.decoration_typ == 10 )
					clr = cxt.red;
				else if ( ti_.decoration_typ == 9 )
					clr = cxt.halftoneRed;


				cxt.cxt->DrawText( str+ti_.decoration_start_pos,  ti_.decoration_end_pos-ti_.decoration_start_pos, cxt.cxtt.textformat, rcb, clr );

				rca.Offset( MesureSingleText(cxt, ti_.decoration_end_pos ), 0 );
				cxt.cxt->DrawText( str+ti_.decoration_end_pos,  len-ti_.decoration_end_pos, cxt.cxtt.textformat, rca, cxt.black );

			}

			DrawCaret( cxt );


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

			//mode_ = 0;

			if ( rc_.PtInRect(pt3 ))
			{
				Activate();
				ret = 1;
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

	ti_.global_rc = rc;
	shift_control_key_ = 0;

	ti_.decoration_start_pos = ti_.decoration_end_pos = 0;
	ti_.decoration_typ = 0;


	bridge_.Activate( &ti_, this );
}
void D2DTextbox::UnActivate()
{
	bridge_.UnActivate();
}
void D2DTextbox::DrawCaret(D2DContext& cxt)
{	
	FRectF rca(0,0,2,60);


	float ypos = 0;
	float xpos = MesureSingleText( cxt, ti_.sel_start_pos );
	


	D2DMatrix mat(cxt);
	mat.PushTransform();
	mat.Offset(xpos, ypos);

	if ( ti_.sel_start_pos == ti_.sel_end_pos )
		cxt.cxt->DrawRectangle( rca, cxt.black );
	else
	{
		float xpos2 = MesureSingleText( cxt, ti_.sel_end_pos );

		rca.right = rca.left + ( xpos2-xpos );
		cxt.cxt->FillRectangle( rca, cxt.halftone );
	}

	mat.PopTransform();
}
float D2DTextbox::MesureSingleText(D2DContext& cxt, int pos )
{
	if ( pos == 0 ) return 0;

	ComPTR<IDWriteTextLayout> temp;

	cxt.cxtt.wfactory->CreateTextLayout( ti_.text.c_str(), ti_.text.length(), cxt.cxtt.textformat, rc_.Width(), rc_.Height(), &temp );

	DWRITE_HIT_TEST_METRICS tm; // posにある文字の情報、文字列情報ではない
	float x1=0,y1=0;
	temp->HitTestTextPosition( pos, false, &x1,&y1,&tm);

	return tm.left;
}

