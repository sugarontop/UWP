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

	cxt_.Init(m_deviceResources);
	cxt_.CreateRenderTargetResource(m_deviceResources->GetD2DDeviceContext());

	
	imebridge_ = br;
	redraw_ = true;

	OnInitail();


	Windows::Foundation::TimeSpan tm;
	tm.Duration = (10 * 1000) * 5000; // 5秒

	auto tmdelegate = [this](Windows::System::Threading::ThreadPoolTimer^ timer)
	{
		AliveMetor(timer);
	};
	ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(tmdelegate), tm);

}

void template1Main::OnInitail()
{
	//
	// Create Window controls. Button.
	// 

	FRectF rc(10,10,200,40);
	D2DButton* b1 = new D2DButton();
	b1->CreateButton( this, this, rc, 0, L"noname" );

	rc.Offset( 220, 0 );
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");

	rc.Offset(220, 0);
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");

	b1->OnClick_ = [this](D2DButton*)
	{
		// TEST 

		this->GetResource()->HandleDeviceLost(); // Force Device Lost
	};
	
	_ASSERT(imebridge_ != nullptr );

	//
	// Create Window controls. Textbox.
	// 
	
	FRectF rcTextbox( 10, 400, FSizeF(800,60));
	auto* t1 = new D2DTextbox(*imebridge_);
	t1->Create(this, this, rcTextbox, 0, L"noname");

	rcTextbox.Offset(0, 120);
	auto* t2 = new D2DTextbox(*imebridge_);
	t2->Create(this, this, rcTextbox, 0, L"noname");


	t1->SetText( L"This is textbox with IME.");
	t2->SetText(L"ここはtextbox。");
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

