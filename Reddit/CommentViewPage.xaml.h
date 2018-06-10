//
// CommentViewPage.xaml.h
// Declaration of the CommentViewPage class
//

#pragma once
#include "AccountInterface.h"
#include "CommentView.xaml.h"
#include "CommentViewPage.g.h"
#include "subpostUWP.h"
#include "converters.h"
namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommentViewPage sealed : NavIndexed
	{
	public:
		property account::subredditInfo subInfo
		{
			account::subredditInfo get()
			{
				return _subInfo;
			}
		}
		// Inherited via NavIndexed
		virtual property int NavigationIndex
		{
			int get()
			{
				return navIndex;
			}
		}
		property account::commentSort sort
		{
			account::commentSort get() { return nav->sort; }
			void set(account::commentSort a);
		}
		CommentViewPage();
		// Inherited via NavIndexed
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get()
			{
				return CommentViewPage::typeid;
			}
		}
	protected:
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
	private:
		int navIndex; 
		Windows::Foundation::Collections::IVector<account::reportReason>^ reportReasons;
		bool initialHtmlHelpersdone = false;
		Windows::Foundation::EventRegistrationToken unregisterToken;
		account::commentUWPlisting listing;
		std::queue<Windows::UI::Xaml::Documents::BlockCollection^> finishedHtmlBlocks;
		account::subredditInfo _subInfo;
		commentNavstate * nav;
		void sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnhtmlLoadingFinished(Platform::Object ^sender, Windows::UI::Xaml::Controls::RichTextBlock ^args);

		

		
	};
}
