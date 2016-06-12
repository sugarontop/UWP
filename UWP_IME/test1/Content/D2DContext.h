#pragma once
#include "D2DMisc.h"
#include "comptr.h"
#include "..\Common\DeviceResources.h"
namespace V4
{
	struct D2DContextBase
	{
		ComPTR<ID2D1RenderTarget>  cxt;
		IDWriteTextFormat* text;
		IDWriteFactory* wfactory;
	};


	// SingletonD2DInstanceは独立した存在なので、HWNDに関わるリソースはもたない。
	struct SingletonD2DInstance
	{
		ComPTR<IDWriteFactory> wrfactory;
		ComPTR<ID2D1Factory>  factory;
		ComPTR<IDWriteTextFormat> text; // IDWriteTextFormat1 is from Win8.1.

		static SingletonD2DInstance& Init();

	};

#define STOCKSIZE 16
	struct D2DContext;



	struct D2DContextText
	{
		bool Init(D2DContext& inshw, float height, LPCWSTR fontname);

		UINT GetLineMetrics(const D2D1_SIZE_F& sz, LPCWSTR str, int len, DWRITE_TEXT_METRICS& textMetrics, std::vector<DWRITE_LINE_METRICS>& lineMetrics);
		UINT GetLineMetric(const D2D1_SIZE_F& sz, LPCWSTR str, int len, DWRITE_TEXT_METRICS& textMetrics, DWRITE_LINE_METRICS& lineMetric);
		UINT GetLineMetric(const D2D1_SIZE_F& sz, LPCWSTR str, int len, DWRITE_TEXT_METRICS& textMetrics);
		UINT GetLineMetric(const D2D1_SIZE_F& sz, IDWriteTextFormat* fmt, LPCWSTR str, int len, DWRITE_TEXT_METRICS& textMetrics);

		ComPTR<IDWriteTextFormat> textformat;
		ComPTR<IDWriteFactory> wfactory;

		float xoff;			// １行表示の左端の余幅
		float line_height;	// １行表示の高さ

	};
	struct D2DContext : public D2DContextBase
	{
		/*
		struct D2DContextBase
		{
		ComPTR<ID2D1RenderTarget>  cxt;
		IDWriteTextFormat* text;
		IDWriteFactory* wfactory;
		};
		*/



		SingletonD2DInstance* insins;


		operator ID2D1RenderTarget*() const { return cxt.p; }
#ifdef USE_ID2D1DEVICECONTEXT
		ComPTR<IDXGISwapChain1> dxgiSwapChain;
		D2D1_SIZE_U RenderSize_;
#endif

		ComPTR<ID2D1SolidColorBrush> ltgray;
		ComPTR<ID2D1SolidColorBrush> black;
		ComPTR<ID2D1SolidColorBrush> white;
		ComPTR<ID2D1SolidColorBrush> red;
		ComPTR<ID2D1SolidColorBrush> gray;
		ComPTR<ID2D1SolidColorBrush> bluegray;
		ComPTR<ID2D1SolidColorBrush> transparent;
		ComPTR<ID2D1SolidColorBrush> halftone;
		ComPTR<ID2D1SolidColorBrush> halftoneRed;
		ComPTR<ID2D1SolidColorBrush> tooltip;

		ComPTR<ID2D1StrokeStyle> dot4_;
		ComPTR<ID2D1StrokeStyle> dot2_;
		ComPTR<ID2D1DrawingStateBlock>  m_stateBlock;

		ComPTR<ID2D1Factory> factory() { return insins->factory; }
		D2DContextText cxtt;

		LPVOID free_space;

		//void Init(SingletonD2DInstance& ins, HWND hWnd);
		void Destroy();

		void Init(SingletonD2DInstance& ins, const std::shared_ptr<DX::DeviceResources>& deviceResources);


		void CreateResourceOpt();

		void CreateDeviceContextRenderTargetTest(HWND hWnd, UINT width);


		void DestroyRenderTargetResource();

		void DestroyAll();

		void SetAntiAlias(bool bl) { cxt->SetAntialiasMode(bl ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED); }


		HRESULT CreateFont(LPCWSTR fontnm, float height, IDWriteTextFormat** ret);


		void CreateRenderResource(HWND hWnd);


		void CreateRenderTargetResource(ID2D1RenderTarget* t);

	protected:
		void CreateHwndRenderTarget(HWND hWnd);
		void CreateDeviceContextRenderTarget(HWND hWnd);
		
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};

	struct D2DRectFilter
	{
		D2DRectFilter(D2DContext& cxt1, const FRectF& rc) :cxt(cxt1)
		{
			cxt.cxt->PushAxisAlignedClip(rc, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			cnt = 1;
		}

		D2DRectFilter(D2DContext& cxt1, FRectF&& rc) :cxt(cxt1)
		{
			cxt.cxt->PushAxisAlignedClip(std::move(rc), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			cnt = 1;
		}

		~D2DRectFilter()
		{
			if (cnt == 1)
				cxt.cxt->PopAxisAlignedClip();
		}
		void Off()
		{
			if (cnt == 1)
			{
				cxt.cxt->PopAxisAlignedClip();
				cnt = 0;
			}
		}

	private:
		D2DContext& cxt;
		int cnt;
	};



	/////////////////////////////////////////////////////////////////////////////////////////////
	class D2DError
	{
	public:
		explicit D2DError(HRESULT hr, LPCWSTR msg, UINT line, LPCSTR fnm) :hr_(hr), msg_(msg), line_(line), fnm_(fnm)
		{
			FString msg1 = FString::Format(L"%s :%d行 HR=%x %s\n", (LPCWSTR) fnm_, line_, hr_, (LPCWSTR) msg_);

			::OutputDebugString(msg1);
		}

	public:
		FString msg_;
		FString fnm_;
		UINT line_;
		HRESULT hr_;

	};
	inline void ThrowIfFailed(HRESULT hr, LPCWSTR msg, UINT line, LPCSTR fnm)
	{
		if (FAILED(hr))
		{
			throw D2DError(hr, msg, line, fnm);
			//General access denied error 0x80070005 
		}
	};
#define THROWIFFAILED(hr,msg) ThrowIfFailed(hr,msg, __LINE__, __FILE__)



	
	struct TextInfo
	{
		TextInfo()
		{
			sel_start_pos = sel_end_pos =0;
			decoration_start_pos = decoration_end_pos =0;
			decoration_typ = 0;
			
		}
		std::wstring text;
		int sel_start_pos;
		int sel_end_pos;
		FRectF rcTextbox;
		
		std::shared_ptr<FRectF> rcChar;

		int decoration_start_pos;
		int decoration_end_pos;
		int decoration_typ;

		ComPTR<IDWriteFactory> wfac_;
		ComPTR<IDWriteTextFormat> fmt_;
	};


	class D2CoreTextBridge
	{
		public :
			D2CoreTextBridge():edcxt_(nullptr),target_(nullptr){}
			void Set( Windows::UI::Text::Core::CoreTextEditContext^ cxt)
			{	
				edcxt_ = cxt;
				edSelection_.StartCaretPosition = 0;
				edSelection_.EndCaretPosition = 0;
			}
			void Activate(TextInfo* inf, void* target)
			{
				target_ = target;
				info_ = inf;
				edcxt_->NotifyFocusEnter();

				Windows::UI::Text::Core::CoreTextRange rng;
				Windows::UI::Text::Core::CoreTextRange sel;

				sel.StartCaretPosition = info_->sel_start_pos;
				sel.EndCaretPosition = info_->sel_end_pos;


				edcxt_->NotifyTextChanged(rng,0,sel);
			}
			void UnActivate()
			{
				target_ = nullptr;
				info_ = nullptr;
				edSelection_.StartCaretPosition = 0;
				edSelection_.EndCaretPosition = 0;

				edcxt_->NotifyFocusLeave();
			}

			void SetCaret(int pos)
			{
				Windows::UI::Text::Core::CoreTextRange sel;
				sel.StartCaretPosition = pos;
				sel.EndCaretPosition = pos;

				edcxt_->NotifySelectionChanged(sel);
			}
			void SetCaret(int spos, int epos)
			{
				Windows::UI::Text::Core::CoreTextRange sel;
				sel.StartCaretPosition = spos;
				sel.EndCaretPosition = epos;

				edcxt_->NotifySelectionChanged(sel);
			}

			void SetNewSelection( Windows::UI::Text::Core::CoreTextRange& ed )
			{
				edSelection_ = ed;
				info_->sel_start_pos = edSelection_.StartCaretPosition ;
				info_->sel_end_pos = edSelection_.EndCaretPosition ;
			}

			void CompositionStarted(){}
			void CompositionCompleted()
			{
				info_->decoration_typ = 0;
				info_->decoration_start_pos = info_->decoration_end_pos = 0;
				
			}

			Windows::UI::Text::Core::CoreTextRange GetSelection()
			{
				edSelection_.EndCaretPosition =	info_->sel_end_pos;
				edSelection_.StartCaretPosition = info_->sel_start_pos;

				return edSelection_;
			}
			void* GetTarget()
			{
				return target_;
			}
			
			void UpdateText( FSizeF rcMaxText )
			{
				int len = info_->text.length();

				if (len == 0)
				{
					info_->rcChar.reset();
					return;
				}

				ComPTR<IDWriteTextLayout> temp;		

				info_->wfac_->CreateTextLayout(info_->text.c_str(), len, info_->fmt_, rcMaxText.width, rcMaxText.height, &temp);
				info_->rcChar = std::shared_ptr<FRectF>(new FRectF[len], std::default_delete<FRectF[]>());

				FRectF* prc = info_->rcChar.get();
				FRectF rc;

				for (int i = 0; i < len; i++)
				{
					DWRITE_HIT_TEST_METRICS tm;
					float x1 = 0, y1 = 0;
					temp->HitTestTextPosition(i, false, &x1, &y1, &tm);

					float cx = tm.width;

					rc.SetSize(cx, tm.height);
					
					prc[i] = rc;

					rc.Offset(cx, 0 );
				}
			}

			TextInfo* info_;
			

			private :
				void* target_;
				Windows::UI::Text::Core::CoreTextEditContext^ edcxt_;
				Windows::UI::Text::Core::CoreTextRange edSelection_;			
				
	};


#ifdef _DEBUG
#define TRACE FString::Trace
#else
#define TRACE
#endif

};



