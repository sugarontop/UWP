#include "pch.h"
#include "D2DUniversalControl.h"
#include "D2DWindowMessage.h"
namespace V4 {

D2DMainWindow::D2DMainWindow():back_color_(ColorF(ColorF::AliceBlue))
{
	
}

void D2DMainWindow::SetCapture(D2DCaptureObject* p, FPointF* pt, D2DMat* mat )
{
	if (capture_obj_.empty() )
		capture_obj_.push(p);
	else if ( p != capture_obj_.top())
		capture_obj_.push(p);

	if ( pt )
		capture_pt_ = *pt; 
	else
		capture_pt_ = (FPointF(0,0));

	if ( mat )
		capture_matrix_ = *mat;

	//////////////////////////////////////

	if ( !view_controls_.empty())
	{
		view_temp_.push( view_controls_ );
		view_controls_.clear();
	}
	view_controls_.resize( controls_.size() );
			
	UINT j = 0, k = 0;
	for( auto& it : controls_ )
	{
		if ( p != it.get() )
			view_controls_[j++] = it.get();
		else
		{
			k = j;
		}
	}

	if ( j < view_controls_.size())
		view_controls_[j] = controls_[k].get();

}
D2DCaptureObject* D2DMainWindow::ReleaseCapture()
{
	auto p = capture_obj_.top();
	//roundpaint_obj_ = nullptr;

	capture_obj_.pop();		

	//HWND hWnd = ::GetCapture();

	if ( capture_obj_.empty())
	{		
		//::ReleaseCapture();
	}

	//::SetFocus( hWnd );
	redraw_ = true;


	view_controls_.clear();
	if ( !view_temp_.empty())
	{
		view_controls_ = view_temp_.top();
		view_temp_.pop();
	}


	return p;
}
int D2DMainWindow::NomarlWndProc( D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	int ret = 0;
	D2DCaptureObject* cap = GetCapture();
	if ( cap )
	{
		ret = cap->WndProc( this, message,wp,lp );

		if ( ret == 0 )
		{
			for (auto& it : controls_)
			{				
				if ( cap != (D2DCaptureObject*)it.get() )
				{
					ret = it->WndProc(this, message, wp, lp);
					if ( ret )
						break;
				}
			}
		}

	}
	else
	{
		for (auto& it : controls_)
		{
			ret = it->WndProc(this, message, wp, lp);
			if ( ret )
				break;
		}
	}
	return ret;
}
int D2DMainWindow::WndProc(D2DWindow* parent, int msg, INT_PTR wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	switch( msg )
	{		
		case WM_PAINT:
		{
			ID2D1DeviceContext* cxt = GetResource()->GetD2DDeviceContext();
			//Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

			cxt->SaveDrawingState( cxt_.m_stateBlock);
			cxt->BeginDraw();
			D2D1_MATRIX_3X2_F mat = Matrix3x2F::Identity();

			
			cxt->SetTransform(mat);
			cxt->Clear(back_color_);

			if (capture_obj_.empty()) 
			{
				for( auto& it : controls_ )
					it->WndProc(this, WM_PAINT,0,nullptr);
			}
			else
			{
				for( auto& it : view_controls_ )
					it->WndProc(this, WM_PAINT,0,nullptr);
			}


#ifdef _DEBUG
			D2DMat xm;
			cxt->GetTransform(&xm);
			_ASSERT( xm._31 == 0 && xm._32 == 0 );
#endif

			bool bl = Caret::GetCaret().Draw( cxt_ );

			//D2DERR_RECREATE_TARGET をここで無視します。このエラーは、デバイスが失われたことを示します。
			// これは、Present に対する次回の呼び出し中に処理されます。
			HRESULT hr = cxt->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				//DX::ThrowIfFailed(hr);
			}

			cxt->RestoreDrawingState(cxt_.m_stateBlock);


			redraw_ = bl; //false;
			return 0;
		}
		break;		
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		{
			redraw_ = true;
			NomarlWndProc( this, msg, wp, lp );
		}	
		break;
		case WM_D2D_IDLE:
		{
			if ( capture_obj_.empty() )
			{
				pre_death_objects_.clear();
			}
		}
		break;
		case WM_SIZE:
		{
			Windows::UI::Core::WindowSizeChangedEventArgs^ args = (Windows::UI::Core::WindowSizeChangedEventArgs^)lp;

			rc_.top = 0;
			rc_.left = 0;
			rc_.right = args->Size.Width;
			rc_.bottom = args->Size.Height;
			NomarlWndProc( this, msg, wp, lp );
		}	
		break;
		case WM_D2D_SIZE_FITTING:
		{
			// スルー

			NomarlWndProc( this, msg, wp, lp );
		}	
		break;

		default:
		{
			NomarlWndProc( this, msg, wp, lp );
		}
		break;
	}


	

	return 0;
}


void D2DMainWindow::AddDeath( std::shared_ptr<D2DControl> obj )
{
	pre_death_objects_.push_back( obj );
}
void D2DMainWindow::AliveMetor(Windows::System::Threading::ThreadPoolTimer^ timer)
{	
	WndProc(0,WM_D2D_IDLE,0,nullptr);

	TRACE( L"AliveMetor\n" );
}

}