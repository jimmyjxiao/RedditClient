#pragma once
#include "subpost.h"
#include "serviceHelpers.h"
namespace account
{

	

	public ref class subpostUWP sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	private:
		Windows::Foundation::Uri^ thumbnailUri;
		Windows::Foundation::Uri^ previewuri;
		Platform::IBox<bool>^ likes;
		Platform::String^ fullname;
		Platform::String^ uniqueID;
		void _changeupvote(Platform::Object^ param);
		void _changedownvote(Platform::Object^ param);
		void previewDialog(Platform::Object^);
	internal:
		subpost helper;
		std::unique_ptr<serviceHelpers::previewHelperbase> contentHelper;

	public:
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;
		property int score
		{
			int get()
			{
				return helper.score;
			}
		}
		property Platform::IBox<bool>^ Liked
		{
			Platform::IBox<bool>^ get()
			{
				return helper.myvote;
			}
			void set(Platform::IBox<bool>^ input);
		}
		property Windows::UI::Xaml::Input::ICommand^ previewCommand;
		property Windows::UI::Xaml::Input::ICommand^ changeupvote;
		property Windows::UI::Xaml::Input::ICommand^ changedownvote;
		property Windows::Foundation::Uri^ previewURI
		{
			Windows::Foundation::Uri^ get()
			{
				return previewuri;
			}
		}
		property Windows::Foundation::Uri^ thumbnailURI
		{
			Windows::Foundation::Uri^ get()
			{
				return thumbnailUri;
			}
		}
		property postContentType contentType
		{
			postContentType get()
			{
				return helper.contentType;
			}
			void set(postContentType z)
			{
				helper.contentType = z;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("contentType"));
			}
		}
		property Platform::String^ selftext_html
		{
			Platform::String^ get()
			{
				return helper.selftext_html;
			}
		}
		property Platform::String^ selftext
		{
			Platform::String^ get()
			{
				return nullptr;
			}
		}
		property Platform::String^ OP
		{
			Platform::String^ get()
			{
				return helper.author;
			}
		}
		property Platform::String^ Title
		{
			Platform::String^ get()
			{
				return helper.Title;
			}
		}
		property Platform::String^ link
		{
			Platform::String^ get()
			{
				return helper.link;
			}
		}
		property bool self
		{
			bool get()
			{
				return helper.self;
			}
		}
		property Platform::String^ subreddit
		{
			Platform::String^ get()
			{
				return helper.subreddit;
			}
		}
		property int num_comments
		{
			int get()
			{
				return helper.num_comments;
			}
		}
		property bool NSFW
		{
			bool get() 
			{
				return helper.NSFW;
			}
		}
	internal:
		concurrency::task<Windows::Data::Json::JsonObject^> getComments();
		subpostUWP(Windows::Data::Json::JsonObject^ data);

	};
	struct subredditlisting
	{
		concurrency::task<Platform::Collections::Vector<account::subpostUWP^>^> getTask;
		Platform::String^ after;
		Platform::String^ before;
		Platform::Collections::Vector<account::subpostUWP^>^ listing;
	};

}
