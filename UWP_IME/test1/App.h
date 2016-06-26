#pragma once 
 
#include "pch.h"
#include "Common\DeviceResources.h"
#include "template1Main.h"

namespace template1
{
	// アプリのメイン エントリ ポイントです。Windows シェルでアプリを接続し、アプリケーション ライフサイクル イベントを処理します。
	ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		App();

		// IFrameworkView メソッド。
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
		virtual void Load(Platform::String^ entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	protected:
		// アプリケーション ライフサイクル イベント ハンドラー。
		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnResuming(Platform::Object^ sender, Platform::Object^ args);

		// ウィンドウ イベント ハンドラー。
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
		void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

		// DisplayInformation イベント ハンドラー。
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<template1Main> m_main;


		bool m_windowClosed;
		bool m_windowVisible;
		void OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
		void OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
		
		void OnPointerMoved(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
		void OnPointerReleased(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
		void OnPointerPressed(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args);
		void OnCharacterReceived(Windows::UI::Core::CoreWindow ^sender,Windows::UI::Core::CharacterReceivedEventArgs ^args);

		void OnInputLanguageChanged(Windows::UI::Text::Core::CoreTextServicesManager ^sender,Platform::Object ^args);
		 void OnCompositionStarted(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextCompositionStartedEventArgs ^args);
		 void OnCompositionCompleted(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextCompositionCompletedEventArgs ^args);
		 void OnFocusRemoved(Windows::UI::Text::Core::CoreTextEditContext ^sender,Platform::Object ^args);
		 void OnLayoutRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextLayoutRequestedEventArgs ^args);
		 void OnSelectionRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextSelectionRequestedEventArgs ^args);
		 void OnSelectionUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextSelectionUpdatingEventArgs ^args);
		 void OnTextRequested(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextTextRequestedEventArgs ^args);
		 void OnTextUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextTextUpdatingEventArgs ^args);
		 void OnFormatUpdating(Windows::UI::Text::Core::CoreTextEditContext ^sender,Windows::UI::Text::Core::CoreTextFormatUpdatingEventArgs ^args);

		 
		D2CoreTextBridge imeBridge_;
	};
}

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
