//
// UserPage.xaml.h
// Declaration of the UserPage class
//

#pragma once
#include "NavStates.h"
#include "UserPage.g.h"
namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class UserPage sealed 
	{
	public:
		
		UserPage();
	protected:
		void OnNavigatedToPageCode() override;
	private:
	internal:
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get() override
			{
				return UserPage::typeid;
			}
		}
	};
}
