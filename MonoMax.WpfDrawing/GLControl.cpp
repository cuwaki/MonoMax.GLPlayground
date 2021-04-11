#include "GLControl.h"

using namespace System::Threading::Tasks;
using namespace System::Windows;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Controls;

//#define SINGLE_THREADING 1

float GetWindowDPIRate();

namespace SMGE
{
	GLControl::GLControl()
	{
		//auto m_hwnd = ::GetForegroundWindow();
		//this->Loaded += gcnew System::Windows::RoutedEventHandler(this, &GLControl::OnLoaded);
	}

	GLControl::~GLControl()
	{
		Destroy();
	}

	void GLControl::OnRenderSizeChanged(System::Windows::SizeChangedInfo^ info)
	{
		m_widthOriginal = (int)info->NewSize.Width;
		m_heightOriginal = (int)info->NewSize.Height;
		m_widthWindowDPI = m_widthOriginal * GetWindowDPIRate();
		m_heightWindowDPI = m_heightOriginal * GetWindowDPIRate();

		if (!m_isInitialized)
		{
			double wantFPS = 1000. / 60.;

			Grid^ mainGrid = gcnew Grid();
			m_textFpsCounter = gcnew TextBlock();
			m_textFpsCounter->Margin = Thickness(3);
			m_textFpsCounter->VerticalAlignment = System::Windows::VerticalAlignment::Bottom;

			m_ImageControl = gcnew Image();
			m_ImageControl->RenderTransformOrigin = Point(0.5, 0.5);
			m_ImageControl->RenderTransform = gcnew ScaleTransform(1.0, -1.0);
			
			mainGrid->Children->Add(m_ImageControl);
			mainGrid->Children->Add(m_textFpsCounter);

			AddChild(mainGrid);
			
			System::Windows::Controls::Panel::SetZIndex(m_ImageControl, -1);

			/////////////////////////////////
			m_lastUpdate = System::DateTime::Now;

			m_tickTimer = gcnew System::Windows::Threading::DispatcherTimer(System::Windows::Threading::DispatcherPriority::Send);
			m_tickTimer->Interval = System::TimeSpan::FromMilliseconds(wantFPS * 0.95);	// 렌더보다 약간 빨리 돌게
			m_tickTimer->Tick += gcnew System::EventHandler(this, &SMGE::GLControl::Tick);
			m_tickTimer->Start();

#ifdef SINGLE_THREADING
#else
			m_renderTimer = gcnew System::Windows::Threading::DispatcherTimer(System::Windows::Threading::DispatcherPriority::Send);
			m_renderTimer->Interval = System::TimeSpan::FromMilliseconds(wantFPS);
			m_renderTimer->Tick += gcnew System::EventHandler(this, &SMGE::GLControl::Render);
			m_renderTimer->Start();
#endif

			/////////////////////////////////
			m_renderingEngine = new nsRE::CRenderingEngine(m_widthOriginal, m_heightOriginal, GetWindowDPIRate());	// Managed 라서 std::unique_ptr 못씀
			m_renderingEngine->Init();
			m_isInitialized = true;
		}

		m_renderingEngine->Resize(m_widthOriginal, m_heightOriginal);

#if IS_EDITOR
		double dpiX, dpiY;
		int colorDepth;
		m_renderingEngine->getWriteableBitmapInfo(dpiX, dpiY, colorDepth);
		
		// colorDepth == 4 이어야하고 그래서 PixelFormats::Pbgra32 를 쓴다
		// GL_ColorType == GL_BGRA 여야한다
		m_writeableImg = gcnew WriteableBitmap(m_widthWindowDPI, m_heightWindowDPI, dpiX, dpiY, PixelFormats::Pbgra32, nullptr);

		m_WriteableBuffer = (char*)m_writeableImg->BackBuffer.ToPointer();
		m_ImageControl->Source = m_writeableImg;
#endif
	}

	bool GLControl::GetIsRunning(void)
	{
		return m_isRunning;
	}

	void GLControl::Destroy(void)
	{
		m_renderingEngine->DeInit();
		delete m_renderingEngine;
		m_renderingEngine = nullptr;
	}

	void GLControl::UpdateImageData(void)
	{
		m_writeableImg->Lock();
		m_writeableImg->AddDirtyRect(Int32Rect(0, 0, m_widthWindowDPI, m_heightWindowDPI));
		m_writeableImg->Unlock();
	}

	//void GLControl::OnLoaded(System::Object^ sender, System::Windows::RoutedEventArgs^ e)
	//{
	//	//auto helper = gcnew System::Windows::WindowInteropHelper(this);
	//	//System::IntPtr ptr = helper.Handle;

	//	volatile int xxx = 0;
	//	//m_hwnd = (HWND)this->Handle.ToPointer();
	//	return;
	//}

	void GLControl::Tick(System::Object^ sender, System::EventArgs^ e)
	{
		m_renderingEngine->Tick();
#ifdef SINGLE_THREADING
		Render(sender, e);
#endif
	}

	void GLControl::Render(System::Object^ sender, System::EventArgs^ e)
	{
#ifdef SINGLE_THREADING
#else
		System::TimeSpan elapsed = (System::DateTime::Now - m_lastUpdate);
		if (elapsed.TotalMilliseconds >= 1000)
		{
			m_textFpsCounter->Text = "FPS= " + m_fpsCounter.ToString();
			m_fpsCounter = 0;
			m_lastUpdate = System::DateTime::Now;
		}

		m_renderingEngine->Render(m_WriteableBuffer);
		m_ImageControl->Dispatcher->Invoke(gcnew System::Action(this, &GLControl::UpdateImageData));

		m_fpsCounter++;
#endif
	}
}

float GetWindowDPIRate()
{
	/*
	* https://stackoverflow.com/questions/54912038/querying-windows-display-scaling

	이걸로 dpi 제대로 구해진다고 하는데 나는 안된다 어쩔수 없이 일단 하드코딩함

	auto activeWindow = GetForegroundWindow();//GetActiveWindow();
	HMONITOR monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTOPRIMARY);

	// Get the logical width and height of the monitor
	MONITORINFOEX monitorInfoEx;
	monitorInfoEx.cbSize = sizeof(monitorInfoEx);
	GetMonitorInfo(monitor, &monitorInfoEx);
	auto cxLogical = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
	auto cyLogical = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;

	// Get the physical width and height of the monitor
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettings(monitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
	auto cxPhysical = devMode.dmPelsWidth;
	auto cyPhysical = devMode.dmPelsHeight;

	// Calculate the scaling factor
	auto horizontalScale = ((double)cxPhysical / (double)cxLogical);
	auto verticalScale = ((double)cyPhysical / (double)cyLogical);

	std::cout << "Horizonzal scaling: " << horizontalScale << "\n";
	std::cout << "Vertical scaling: " << verticalScale;
	*/

	const auto hwnd = ::GetForegroundWindow();	// hwnd 는 실행 시점에 따라 바뀔 수 있다. 이 코드를 작성한 상황에서는 desktop 이었다.

	auto dpi = ::GetDpiForWindow(hwnd);
	switch (dpi)
	{
	case 96:	return 1.f;
	case 120:	return 1.25f;
	case 144:	return 1.5f;
	case 192:	return 2.f;
	}

	assert(false);
	return 0.f;
}
