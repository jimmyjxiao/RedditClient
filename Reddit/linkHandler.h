#pragma once
#include <concurrent_unordered_map.h>
namespace Reddit
{
	ref class linkHandler sealed
	{
	internal:
		static concurrency::concurrent_unordered_map<int, concurrency::task<std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*>> > urlLookupTasks;
		static linkHandler ^ getInstance();
		static void linkClicked(Windows::UI::Xaml::Documents::Hyperlink^ s, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ e);
		void goToLink(Windows::Foundation::Uri^ link);
		void linkloaded(Windows::Foundation::Uri^ link, Windows::UI::Xaml::Documents::InlineCollection^ inlineCollection, unsigned int index, Windows::UI::Xaml::Documents::Hyperlink^ sender);
	private:
		static linkHandler ^ _inst;
		linkHandler();
		~linkHandler();
	};

}

