#include "pch.h"
#include "subpostUWP.h"
#include "globalvars.h"
#include "DelegateCommand.h"
#include <regex>
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
namespace wsaw = Windows::Security::Authentication::Web;
using namespace account;


	void account::subpostUWP::Liked::set(Platform::IBox<bool>^ input)
	{
		if (input != likes)
		{
			auto orig = likes;
			likes = input;
			helper.vote(input); //Todo: error handling
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Liked"));

		}
	}
	void account::subpostUWP::_changeupvote(Platform::Object ^ param)
	{
		//__debugbreak();
		if (likes == nullptr || likes->Value == false)
			Liked = true;
		else if (likes->Value == true)
			Liked = nullptr;
	}

	void account::subpostUWP::_changedownvote(Platform::Object ^ param)
	{
		//__debugbreak();
		if (likes == nullptr || likes->Value == true)
			Liked = false;
		else if (likes->Value == false)
			Liked = nullptr;
	}

	void account::subpostUWP::previewDialog(Platform::Object ^)
	{

		auto dialog = ref new Windows::UI::Xaml::Controls::ContentDialog();
		dialog->Content = contentHelper->viewerControl();
		dialog->PrimaryButtonText = "Go to Comments";
		dialog->CloseButtonText = "Close";
		dialog->ShowAsync();
	}

	concurrency::task<Windows::Data::Json::JsonObject^> account::subpostUWP::getComments()
	{
		//concurrency::create_task();
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}

	account::subpostUWP::subpostUWP(Windows::Data::Json::JsonObject ^ data) : helper(data)
	{
		if (!helper.self)
		{
			serviceHelpers::urlHelper(data).then([this](std::pair<postContentType, serviceHelpers::previewHelperbase*> x) {
				contentHelper = std::unique_ptr<serviceHelpers::previewHelperbase>(x.second);
				return Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, x]() {
					contentType = x.first;
				}));
			});
		}
		try
		{
			if (data->HasKey("preview"))
			{
				try
				{
					previewuri = ref new Uri(data->GetNamedObject("preview")->GetNamedArray("images")->GetObjectAt(0)->GetNamedArray("resolutions")->GetObjectAt(2)->GetNamedString("url"));
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
			thumbnailUri = ref new Uri(data->GetNamedString("thumbnail"));
		}
		catch (...)
		{
			thumbnailUri = ref new Uri("http://placehold.it/150x150");
		}
		previewCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::previewDialog), nullptr);
		changedownvote = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::_changedownvote), nullptr);
		changeupvote = ref new DelegateCommand(ref new ExecuteDelegate(this, &subpostUWP::_changeupvote), nullptr);
	}
