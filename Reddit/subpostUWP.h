#pragma once
#include "subpost.h"
#include "serviceHelpers.h"
namespace account
{

	public enum class reportApplicibility
	{
		subpost,
		comment,
		bothAllowed
	};	
	public value struct reportReason
	{
	public:
		Platform::String^ shortdesc;
		//property Platform::String^ description_html;
		Platform::String^ violationReason;
		reportApplicibility scope;
	};
	static reportReason jsonToReport(Windows::Data::Json::JsonObject^ jsonReason)
	{
		try {
			reportReason r;
			auto typestr = jsonReason->GetNamedString("kind");
			if (typestr == "all")
			{
				r.scope = reportApplicibility::bothAllowed;
			}
			else if (typestr == "link")
			{
				r.scope = reportApplicibility::subpost;
			}
			else
			{
				r.scope = reportApplicibility::comment;
			}
			r.shortdesc = jsonReason->GetNamedString("short_name");
			Platform::String^ vr = jsonReason->GetNamedString("violation_reason");
			if (vr == r.shortdesc)
				r.violationReason = r.shortdesc;
			else r.violationReason = vr;
			return std::move(r);
		}
		catch (...)
		{
			__debugbreak();
		}
	}
	public ref class subpostUWP sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged, IRedditTypeIdentifier, VotableInterface
	{
	private:
		Windows::UI::Xaml::Input::ICommand^ _changeupvoteCommand;
		Windows::UI::Xaml::Input::ICommand^ _changedownvoteCommand;
		Windows::Foundation::Uri^ thumbnailUri;
		Windows::Foundation::Uri^ previewuri;
		Platform::String^ fullname;
		Platform::String^ uniqueID;
		void _changeupvote(Platform::Object^ param);
		void _changedownvote(Platform::Object^ param);
		void previewDialog(Platform::Object^);
	internal:
		subpost helper;
		std::variant<std::unique_ptr<serviceHelpers::previewHelperbase>, concurrency::task<serviceHelpers::previewHelperbase*>> contentHelper = nullptr;
	public:
		property Platform::String^ domain
		{
			Platform::String^ get()
			{
				if (helper.self)
				{
					return "self";
				}
				else
				{
					return helper.link->Domain;
				}
			}
		}
		virtual property Platform::String^ CreatedString
		{
			Platform::String^ get()
			{
				return helper.getElapsedCreatedStr();
			}
		}
		virtual property Platform::String^ ScoreText
		{
			Platform::String^ get()
			{
				return helper.getScoreText();
			}
		}
		// Inherited via IRedditTypeIdentifier
		virtual property RedditType rType
		{
			RedditType get()
			{
				return RedditType::subpost;
			}
		}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;
		virtual property account::DistinguishedAccountTypes  DistinguishedAuthorType
		{
			account::DistinguishedAccountTypes get()
			{
				return helper.getAuthorIsDistinguished();
			}
		}
		virtual property unsigned int Golds
		{
			unsigned int get()
			{
				return helper.GetGilded();
			}
		}
		virtual property bool saved
		{
			bool get()
			{
				return helper.getIsSaved();
			}
			void set(bool nw)
			{
				if (nw != helper.getIsSaved())
				{
					if (nw)
					{
						helper.save();
					}
					else
					{
						helper.unsave();
					}
				}
			}
		}
		virtual property int score
		{
			int get()
			{
				return helper.GetScore();
			}
		}
		virtual property int Liked
		{
			int get()
			{
				return helper.getMyVote();
			}
			void set(int input);
		}
		property Windows::UI::Xaml::Input::ICommand^ previewCommand;
		virtual property Windows::UI::Xaml::Input::ICommand^ changeupvote
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _changeupvoteCommand;
			}
		}
		virtual property Windows::UI::Xaml::Input::ICommand^ changedownvote
		{
			Windows::UI::Xaml::Input::ICommand^ get()
			{
				return _changedownvoteCommand;
			}
		}
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
		property Windows::UI::Xaml::FrameworkElement^ thumbnail
		{
			Windows::UI::Xaml::FrameworkElement^ get();
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
		property Platform::String^ selftext
		{
			Platform::String^ get()
			{
				return nullptr;
			}
		}
		virtual property Platform::String^ author
		{
			Platform::String^ get()
			{
				return helper.Getauthor();
			}
		}
		property Platform::String^ Title
		{
			Platform::String^ get()
			{
				return helper.Title;
			}
		}
		virtual property Windows::Foundation::Uri^ link
		{
			Windows::Foundation::Uri^ get()
			{
				if (contentType != account::postContentType::selftype)
				{
					return helper.link;
				}
				else
				{
					return ref new Windows::Foundation::Uri(L"https://www.reddit.com", helper.Getpermalink());
				}
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
		Platform::String^ after = nullptr;
		Platform::String^ before = nullptr;
		Platform::Collections::Vector<account::subpostUWP^>^ listing = nullptr;
		~subredditlisting()
		{
			listing = nullptr;
			//__debugbreak();
		}
	};

}
namespace std
{
	template<>
	struct std::equal_to<account::reportReason>
	{
		bool operator () (const account::reportReason & lhs, const account::reportReason& rhs)
		{
			return ((lhs.shortdesc == rhs.shortdesc) && (lhs.scope == rhs.scope) && (lhs.violationReason == rhs.violationReason));
		}
	};
}