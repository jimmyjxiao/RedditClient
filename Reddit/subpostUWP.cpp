#include "pch.h"
#include "subpostUWP.h"
#include "globalvars.h"
#include "DelegateCommand.h"
#include <regex>
#include "ApplicationDataHelper.h"
#include "App.xaml.h"
#include "PopupContent.h"
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
namespace wsaw = Windows::Security::Authentication::Web;
using namespace account;


	void account::subpostUWP::Liked::set(int input)
	{
		if (input != helper.getMyVote())
		{
			helper.vote(input); //Todo: error handling
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Liked"));
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("score"));
		}
	}
	void account::subpostUWP::_changeupvote(Platform::Object ^ param)
	{
		int cvote = helper.getMyVote();
		if (cvote == 0 || cvote == 1)
			Liked = true;
		else if (cvote == 1)
			Liked = 0;
	}

	void account::subpostUWP::_changedownvote(Platform::Object ^ param)
	{
		//__debugbreak();
		int cvote = helper.getMyVote();
		if (cvote == 0 || cvote == 1)
			Liked = -1;
		else if (cvote == -1)
			Liked = 0;
	}

	void account::subpostUWP::previewDialog(Platform::Object ^)
	{
		
		/*dialog->PrimaryButtonText = "Go to Comments";
		dialog->CloseButtonText = "Close";*/
		if (std::holds_alternative<std::unique_ptr<account::serviceHelpers::previewHelperbase>>(contentHelper))
		{
		nonC:
			Reddit::PopupContent::OpenPreviewPopup(std::get<std::unique_ptr<account::serviceHelpers::previewHelperbase>>(contentHelper).get());
		}
		else
		{
			try {
				std::get<concurrency::task<serviceHelpers::previewHelperbase*>>(contentHelper).then([](account::serviceHelpers::previewHelperbase* p) {
					Reddit::PopupContent::OpenPreviewPopup(p);
				});
			}
			catch (std::bad_variant_access& b)
			{
				goto nonC;
			}
		}
	}


	concurrency::task<Windows::Data::Json::JsonObject^> account::subpostUWP::getComments()
	{
		//concurrency::create_task();
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Windows::UI::Xaml::FrameworkElement^ account::subpostUWP::thumbnail::get()
	{
		if (helper.self)
		{
			auto symbol = ref new Windows::UI::Xaml::Controls::FontIcon();
			symbol->Glyph = L"";
			symbol->FontSize = 76;
			return symbol;
		}
		else
		{
			auto image = ref new Windows::UI::Xaml::Controls::Image();
			image->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(thumbnailUri);
			return image;
		}
	}
	account::subpostUWP::subpostUWP(Windows::Data::Json::JsonObject ^ data) : helper(data)
	{
		if (!helper.self)
		{
			/*contentTask = serviceHelpers::jsonHelper(data);
			contentTask.then([this](std::pair<postContentType, serviceHelpers::previewHelperbase*> x) {
				
				contentHelper = x.second;
				if (contentType != x.first)
				{
					Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, x]() {
						contentType = x.first;
					}));
				}
				return x.second;
			});*/
			auto prelim = account::serviceHelpers::prelimContentHelper(data);
			contentType = std::get<0>(prelim);
			if (std::get<1>(prelim) == nullptr && contentType != postContentType::linktype)
			{
				if (std::get<2>(prelim) != nullptr)
				{
					contentHelper = (*std::get<2>(prelim))(data, std::get<0>(prelim)).then([this](std::pair<postContentType, serviceHelpers::previewHelperbase*> x) {
						if (contentType != x.first)
						{
							Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, x]() {
								contentType = x.first;
							}));
						}
						contentHelper = std::unique_ptr<account::serviceHelpers::previewHelperbase>(x.second);
						return x.second;
					});
				}
				else
				{
					contentHelper = account::serviceHelpers::jsonHelper(data).then([this](std::pair<postContentType, serviceHelpers::previewHelperbase*> x) {
						if (contentType != x.first)
						{
							Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, x]() {
								contentType = x.first;
							}));
						}
						contentHelper = std::unique_ptr<account::serviceHelpers::previewHelperbase>(x.second);
						return x.second;
					});
				}
			}
			else
			{
				contentHelper = std::unique_ptr<account::serviceHelpers::previewHelperbase>(std::get<1>(prelim));
			}
		}
		else if (helper.self && (helper.selftext != ""))
		{
			contentHelper = std::make_unique<serviceHelpers::selfDisplay>(helper.selftext);
		}
		try
		{
			if (data->HasKey("preview"))
			{
				try
				{					
					auto arr = data->GetNamedObject("preview")->GetNamedArray("images")->GetObjectAt(0)->GetNamedArray("resolutions");
					if (arr->Size > 2)
						previewuri = ref new Uri(arr->GetObjectAt(2)->GetNamedString("url"));
					else
						previewuri = ref new Uri(arr->GetObjectAt(arr->Size - 1)->GetNamedString("url"));
				}
				catch (...)
				{
					auto x = data->GetNamedObject("preview")->GetNamedArray("images")->GetObjectAt(0)->GetNamedArray("resolutions");
					previewuri = ref new Uri(x->GetObjectAt(x->Size - 1)->GetNamedString("url"));
				}
			}
			else
				previewuri = ref new Uri("http://placehold.it/350x150");
		}
		catch (...)
		{
			previewuri = ref new Uri("http://placehold.it/350x150");
		}
		try
		{
			auto thumbstr = data->GetNamedString("thumbnail");
			if(thumbstr != nullptr && thumbstr != "self" && thumbstr != "nsfw" && thumbstr != "default" && thumbstr != "image" && thumbstr != "spoiler")
				thumbnailUri = ref new Uri(thumbstr);
			else
				thumbnailUri = ref new Uri("http://placehold.it/150x150");
		}
		catch (...)
		{
			thumbnailUri = ref new Uri("http://placehold.it/150x150");
		}
		previewCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::previewDialog), nullptr);
		_changedownvoteCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::_changedownvote), nullptr);
		_changeupvoteCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::_changeupvote), nullptr);
	}

	