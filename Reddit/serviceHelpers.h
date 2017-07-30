#pragma once
#include "subpost.h"
namespace account
{
	namespace serviceHelpers
	{

		struct previewHelperbase
		{
		protected:
			Windows::Foundation::Uri^ Link;
		public:
			previewHelperbase(){}
			previewHelperbase(Windows::Foundation::Uri^ url)
			{
				Link = url;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl()
			{
				auto webview = ref new Windows::UI::Xaml::Controls::WebView();
				webview->Navigate(Link);
				return webview;
			}
		};
		struct imageDisplay : previewHelperbase
		{
		protected:
			Windows::Foundation::Uri^ contentLink;
		public:
			imageDisplay(Windows::Foundation::Uri^ url) : previewHelperbase(url)
			{
				contentLink = url;
			}
			imageDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url) : previewHelperbase(Url)
			{
				contentLink = imageDirectLink;
			}
			virtual Windows::UI::Xaml::Media::ImageSource^ fullsizeImageSource() { return ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(contentLink); }
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override
			{
				auto img = ref new Windows::UI::Xaml::Controls::Image();
				img->Source = fullsizeImageSource();
				return img;
			}
		};
		struct gifvDisplay : imageDisplay
		{
		protected:
			Windows::Media::Core::MediaSource^ source;
		public:
			gifvDisplay(Windows::Foundation::Uri^ imageDirectLink, Windows::Foundation::Uri^ Url) : imageDisplay(imageDirectLink, Url)
			{
				source = Windows::Media::Core::MediaSource::CreateFromUri(contentLink);
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override
			{
				auto player = ref new Windows::Media::Playback::MediaPlayer();
				player->AutoPlay = true;
				player->Source = source;
				player->IsMuted = true;
				player->IsLoopingEnabled = true;
				auto element = ref new Windows::UI::Xaml::Controls::MediaPlayerElement();
				element->SetMediaPlayer(player);
				element->AreTransportControlsEnabled = false;
				return element;
			}
		};
		struct albumDisplay : previewHelperbase
		{
		protected:
			Platform::Collections::Vector<Windows::Foundation::Uri^>^ source;
		public:
			albumDisplay(Windows::Foundation::Uri^ url, Platform::Collections::Vector<Windows::Foundation::Uri^>^ items) : previewHelperbase(url)
			{
				source = items;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override
			{
				auto flipview = ref new Windows::UI::Xaml::Controls::FlipView();
				flipview->ItemsSource = source;
				return flipview;
			}
		};
		struct embedDisplay : previewHelperbase
		{
		protected:
			Platform::String^ framestr;
		public:
			embedDisplay(Windows::Foundation::Uri^ url, Platform::String^ frame) : previewHelperbase(url)
			{
				framestr = frame;
			}
			virtual Windows::UI::Xaml::UIElement^ viewerControl() override
			{
				auto webview = ref new Windows::UI::Xaml::Controls::WebView(Windows::UI::Xaml::Controls::WebViewExecutionMode::SeparateThread);
				webview->NavigateToString(framestr);
				return webview;
			}
		};
		concurrency::task<std::pair<account::postContentType, previewHelperbase*>> urlHelper(Windows::Data::Json::JsonObject^ z);
		template<size_t size>
		std::array<previewHelperbase, size> getManyHelpers(std::vector<Windows::Data::Json::JsonObject^>const & vec);

	}
}