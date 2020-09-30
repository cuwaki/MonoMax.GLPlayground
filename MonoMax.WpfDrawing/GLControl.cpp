#include "GLControl.h"

using namespace System::Threading::Tasks;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Controls;

namespace SMGE
{
	GLControl::GLControl()
	{
	}

	GLControl::~GLControl()
	{
		Destroy();
	}

	void GLControl::OnRenderSizeChanged(System::Windows::SizeChangedInfo^ info)
	{
		m_width = (int)info->NewSize.Width;
		m_height = (int)info->NewSize.Height;

		if (!m_isInitialized)
		{
			double wantFPS = 1000. / 60.;

			Grid^ mainGrid = gcnew Grid();
			m_textFpsCounter = gcnew TextBlock();
			m_textFpsCounter->Margin = Thickness(3);
			m_textFpsCounter->VerticalAlignment = System::Windows::VerticalAlignment::Bottom;
			

			m_lastUpdate = System::DateTime::Now;

			m_tickTimer = gcnew System::Windows::Threading::DispatcherTimer(System::Windows::Threading::DispatcherPriority::Send);
			m_tickTimer->Interval = System::TimeSpan::FromMilliseconds(wantFPS * 0.95);	// 렌더보다 약간 빨리 돌게
			m_tickTimer->Tick += gcnew System::EventHandler(this, &SMGE::GLControl::Tick);
			m_tickTimer->Start();

			m_renderTimer = gcnew System::Windows::Threading::DispatcherTimer(System::Windows::Threading::DispatcherPriority::Send);
			m_renderTimer->Interval = System::TimeSpan::FromMilliseconds(wantFPS);
			m_renderTimer->Tick += gcnew System::EventHandler(this, &SMGE::GLControl::Render);
			m_renderTimer->Start();

			m_ImageControl = gcnew Image();
			m_ImageControl->RenderTransformOrigin = Point(0.5, 0.5);
			m_ImageControl->RenderTransform = gcnew ScaleTransform(1.0, -1.0);
			
			mainGrid->Children->Add(m_ImageControl);
			mainGrid->Children->Add(m_textFpsCounter);

			AddChild(mainGrid);
			
			System::Windows::Controls::Panel::SetZIndex(m_ImageControl, -1);

			m_graphicsEngine = new nsRE::CRenderingEngine();
			m_graphicsEngine->Init();
			m_isInitialized = true;
		}

		m_graphicsEngine->Resize(m_width, m_height);

		double dpiX, dpiY;
		int colorDepth;
		m_graphicsEngine->getWriteableBitmapInfo(dpiX, dpiY, colorDepth);
		
		// colorDepth == 4 이어야하고 그래서 PixelFormats::Pbgra32 를 쓴다
		m_writeableImg = gcnew WriteableBitmap(m_width, m_height, dpiX, dpiY, PixelFormats::Pbgra32, nullptr);

		m_WriteableBuffer = (char*)m_writeableImg->BackBuffer.ToPointer();
		m_ImageControl->Source = m_writeableImg;
	}

	bool GLControl::GetIsRunning(void)
	{
		return m_isRunning;
	}

	void GLControl::Destroy(void)
	{
		m_graphicsEngine->DeInit();
		delete m_graphicsEngine;
		m_graphicsEngine = nullptr;
	}

	void GLControl::UpdateImageData(void)
	{
		m_writeableImg->Lock();
		m_writeableImg->AddDirtyRect(Int32Rect(0, 0, m_width, m_height));
		m_writeableImg->Unlock();
	}

	void GLControl::Tick(System::Object^ sender, System::EventArgs^ e)
	{
		m_graphicsEngine->Tick();
	}

	void GLControl::Render(System::Object^ sender, System::EventArgs^ e)
	{
		System::TimeSpan elapsed = (System::DateTime::Now - m_lastUpdate);
		if (elapsed.TotalMilliseconds >= 1000)
		{
			m_textFpsCounter->Text = "FPS= " + m_fpsCounter.ToString();
			m_fpsCounter = 0;
			m_lastUpdate = System::DateTime::Now;
		}

		m_graphicsEngine->Render(m_WriteableBuffer);
		m_ImageControl->Dispatcher->Invoke(gcnew System::Action(this, &GLControl::UpdateImageData));

		m_fpsCounter++;
	}
}
