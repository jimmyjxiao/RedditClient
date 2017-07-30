//
// subpostContentResizingPlaceholder.xaml.h
// Declaration of the subpostContentResizingPlaceholder class
//

#pragma once

#include "subpostContentResizingPlaceholder.g.h"
#include "subpostUWP.h"
namespace Reddit
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class subpostContentResizingPlaceholder sealed
	{
	public:
		subpostContentResizingPlaceholder();
		property account::subpostUWP^ post
		{
			void set(account::subpostUWP^ s)
			{
				contentHelper = s->contentHelper.get();
			}
		}
		void activatecontent();
	private:
		account::serviceHelpers::previewHelperbase* contentHelper = nullptr;
		void onParentExpanded(Platform::Object^, Platform::Object^);
		void UserControl_ManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e);
		bool contentIntialized = false;
	};
}
