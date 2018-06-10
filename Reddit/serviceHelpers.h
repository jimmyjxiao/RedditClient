#pragma once
#include "subpost.h"
#include <unordered_map>
#include <variant>
namespace account
{
	namespace serviceHelpers
	{
		struct previewHelperbase
		{
		protected:
			Windows::Foundation::Uri^ Link;
		public:
			bool easypreview;
			previewHelperbase() { easypreview = false; }
			previewHelperbase(Windows::Foundation::Uri^ url)
			{
				easypreview = false;
				Link = url;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl();
		};
		struct selfDisplay : previewHelperbase
		{
		private:
			Platform::String^ md;
		public:
			selfDisplay(Platform::String^ md)
			{
				easypreview = true;
				this->md = md;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl();
		};
		struct imageDisplay : previewHelperbase
		{
		protected:
			Windows::Foundation::Uri^ contentLink;
			imageDisplay(){}
		public:
			imageDisplay(Windows::Foundation::Uri^ url) : previewHelperbase(url) 
			{
				contentLink = url;
				auto strD = contentLink->ToString();
				easypreview = true;
			}
			imageDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url) : previewHelperbase(Url) 
			{
				easypreview = true;
				contentLink = imageDirectLink;
			}
			virtual Windows::UI::Xaml::Media::ImageSource^ fullsizeImageSource() { return ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(contentLink); }
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
		};
		struct gifvDisplay : imageDisplay
		{
		protected:
			gifvDisplay() {}
			Windows::Media::Core::MediaSource^ source;
		public:
			gifvDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url, bool setSource = true);

			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
		};
		struct QualityScaleGifv : gifvDisplay
		{
		private:
			std::vector < std::pair<const std::wstring, Windows::Foundation::Uri^> >Qualities;
		public:
			QualityScaleGifv(std::vector < std::pair<const std::wstring, Windows::Foundation::Uri^> > q, Windows::Foundation::Uri^ url);
		};
		struct videoDisplay : gifvDisplay
		{

		protected:
			bool adaptive;
		public:
			videoDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url);
			videoDisplay(Windows::Foundation::Uri ^ imageDirectLink, Windows::Foundation::Uri ^ Url, Windows::Media::Streaming::Adaptive::AdaptiveMediaSource ^ source);

			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
		};
		struct albumDisplay : previewHelperbase
		{
		protected:
			Platform::Collections::Vector<Windows::UI::Xaml::UIElement^>^ ControlsList;
			std::vector<std::unique_ptr<previewHelperbase>> items;
		public:
			albumDisplay(Windows::Foundation::Uri^ url, std::vector<std::unique_ptr<previewHelperbase>> items) : previewHelperbase(url)
			{
				easypreview = true;
				this->items = std::move(items);
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
		};
		struct embedDisplay : previewHelperbase
		{
		protected:
			Platform::String^ framestr;
		public:
			embedDisplay(Windows::Foundation::Uri^ url, Platform::String^ frame) : previewHelperbase(url)
			{
				easypreview = true;
				framestr = frame;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
		};

		struct ServiceHelper
		{
			virtual concurrency::task<std::pair<account::postContentType, previewHelperbase*>> operator()(Windows::Foundation::Uri^ uri, account::postContentType hint = account::postContentType::selftype) const
			{
				return concurrency::task_from_result(prelimCall(uri));
			}
			virtual concurrency::task<std::pair<account::postContentType, previewHelperbase*>> operator()(Windows::Data::Json::JsonObject^ json, account::postContentType hint = account::postContentType::selftype) const
			{
				return operator()(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
			}
			virtual std::pair<account::postContentType, previewHelperbase*> prelimCall(Windows::Foundation::Uri^ uri) const = 0;
			virtual std::pair<account::postContentType, previewHelperbase*> prelimCall(Windows::Data::Json::JsonObject^ json) const
			{
				return prelimCall(ref new Windows::Foundation::Uri(json->GetNamedString("url")));
			}
			ServiceHelper(const ServiceHelper&) = delete;
			ServiceHelper() {}
		};
		extern const std::unordered_map<
			std::wstring_view,
			const ServiceHelper&
		> domainMap;
		concurrency::task<std::pair<account::postContentType, previewHelperbase*>> jsonHelper(Windows::Data::Json::JsonObject^ z);
		concurrency::task<std::pair<account::postContentType, previewHelperbase*>> urlHelper(Windows::Foundation::Uri^ url);
		std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const> prelimContentHelper(Windows::Data::Json::JsonObject^ z);
		std::tuple<account::postContentType, previewHelperbase*, const ServiceHelper* const> prelimContentHelper(Windows::Foundation::Uri^ url);
		//std::vector<std::unique_ptr<previewHelperbase>>  getManyHelpers(std::vector<Windows::Data::Json::JsonObject^>const & vec);
	}
}