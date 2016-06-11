#include "pch.h"
#include "template1Main.h"
#include "Common\DirectXHelper.h"

using namespace template1;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::UI::Text::Core;


// アプリケーションの読み込み時にアプリケーション資産を読み込んで初期化します。
template1Main::template1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2CoreTextBridge* br) :
	m_deviceResources(deviceResources)
{
	// デバイスが失われたときや再作成されたときに通知を受けるように登録します
	m_deviceResources->RegisterDeviceNotify(this);


	static V4::SingletonD2DInstance ins;
	cxt_.Init(ins, m_deviceResources );
	cxt_.CreateRenderTargetResource(m_deviceResources->GetD2DDeviceContext());

	
	imebridge_ = br;
	redraw_ = true;

	OnInitail();

}

void template1Main::OnInitail()
{
	FRectF rc(10,10,200,40);
	D2DButton* b1 = new D2DButton();
	b1->CreateButton( this, this, rc, 0, L"noname" );

	rc.Offset( 220, 0 );
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");

	rc.Offset(220, 0);
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");





	_ASSERT(imebridge_ != nullptr );

	rc.Offset(-220, 300);
	rc.SetSize(400,100);
	auto* t1 = new D2DTextbox(*imebridge_);
	t1->Create(this, this, rc, 0, L"noname");

	rc.Offset(0, 120);	
	auto* t2 = new D2DTextbox(*imebridge_);
	t2->Create(this, this, rc, 0, L"noname");
}


template1Main::~template1Main()
{
	// デバイスの通知を登録解除しています
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

//ウィンドウのサイズが変更される (デバイスの方向が変更されるなど) 場合に、 アプリケーションの状態を更新します。
void template1Main::CreateWindowSizeDependentResources() 
{
	// TODO: これをアプリのコンテンツのサイズに依存する初期化で置き換えます。
	//m_sceneRenderer->CreateWindowSizeDependentResources();
}

// アプリケーション状態をフレームごとに 1 回更新します。
void template1Main::Update() 
{
	// シーン オブジェクトを更新します。

}

// 現在のアプリケーション状態に応じて現在のフレームをレンダリングします。
// フレームがレンダリングされ、表示準備が完了すると、true を返します。
bool template1Main::Render() 
{
	WndProc(0,WM_PAINT,0,nullptr);

	

	return true;
}

// デバイス リソースを解放する必要が生じたことをレンダラーに通知します。
void template1Main::OnDeviceLost()
{
	cxt_.DestroyRenderTargetResource();
}

// デバイス リソースの再作成が可能になったことをレンダラーに通知します。
void template1Main::OnDeviceRestored()
{
	cxt_.CreateRenderTargetResource(m_deviceResources->GetD2DDeviceContext());
	
}


int template1Main::WndProc(D2DWindow* parent, int msg, INT_PTR wp, Windows::UI::Core::ICoreWindowEventArgs^ lp)
{
	switch( msg )
	{		
		case WM_PAINT:
		{
			ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
			Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

			context->SaveDrawingState( cxt_.m_stateBlock);
			context->BeginDraw();
			D2D1_MATRIX_3X2_F mat = Matrix3x2F::Identity();

			ID2D1DeviceContext* cxt = context;
			cxt->SetTransform(mat);
			cxt->Clear(ColorF(ColorF::White));


			for( auto& it : controls_ )
				it->WndProc(this, WM_PAINT,0,nullptr);

			

			//D2DERR_RECREATE_TARGET をここで無視します。このエラーは、デバイスが失われたことを示します。
			// これは、Present に対する次回の呼び出し中に処理されます。
			HRESULT hr = context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)
			{
				DX::ThrowIfFailed(hr);
			}

			context->RestoreDrawingState(cxt_.m_stateBlock);


			redraw_ = false;
			return 0;
		}
		break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			for (auto& it : controls_)
				it->WndProc(this, msg, wp, lp);

			redraw_ = true;
		break;

		default:
			for (auto& it : controls_)
				it->WndProc(this, msg, wp, lp);
		break;
	}


	

	return 0;
}


