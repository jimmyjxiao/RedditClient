#include "pch.h"
#include "linkHandler.h"
using namespace Reddit;
linkHandler^ Reddit::linkHandler::_inst = nullptr;
linkHandler ^ Reddit::linkHandler::getInstance()
{
	if (_inst == nullptr)
	{
		_inst = ref new linkHandler;
	}
	return _inst;
}
void Reddit::linkHandler::linkClicked(Windows::UI::Xaml::Documents::Hyperlink ^ s, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^ e)
{
	
	//Windows::System::Launcher::LaunchUriAsync(data->getUri());
}

void Reddit::linkHandler::goToLink(Windows::Foundation::Uri ^ link)
{
	if (link->Domain == L"reddit.com")
	{

	}
}
concurrency::concurrent_unordered_map<int, concurrency::task<std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*>> > Reddit::linkHandler::urlLookupTasks;
void setToBlock(Windows::UI::Xaml::Documents::InlineCollection ^ const &  inlineCollection, unsigned int index, std::pair<account::postContentType,account::serviceHelpers::previewHelperbase*> const & res)
{
	auto text = ref new Windows::UI::Xaml::Documents::InlineUIContainer();
	
	try { inlineCollection->InsertAt(index, text); }
	catch (...)
	{
		if (inlineCollection->Size == index)
		{
			inlineCollection->Append(text);
		}
	}

}
void Reddit::linkHandler::linkloaded(Windows::Foundation::Uri^ link, Windows::UI::Xaml::Documents::InlineCollection ^ inlineCollection, unsigned int index, Windows::UI::Xaml::Documents::Hyperlink^ sender)
{
	sender->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^>(&linkClicked);
	auto it = urlLookupTasks.find(link->GetHashCode());
	if (it != urlLookupTasks.end())
	{
		if (it->second.is_done())
		{
			setToBlock(inlineCollection, index, it->second.get());
		}
		else
		{
			it->second.then([inlineCollection, index](std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*>  result) {
				Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
				Windows::UI::Core::CoreDispatcherPriority::Normal,
				ref new Windows::UI::Core::DispatchedHandler([inlineCollection, index, res= std::move(result) ]()
			{
					setToBlock(inlineCollection, index, res);
			})); });
			
		}
	}
	else
	{
		
	}
}

linkHandler::linkHandler()
{
	mdblock::mdRichProperties::LinkLoaded += ref new mdblock::linkLoadedeventhandler(this, &linkHandler::linkloaded);
	mdblock::mdRichProperties::RedditLinkClicked += ref new mdblock::RedditLinkClickedEventHandler([](Platform::String^ name, bool type) {

	});
}


linkHandler::~linkHandler()
{
}



