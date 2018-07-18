//
// PopupContent.cpp
// Implementation of the PopupContent class.
//

#include "pch.h"
#include "PopupContent.h"

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

void Reddit::PopupContent::OpenPreviewPopup(account::serviceHelpers::previewHelperbase * ContentHelper)
{
	auto popup = ref new Windows::UI::Xaml::Controls::Primitives::Popup();
	popup->LightDismissOverlayMode = LightDismissOverlayMode::On;
	popup->IsLightDismissEnabled = true;
	popup->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	popup->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	popup->Child = ref new Reddit::PopupContent(std::move(ContentHelper));
	popup->IsOpen = true;
}

void Reddit::PopupContent::OnApplyTemplate()
{
	static_cast<Button^>(GetTemplateChild("CloseButton"))->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Reddit::PopupContent::OnCloseButtonClick);
	static_cast<RelativePanel^>(GetTemplateChild("rootGrid"))->Loading += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::FrameworkElement ^, Platform::Object ^>(this, &Reddit::PopupContent::OnGridLoading);
}

PopupContent::PopupContent(account::serviceHelpers::previewHelperbase * ContentHelper)
{
	DefaultStyleKey = "Reddit.PopupContent";
	this->Content = ContentHelper->viewerControl();
}


void Reddit::PopupContent::OnCloseButtonClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	safe_cast<Windows::UI::Xaml::Controls::Primitives::Popup^>(Parent)->IsOpen = false;
}


void Reddit::PopupContent::OnGridLoading(Windows::UI::Xaml::FrameworkElement ^sender, Platform::Object ^args)
{
	const auto &grid = static_cast<RelativePanel^>(sender);
	grid->Height = Windows::UI::Xaml::Window::Current->Bounds.Height;
	grid->Width = Windows::UI::Xaml::Window::Current->Bounds.Width;
	const auto &presenter = static_cast<ContentPresenter^>(static_cast<RelativePanel^>(sender)->Children->GetAt(0));
	presenter->MaxHeight = Windows::UI::Xaml::Window::Current->Bounds.Height - Reddit::rootWindowGrid::getCurrent()->titleRowDef->ActualHeight;
	presenter->MaxWidth = Windows::UI::Xaml::Window::Current->Bounds.Width;
}
