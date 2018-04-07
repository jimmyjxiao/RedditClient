//
// contentResizer.cpp
// Implementation of the contentResizer class.
//

#include "pch.h"
#include "contentResizer.h"
#include "rootWindowGrid.xaml.h"
using namespace Reddit;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;

// The Templated Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234235

contentResizer::contentResizer()
{
	DefaultStyleKey = "Reddit.contentResizer";
	resizearrow = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::SizeNorthwestSoutheast, 0);
}

void Reddit::contentResizer::OnApplyTemplate()
{
	ManipulationDelta += ref new Windows::UI::Xaml::Input::ManipulationDeltaEventHandler(this, &Reddit::contentResizer::OnManipulationDelta);
	const auto &page = rootWindowGrid::getCurrent();
	static_cast<Windows::UI::Xaml::FrameworkElement^>(Content)->MaxWidth = page->ActualWidth - 300;
	page->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Reddit::contentResizer::sizeChangedHandler);
	auto icon = static_cast<FontIcon^>(GetTemplateChild("resizer"));
	icon->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Reddit::contentResizer::OnPointerEntered);
	icon->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Reddit::contentResizer::OnPointerExited);
	icon->ManipulationStarted += ref new Windows::UI::Xaml::Input::ManipulationStartedEventHandler(this, &Reddit::contentResizer::OnManipulationStarted);
	icon->ManipulationCompleted += ref new Windows::UI::Xaml::Input::ManipulationCompletedEventHandler(this, &Reddit::contentResizer::OnManipulationCompleted);
}


void Reddit::contentResizer::OnManipulationDelta(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^e)
{
	if (inIconmanipulation)
	{
		Window::Current->CoreWindow->PointerCursor = resizearrow;
	}
	float scl = e->Delta.Scale - 1;
	if ((e->Delta.Translation.X != 0 || e->Delta.Translation.Y !=0) && scl == 0)
	{
		scl = (e->Delta.Translation.X + e->Delta.Translation.Y) / 1000;
	}
	const auto &x = static_cast<Windows::UI::Xaml::FrameworkElement^>(Content);
	if (scl > 0)
	{
		if (x->Width >= x->MaxWidth);
		else
		{
			goto doz;
		}
	}
	else
	{
		doz:
		x->Width = (x->ActualWidth) + (scl * x->ActualWidth);
		x->Height = (x->ActualHeight) + (scl * x->ActualHeight);
	}
	

}


void Reddit::contentResizer::OnPointerEntered(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	Window::Current->CoreWindow->PointerCursor = resizearrow;
}


void Reddit::contentResizer::OnPointerExited(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	if(!inIconmanipulation)
		Window::Current->CoreWindow->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
}

void Reddit::contentResizer::sizeChangedHandler(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e)
{
	static_cast<Windows::UI::Xaml::FrameworkElement^>(Content)->MaxWidth = e->NewSize.Width - 300;
}


void Reddit::contentResizer::OnManipulationStarted(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs ^e)
{
	inIconmanipulation = true;
}


void Reddit::contentResizer::OnManipulationCompleted(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs ^e)
{
	inIconmanipulation = false;
	Window::Current->CoreWindow->PointerCursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
}
