//
// CommentViewPage.xaml.h
// Declaration of the CommentViewPage class
//

#pragma once
#include "AccountInterface.h"
#include "CommentView.xaml.h"
#include "NavStates.h"
#include "CommentViewPage.g.h"

namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommentViewPage sealed
	{
	public:
		CommentViewPage();
	protected:
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		commentNavstate * nav;
		void sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
	};
}
