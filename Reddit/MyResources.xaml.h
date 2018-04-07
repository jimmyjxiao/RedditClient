//
// MyResources.xaml.h
// Declaration of the MyResources class
//

#pragma once
#include "NoToggleBehaviorButton.h"
#include "MyResources.g.h"
#include "subpostUWP.h"
#include "AccountInterface.h"
#include "converters.h"
#include "contentResizer.h"
#include "reportFlyout.h"
namespace Reddit
{
	public ref class subPostControlIconTemplateSelector sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		property Windows::UI::Xaml::Controls::ControlTemplate^ selfTemplate;
		property Windows::UI::Xaml::Controls::ControlTemplate^ thumbnailTemplate;
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			if (static_cast<bool>(value))
				return selfTemplate;
			else
				return thumbnailTemplate;

		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException;
		}
	};
	public ref class AppBarToggleColoredButton sealed : Windows::UI::Xaml::Controls::AppBarToggleButton
	{
	private:
		static Windows::UI::Xaml::DependencyProperty^ _brushProp;
	internal:
		static void registerDependencyProperty();
	public:
		AppBarToggleColoredButton()
		{
			DefaultStyleKey = "Reddit.AppBarToggleColoredButton";
		}
		property Windows::UI::Xaml::Media::Brush^ CheckedBrush
		{
			Windows::UI::Xaml::Media::Brush^ get();
			void set(Windows::UI::Xaml::Media::Brush^ s);
		}
		static property Windows::UI::Xaml::DependencyProperty^ CheckedBrushProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return _brushProp;
			}
		}
	};
	public ref class EXplaceHolder sealed : Windows::UI::Xaml::Controls::ContentControl
	{
	private:
		bool _load = false;
		Platform::WeakReference s;
	public:
		property bool contentLoaded {
			bool get()
			{
				return _load;
			}
			void set(bool a)
			{
				_load = a;
			}
		}
		EXplaceHolder();
		property account::subpostUWP^ post
		{
			void set(account::subpostUWP^ sa)
			{
				s = sa;
			}
		}
	internal:
		
		void OnExpanding(Platform::Object^, Platform::Object^);
		void  OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MyResources sealed
	{
	public:
		MyResources();
	private:
		void contentExpander_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args);
		void contentExpander_Loading(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBlock_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ContentPresenter_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args);
		void AppBarButton_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void replyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void username_click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
		void subreddit_click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
	};
}
