//
// TestPage.xaml.h
// Declaration of the TestPage class
//

#pragma once

#include "TestPage.g.h"

namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TestPage sealed
	{
	public:
		TestPage();
	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
