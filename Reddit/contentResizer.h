//
// contentResizer.h
// Declaration of the contentResizer class.
//

#pragma once

namespace Reddit
{
	public ref class contentResizer sealed : public Windows::UI::Xaml::Controls::ContentControl
	{
	public:
		contentResizer();
	protected:
		void OnApplyTemplate() override;
		void OnManipulationDelta(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs ^e);
		void OnPointerEntered(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
		void OnPointerExited(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e);
	private:
		void sizeChangedHandler(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		Windows::UI::Core::CoreCursor^ resizearrow;
		bool inIconmanipulation = false;
		void OnManipulationStarted(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationStartedRoutedEventArgs ^e);
		void OnManipulationCompleted(Platform::Object ^sender, Windows::UI::Xaml::Input::ManipulationCompletedRoutedEventArgs ^e);
	};
}
