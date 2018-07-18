#include "pch.h"
#include "linkHandler.h"
#include "globalvars.h"
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
	

}
void Reddit::linkHandler::linkloaded(Windows::Foundation::Uri^ link, Windows::UI::Xaml::Documents::InlineCollection ^ inlineCollection, unsigned int index, Windows::UI::Xaml::Documents::Hyperlink^ sender)
{
	sender->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^>(&linkClicked);
	auto it = urlLookupTasks.find(link->GetHashCode());
	if (it != urlLookupTasks.end())
	{
		auto addButton = [inlineCollection = std::move(inlineCollection), index](std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*> p) {
			if (p.first != account::postContentType::linktype)
			{
				auto text = ref new Windows::UI::Xaml::Documents::InlineUIContainer();
				auto exp = ref new ExpanderControl::Expander();
				exp->MinHeight = 24;
				exp->MinWidth = 24;
				exp->Template = static_cast<Windows::UI::Xaml::Controls::ControlTemplate^>(static_cast<App^>(App::Current)->listTemplateRes->Lookup("inlineExpanderButtoning"));
				exp->Content = ref new EXplaceHolder(p.second);
				/*auto rec = ref new Windows::UI::Xaml::Shapes::Rectangle();
				rec->Fill = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
				rec->Width = 300;
				rec->Height = 300;
				exp->Content = std::move(rec);*/
				text->Child = std::move(exp);
				try {
					inlineCollection->InsertAt(index, text);
				}
				catch (...)
				{
					if (inlineCollection->Size == index)
					{
						inlineCollection->Append(text);
					}
				}
			}
		};
		if (it->second.is_done())
		{
			addButton(it->second.get());
		}
		else
		{
			it->second.then(addButton);
		}
	}
	//if (it != urlLookupTasks.end())
	//{
	//	__debugbreak();
	//		//auto text = ref new Windows::UI::Xaml::Documents::InlineUIContainer();
	//		//auto exp = ref new ExpanderControl::Expander();
	//		//exp->Content = ref new EXplaceHolder(&it->second);
	//		//try {
	//		//	inlineCollection->InsertAt(index, text);
	//		//}
	//		//catch (...)
	//		//{
	//		//	if (inlineCollection->Size == index)
	//		//	{
	//		//		inlineCollection->Append(text);
	//		//	}
	//		//}
	//		///*it->second.then([inlineCollection, index](std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*>  result) {
	//		//	Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
	//		//	Windows::UI::Core::CoreDispatcherPriority::Normal,
	//		//	ref new Windows::UI::Core::DispatchedHandler([inlineCollection, index, res= std::move(result) ]()
	//		//{
	//		//	
	//		//})); });*/
	//}
	//else
	//{
	//	__debugbreak();
	//}
	/*auto text = ref new Windows::UI::Xaml::Documents::Run();
	text->Text = L"🔸🔸🔸";*/
	//auto text = ref new Windows::UI::Xaml::Documents::Hyperlink();
	//auto button = ref new Windows::UI::Xaml::Controls::HyperlinkButton();
	///*button->Click += ref new Windows::UI::Xaml::RoutedEventHandler([x = it->second.get().second](Platform::Object^, Platform::Object^) {
	//	auto dialog = ref new Windows::UI::Xaml::Controls::ContentDialog();
	//	dialog->Content = x->viewerControl();
	//	dialog->ShowAsync();
	//});*/
	///*button->PointerEntered += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Reddit::linkHandler::OnPointerEntered);
	//button->PointerExited += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &Reddit::linkHandler::OnPointerExited);*/
	//button->Height = 40;
	//button->Width = 40;
	//text->Child = std::move(button);
	
}
const bool RedditTypeUser = 0;
const bool RedditTypeSubreddit = 1;
linkHandler::linkHandler()
{
	mdblock::mdRichProperties::LinkLoaded += ref new mdblock::linkLoadedeventhandler(this, &linkHandler::linkloaded);
	mdblock::mdRichProperties::RedditLinkClicked += ref new mdblock::RedditLinkClickedEventHandler([](Platform::String^ name, bool type) {
		if (type == RedditTypeSubreddit)
		{
			
		}
		else
		{

		}
	});
}


linkHandler::~linkHandler()
{
}





void Reddit::linkHandler::OnPointerEntered(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	e->Handled = true;
	const auto &butt = static_cast<Windows::UI::Xaml::Controls::Button^>(sender);
	butt->Tag = Windows::UI::Xaml::Window::Current->CoreWindow->PointerCursor;
	static auto arrowcursor = ref new Windows::UI::Core::CoreCursor(Windows::UI::Core::CoreCursorType::Arrow, 0);
	Windows::UI::Xaml::Window::Current->CoreWindow->PointerCursor = arrowcursor;
}


void Reddit::linkHandler::OnPointerExited(Platform::Object ^sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^e)
{
	/*const auto &butt = static_cast<Windows::UI::Xaml::Controls::Button^>(sender);
	if (butt->Tag != nullptr)
	{
		Windows::UI::Xaml::Window::Current->CoreWindow->PointerCursor = static_cast<Windows::UI::Core::CoreCursor^>(butt->Tag);
	}
	else
	{
		__debugbreak();
	}*/
}
