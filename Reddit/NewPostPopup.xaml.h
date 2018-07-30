//
// NewPostPopup.xaml.h
// Declaration of the NewPostPopup class
//

#pragma once

#include "NewPostPopup.g.h"
#include "ReplyBox.xaml.h"
#include "converters.h"
namespace Reddit
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class NewPostPopup sealed
	{
	public:
		NewPostPopup();
		NewPostPopup(Platform::String^ s);
		property Platform::String^ Subreddit;
		property Windows::Foundation::Uri^ url;
		property Platform::String^ md;
		property Platform::String^ PostTitle;
		property bool NSFW;
		property bool Spoiler;
		bool SubmitEnabled();
	private:
		bool sub_is_fine;
		Platform::String^ deducedName;
		void updateDeducedName();
		void ContentDialog_PrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void ContentDialog_SecondaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void title_changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void ContentDialog_Opened(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogOpenedEventArgs^ args);
		void subbox_PreviewKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void WYSIWYG_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args);
		void WYSIWYG_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void mdEditor_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void mdEditor_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void subbox_BeforeTextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs^ args);
		void subbox_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
	};
}
