#pragma once
#include "subpostUWP.h"
namespace Reddit
{
	
	public ref class reportFlyout sealed : Windows::UI::Xaml::Controls::Flyout
	{
	private:
		Platform::String^ _sub;
	public:
		reportFlyout();
		property Platform::Object ^ reasonsVec
		{
			void set(Platform::Object^ r)
			{
				_reasons = static_cast<Platform::Collections::Vector<account::reportReason>^>(r);
			}
		}
		property Platform::String^ subreddit
		{
			Platform::String^ get() { return _sub; }
			void set(Platform::String^ a) { _sub = a; }
		}
		property bool isComment
		{
			void set(bool a)
			{
				if (a)
				{
					applic = account::reportApplicibility::comment;
				}
				else
					applic = account::reportApplicibility::subpost;
			}
		}
	private:
		void load();
		Platform::Collections::Vector<account::reportReason>^ _reasons;
		Windows::UI::Xaml::Controls::RadioButton^ textRadio;
		Windows::UI::Xaml::Controls::Button^ submitButton;
		void otherTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void radioChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^);
		account::reportApplicibility applic;
		bool loaded = false;
		Windows::UI::Xaml::Controls::TextBox^ textBox;
		void OnOpening(Platform::Object ^sender, Platform::Object ^args);
		Windows::UI::Xaml::Controls::StackPanel^ reasonsPanel;
		
	};
}