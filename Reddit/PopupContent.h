//
// PopupContent.h
// Declaration of the PopupContent class.
//

#pragma once
#include "serviceHelpers.h"
namespace Reddit
{
	public ref class PopupContent sealed : public Windows::UI::Xaml::Controls::ContentControl
	{
	protected:
		void OnApplyTemplate() override;
	internal:
		static void OpenPreviewPopup(account::serviceHelpers::previewHelperbase * ContentHelper);
		PopupContent(account::serviceHelpers::previewHelperbase * ContentHelper);
		void OnCloseButtonClick(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	private:
		Windows::UI::Xaml::Controls::Grid^ rootGrid;
		void OnGridLoading(Windows::UI::Xaml::FrameworkElement ^sender, Platform::Object ^args);
	};
}
