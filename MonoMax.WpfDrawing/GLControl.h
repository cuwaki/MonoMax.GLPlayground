#pragma once

#pragma managed(push, off)
#include "../MonoMax.EngineCore/EngineCore.h"
#pragma managed(pop)

// https://www.codeproject.com/Articles/23736/Creating-OpenGL-Windows-in-WPF

namespace SMGE
{
	public ref class GLControl : public System::Windows::Controls::UserControl
	{
	private:
		nsRE::CRenderingEngine* m_renderingEngine;

		bool m_isInitialized = false;
		int m_fpsCounter;
		int m_widthOriginal, m_heightOriginal;
		int m_widthWindowDPI, m_heightWindowDPI;

		System::Windows::Threading::DispatcherTimer^ m_tickTimer;
		System::Windows::Threading::DispatcherTimer^ m_renderTimer;

		System::Windows::Controls::TextBlock^ m_textFpsCounter;
		System::DateTime m_lastUpdate;

		char* m_WriteableBuffer;
		bool m_isRunning;
		
		System::Windows::Controls::Image^ m_ImageControl;
		System::Windows::Media::Imaging::WriteableBitmap^ m_writeableImg;

		void Tick(System::Object^ sender, System::EventArgs^ e);
		void Render(System::Object^ sender, System::EventArgs^ e);
		void UpdateImageData();

	protected:
		//void OnLoaded(System::Object^ sender, System::Windows::RoutedEventArgs^ e);

	public:
		GLControl();
		~GLControl();

		bool GetIsRunning(void);
		void Destroy(void);

		// overrides
		void OnRenderSizeChanged(System::Windows::SizeChangedInfo^ info) override;
	};
}

