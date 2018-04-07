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

			Windows::Media::Core::MediaSource^ source;
		public:
			gifvDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url, bool setSource = true);
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override;
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
			Platform::Collections::Vector<Windows::Foundation::Uri^>^ source;
		public:
			albumDisplay(Windows::Foundation::Uri^ url, Platform::Collections::Vector<Windows::Foundation::Uri^>^ items) : previewHelperbase(url)
			{
				easypreview = true;
				source = items;
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


		concurrency::task<std::pair<account::postContentType, previewHelperbase*>> jsonHelper(Windows::Data::Json::JsonObject^ z);
		concurrency::task<std::pair<account::postContentType, previewHelperbase*>> urlHelper(Windows::Foundation::Uri^ url);
		//std::vector<std::unique_ptr<previewHelperbase>>  getManyHelpers(std::vector<Windows::Data::Json::JsonObject^>const & vec);
	}
}