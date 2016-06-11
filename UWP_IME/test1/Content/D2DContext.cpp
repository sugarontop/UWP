#include "pch.h"
#include "D2DContext.h"
#include "Common/DirectXHelper.h"

using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::Storage::Pickers;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Web::Http;
using namespace concurrency;
using namespace Windows::Web;

using namespace Windows::Foundation;

using namespace V4;


SingletonD2DInstance& SingletonD2DInstance::Init()
{
	static SingletonD2DInstance st;

	if (st.factory.p == NULL)
	{
		D2D1_FACTORY_OPTIONS options;
		options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
		//THROWIFFAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**) &st.factory), L"SingletonD2DInstance::Init()");
		//THROWIFFAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1), reinterpret_cast<IUnknown**>(&st.wrfactory)), L"SingletonD2DInstance::Init()");
		//THROWIFFAILED(st.wrfactory->CreateTextFormat(DEFAULTFONT, 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DEFAULTFONT_HEIGHT, L"", &st.text), L"SingletonD2DInstance::Init()");
	}
	return st;


}
void D2DContext::Init(SingletonD2DInstance& ins, const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	insins = &ins;
	m_deviceResources = deviceResources;


	ins.wrfactory = m_deviceResources->GetDWriteFactory();
	ins.factory = m_deviceResources->GetD2DFactory();

	m_deviceResources->GetDWriteFactory()->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"en-US",
		&ins.text
	);
	
	ins.factory->CreateDrawingStateBlock(&m_stateBlock); 


	this->cxt = deviceResources->GetD2DDeviceContext();
	
	cxtt.textformat = ins.text;
	cxtt.wfactory = ins.wrfactory;
}
void D2DContext::CreateRenderTargetResource(ID2D1RenderTarget* rt)
{
	// D2DWindow::CreateD2DWindowのタイミングで実行
	// D2DWindow::WM_SIZEのタイミングで実行


	

	rt->CreateSolidColorBrush(D2RGB(0, 0, 0), &black);
	rt->CreateSolidColorBrush(D2RGB(255, 255, 255), &white);
	rt->CreateSolidColorBrush(D2RGB(192, 192, 192), &gray);
	rt->CreateSolidColorBrush(D2RGB(255, 0, 0), &red);
	rt->CreateSolidColorBrush(D2RGB(230, 230, 230), &ltgray);
	rt->CreateSolidColorBrush(D2RGB(113, 113, 130), &bluegray);
	rt->CreateSolidColorBrush(D2RGBA(0, 0, 0, 0), &transparent);

	rt->CreateSolidColorBrush(D2RGBA(113, 113, 130, 100), &halftone);
	rt->CreateSolidColorBrush(D2RGBA(250, 113, 130, 150), &halftoneRed);

	rt->CreateSolidColorBrush(D2RGBA(255, 242, 0, 255), &tooltip);

	// DestroyRenderTargetResourceを忘れないこと
}
void D2DContext::DestroyRenderTargetResource()
{
/*	UINT a = cxt.p->AddRef() - 1;
	cxt.p->Release();
	_ASSERT(a == 1);

	cxt.Release();
#ifdef USE_ID2D1DEVICECONTEXT
	a = dxgiSwapChain.p->AddRef() - 1;
	dxgiSwapChain.p->Release();
	_ASSERT(a == 1);

	dxgiSwapChain.Release();
#endif
*/
	ltgray.Release();
	black.Release();
	white.Release();
	red.Release();
	gray.Release();
	bluegray.Release();
	transparent.Release();
	halftone.Release();
	halftoneRed.Release();
	tooltip.Release();

}
