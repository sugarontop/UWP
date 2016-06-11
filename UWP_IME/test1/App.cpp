#include "pch.h"
#include "App.h"

#include <ppltasks.h>


using namespace template1;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Text::Core;

// main 関数は、IFrameworkView クラスを初期化する場合にのみ使用します。
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// IFrameworkView の作成時に最初のメソッドが呼び出されます。
void App::Initialize(CoreApplicationView^ applicationView)
{
	// アプリ ライフサイクルのイベント ハンドラーを登録します。この例にはアクティブ化が含まれているため、
	// CoreWindow をアクティブにし、ウィンドウで描画を開始できます。
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

	//この時点では、デバイスにアクセスできます。
	// デバイスに依存するリソースを作成できます。
	m_deviceResources = std::make_shared<DX::DeviceResources>();


	


}

//CoreWindow オブジェクトが作成 (または再作成) されるときに呼び出されます。
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);


	window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &template1::App::OnKeyDown);
	window->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &template1::App::OnKeyUp);
	window->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &template1::App::OnPointerPressed);
	window->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &template1::App::OnPointerMoved);
	window->PointerReleased += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &template1::App::OnPointerReleased);
	window->CharacterReceived += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,Windows::UI::Core::CharacterReceivedEventArgs ^>(this,&template1::App::OnCharacterReceived);

	m_deviceResources->SetWindow(window);
	
	window->IsInputEnabled = true;
	

	// create ime context.
	Windows::UI::Text::Core::CoreTextServicesManager^ ctsMgr = CoreTextServicesManager::GetForCurrentView();
	ctsMgr->InputLanguageChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextServicesManager ^,Platform::Object ^>(this,&template1::App::OnInputLanguageChanged);

	CoreTextEditContext^ edcxt = ctsMgr->CreateEditContext();

	edcxt->CompositionStarted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs ^>(this,&template1::App::OnCompositionStarted);
	edcxt->CompositionCompleted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs ^>(this,&template1::App::OnCompositionCompleted);
	edcxt->FocusRemoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Platform::Object ^>(this,&template1::App::OnFocusRemoved);
	edcxt->LayoutRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs ^>(this,&template1::App::OnLayoutRequested);
	edcxt->SelectionRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs ^>(this,&template1::App::OnSelectionRequested);
	edcxt->SelectionUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs ^>(this,&template1::App::OnSelectionUpdating);
	edcxt->TextRequested += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextTextRequestedEventArgs ^>(this,&template1::App::OnTextRequested);
	edcxt->TextUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs ^>(this,&template1::App::OnTextUpdating);
	edcxt->FormatUpdating += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Text::Core::CoreTextEditContext ^,Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs ^>(this,&template1::App::OnFormatUpdating);

	imeBridge_.Set(edcxt);
}


// シーンのリソースを初期化するか、以前に保存したアプリ状態を読み込みます。
void App::Load(Platform::String^ entryPoint)
{
	if (m_main == nullptr)
	{
		m_main = std::unique_ptr<template1Main>(new template1Main(m_deviceResources, &imeBridge_));

		
	}
}

// このメソッドは、ウィンドウがアクティブになると、呼び出されます。
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			m_main->Update();

			if ( m_main->redraw_ )
			{
				if (m_main->Render())
					m_deviceResources->Present();
			}
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// IFrameworkView で必要です。
// 終了イベントでは初期化解除は呼び出されません。アプリケーションが前景に表示されている間に
//IFrameworkView クラスが解体されると呼び出されます。
void App::Uninitialize()
{
}

// アプリケーション ライフサイクル イベント ハンドラー。

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() は CoreWindow がアクティブ化されるまで起動されません。
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// 遅延を要求した後にアプリケーションの状態を保存します。遅延状態を保持することは、
	//中断操作の実行でアプリケーションがビジー状態であることを示します。
	//遅延は制限なく保持されるわけではないことに注意してください。約 5 秒後に、
	// アプリケーションは強制終了されます。
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
        m_deviceResources->Trim();

		// ここにコードを挿入します。

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// 中断時にアンロードされたデータまたは状態を復元します。既定では、データと状態は
	// 中断から再開するときに保持されます。このイベントは、アプリが既に終了されている場合は
	//発生しません。

	// ここにコードを挿入します。
}

// ウィンドウ イベント ハンドラー。 

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	Windows::Foundation::Size sz;
	sz.Width = sender->Bounds.Width;
	sz.Height = sender->Bounds.Height;
		
	m_deviceResources->SetLogicalSize(sz);
	m_main->CreateWindowSizeDependentResources();


	
}




void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

// DisplayInformation イベント ハンドラー。

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->SetDpi(sender->LogicalDpi);
	m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	m_deviceResources->ValidateDevice();
}

void template1::App::OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
	m_main->WndProc(0, WM_KEYDOWN, 0, args );
}
void template1::App::OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
	m_main->WndProc(0, WM_KEYUP, 0, args);
}

void template1::App::OnPointerPressed(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args)
{	
	m_main->WndProc(0,WM_LBUTTONDOWN, 0, args );

}
void template1::App::OnPointerMoved(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args)
{
	m_main->WndProc(0, WM_MOUSEMOVE, 0,args);
}
void template1::App::OnPointerReleased(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args)
{
	m_main->WndProc(0, WM_LBUTTONUP, 0, args);
}

void template1::App::OnCharacterReceived(Windows::UI::Core::CoreWindow ^sender,Windows::UI::Core::CharacterReceivedEventArgs ^args)
{
	m_main->WndProc(0, WM_CHAR, 0, args);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void template1::App::OnInputLanguageChanged(Windows::UI::Text::Core::CoreTextServicesManager ^sender,Platform::Object ^args)
{
	Windows::Globalization::Language^ lang = sender->InputLanguage;


	
}



Windows::Foundation::Rect ClientToScreen( FRectF rc )
{
	Windows::Foundation::Rect rcc = CoreWindow::GetForCurrentThread()->Bounds;

	rc.Offset( rcc.Left, rcc.Top );

	Windows::Foundation::Rect r;
	r.X = rc.left;
	r.Height = rc.bottom-rc.top;
	r.Y = rc.top;
	r.Width = rc.right - rc.left;
	return r;	
}

void template1::App::OnLayoutRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs ^args)
{
	if ( imeBridge_.info_ )
	{

		args->Request->LayoutBounds->TextBounds = ClientToScreen(imeBridge_.info_->global_rc);

		
		
	}

	TRACE( L"OnLayoutRequested\n" );
		
}


void template1::App::OnSelectionRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs ^args)
{
	args->Request->Selection = imeBridge_.GetSelection();

	TRACE( L"OnSelectionRequested\n" );
}

void template1::App::OnTextRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextTextRequestedEventArgs ^args)
{
	if ( imeBridge_.info_ )
	{

		LPCWSTR stra = imeBridge_.info_->text.c_str();
		args->Request->Text = ref new Platform::String( stra );
				

		::OutputDebugString( FString::Format( L"OnTextRequested, %s\n", stra ).c_str() );
	}
}

void template1::App::OnSelectionUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs ^args)
{
	imeBridge_.SetNewSelection( args->Selection );

	TRACE( L"OnSelectionUpdating\n" );
}

std::wstring str_remove( const std::wstring& str , int s, int e )
{
	if ( s == e )
		return str;

	_ASSERT( s < (int)str.length() );
	_ASSERT( e <= (int)str.length() );


	auto s1 = str.substr( 0, s );

	if ( str.length()-e > 0 )
	{
		auto s2 = str.substr( e, str.length()-e );
		return s1+s2;
	}
	return s1;
}
std::wstring str_append( const std::wstring& str , int s, WCHAR ch )
{
	_ASSERT( s < (int)str.length() );

	auto s1 = str.substr( 0, s );

	auto s2 = str.substr( s, str.length()-s );

	s1 += ch;

	return s1+s2;
}

void template1::App::OnTextUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs ^args)
{
	LPCWSTR c  = args->Text->Data();
	
	int append_len = wcslen(c);

	int ys = args->Range.StartCaretPosition;
	int ye = args->Range.EndCaretPosition;

	{
		auto str3 = str_remove( imeBridge_.info_->text, ys, ye );
		
		for( int i = 0; i < append_len; i++ )
		{
			if ( ys+i < (int)str3.length() )
				str3 = str_append(str3, ys+i, c[i] ); 
			else
			{
				str3 += c;
				break;
			}
		}

		imeBridge_.info_->text = str3;

		//::OutputDebugString( FString::Format( L"%s->add %s->%s\n", str2.c_str(), c, str3.c_str()).c_str() );
	}

	imeBridge_.SetNewSelection( args->NewSelection );


	//::OutputDebugString( FString::Format( L"%d %d, %s\n", edSelection_.StartCaretPosition, edSelection_.EndCaretPosition, c).c_str() );
}

void template1::App::OnFormatUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs ^args)
{	
	TRACE( L"OnFormatUpdating " );

	int typ = (int)args->UnderlineType->Value;

	//if ( args->UnderlineType == Windows::UI::Text::UnderlineType::Wave )
	//	typ = 1;
	
	if ( imeBridge_.info_ )
	{
		imeBridge_.info_->decoration_start_pos = args->Range.StartCaretPosition;
		imeBridge_.info_->decoration_end_pos = args->Range.EndCaretPosition;
		imeBridge_.info_->decoration_typ = typ;

	}

	::OutputDebugString( FString::Format( L"%d %d typ:%d\n", args->Range.StartCaretPosition, args->Range.EndCaretPosition, typ).c_str() );
	m_main->redraw_ = 1;
}

void template1::App::OnCompositionStarted(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs ^args)
{
	// start convert to jp.

	imeBridge_.CompositionStarted();

	TRACE( L"OnCompositionStarted\n" );

}
void template1::App::OnCompositionCompleted(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs ^args)
{
	// finished converting.

	imeBridge_.CompositionCompleted();

	TRACE( L"OnCompositionCompleted\n" );

}

void template1::App::OnFocusRemoved(Windows::UI::Text::Core::CoreTextEditContext ^sender,Platform::Object ^args)
{
		
}


