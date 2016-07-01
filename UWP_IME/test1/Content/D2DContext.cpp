#include "pch.h"
#include "D2DContext.h"
#include "Common/DirectXHelper.h"

using namespace V4;

void D2DContext::Init(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	m_deviceResources = deviceResources;

	m_deviceResources->GetDWriteFactory()->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"en-US",
		&text
	);

}
void D2DContext::CreateRenderTargetResource(ID2D1RenderTarget* rt)
{
	cxt = rt;

	ComPTR<ID2D1Factory> fac;
	rt->GetFactory(&fac);
	fac->CreateDrawingStateBlock(&m_stateBlock);

	cxtt.textformat = text;
	cxtt.wfactory = m_deviceResources->GetDWriteFactory();

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

	// DeviceLost時にこれらすべてReleaseしなければならない
}
void D2DContext::DestroyRenderTargetResource()
{
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

	cxt.Release();
	m_stateBlock.Release();
}
