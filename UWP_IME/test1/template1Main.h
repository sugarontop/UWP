#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "content\d2dcontext.h"

#include "content\d2dUniversalControl.h"

using namespace V4;

// Direct2D および 3D コンテンツを画面上でレンダリングします。
namespace template1
{
	
	class template1Main : public DX::IDeviceNotify, D2DWindow, D2DControls
	{
	public:
		template1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources, D2CoreTextBridge* br );
		~template1Main();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

		virtual int WndProc(D2DWindow* parent, int message, int wp, Windows::UI::Core::ICoreWindowEventArgs^ lp);
		virtual D2DContext* cxt(){ return &cxt_; }
		bool redraw_;

		
	protected :
		void OnDraw(D2DContext& cxt);

		void OnInitail();
		

	private:
		// デバイス リソースへのキャッシュされたポインター。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		
		D2DContext cxt_;
		D2CoreTextBridge* imebridge_;
		
	};
}