//
// UserPage.xaml.h
// Declaration of the UserPage class
//

#pragma once

#include "UserPage.g.h"
namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserPage sealed : NavIndexed
	{
	public:
		virtual property int NavigationIndex
		{
			int get()
			{
				return navIndex;
			}
		}
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get()
			{
				return UserPage::typeid;
			}
		}
		UserPage();
	protected:
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		unsigned int navIndex;
	};
}
