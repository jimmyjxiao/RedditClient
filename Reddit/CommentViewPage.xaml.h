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
#include "NavStates.h"
namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommentViewPage sealed :  Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
		property account::subredditInfo subInfo
		{
			account::subredditInfo get()
			{
				return _subInfo;
			}
		}
		
		property account::commentSort sort
		{
			account::commentSort get();
			void set(account::commentSort a);
		}
		CommentViewPage();
		// Inherited via NavIndexed

	protected:
		virtual void OnNavigatedToPageCode() override;
		void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
	private:
		Windows::Foundation::Collections::IVector<account::reportReason>^ reportReasons;
		bool initialHtmlHelpersdone = false;
		Windows::Foundation::EventRegistrationToken unregisterToken;
		account::commentUWPlisting listing;
		std::queue<Windows::UI::Xaml::Documents::BlockCollection^> finishedHtmlBlocks;
		account::subredditInfo _subInfo;
		void sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnhtmlLoadingFinished(Platform::Object ^sender, Windows::UI::Xaml::Controls::RichTextBlock ^args);

	internal:
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get() override
			{
				return CommentViewPage::typeid;
			}
		}

		

		

	private:
		void subreddit_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
