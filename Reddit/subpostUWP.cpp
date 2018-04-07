﻿#include "pch.h"
#include "subpostUWP.h"
#include "globalvars.h"
#include "DelegateCommand.h"
#include <regex>
#include "ApplicationDataHelper.h"
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
namespace wsaw = Windows::Security::Authentication::Web;
using namespace account;


	void account::subpostUWP::Liked::set(Platform::IBox<bool>^ input)
	{
		if (input != helper.myvote)
		{
			helper.vote(input); //Todo: error handling
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Liked"));
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("score"));
		}
	}
	void account::subpostUWP::_changeupvote(Platform::Object ^ param)
	{
		//__debugbreak();
		if (helper.myvote == nullptr || helper.myvote->Value == false)
			Liked = true;
		else if (helper.myvote->Value == true)
			Liked = nullptr;
	}

	void account::subpostUWP::_changedownvote(Platform::Object ^ param)
	{
		//__debugbreak();
		if (helper.myvote == nullptr || helper.myvote->Value == true)
			Liked = false;
		else if (helper.myvote->Value == false)
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

	account::subpostUWP::~subpostUWP()
	{
		
		delete contentHelper;
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
			contentTask = serviceHelpers::jsonHelper(data).then([this](std::pair<postContentType, serviceHelpers::previewHelperbase*> x) {
				
				contentHelper = x.second;
				if (contentType != x.first)
				{
					Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([this, x]() {
						contentType = x.first;
					}));
				}
				return x.second;
			});
		}
		else if (helper.self && (helper.selftext != ""))
		{
			contentHelper = new serviceHelpers::selfDisplay(helper.selftext);
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

	