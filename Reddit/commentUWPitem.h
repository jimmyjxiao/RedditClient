#pragma once
#include "subpost.h"
#include "subpostUWP.h"
#include <deque> 
#include <queue>
#include "comment.h"

namespace account
{
	struct commentUWPlisting;
	public ref class CommentUWPitem sealed :
		public Windows::UI::Xaml::Data::INotifyPropertyChanged, IRedditTypeIdentifier, VotableInterface
	{
	private:
		Windows::UI::Xaml::Input::ICommand^ _changeupvoteCommand;
		Windows::UI::Xaml::Input::ICommand^ _changedownvoteCommand;
		mdblock::refMDElements^ mdCmds = nullptr;
		void _changedownvote(Platform::Object^);
		void _changeupvote(Platform::Object^);
	internal:
		comment helper;
		union
		{
			commentUWPlisting* replies = nullptr;
			void* linkparent;
		};
		void cacheMDElements();
		CommentUWPitem(Windows::Data::Json::JsonObject^ jsoncomment, Platform::String^ pid = nullptr,const bool handleReplies = true, Platform::String^ subreddit = nullptr);
	public:
		property Platform::String^ ParentSubreddit
		{
			Platform::String^ get()
			{
				return helper.parent_subreddit;
			}
		}
		property bool hasReplies
		{
			bool get()
			{
				return (replies != nullptr);
			}
		}
		property bool reply_is_only_more //If this comment has replies, but only the "more" thing was returned
		{
			bool get();
		}
		property bool controversial
		{
			bool get()
			{
				return helper.controversial;
			}
		}
		virtual property Platform::IBox<bool>^ Liked
		{
			Platform::IBox<bool>^ get()
			{
				return helper.myvote;
			}
			void set(Platform::IBox<bool>^ newvalue);
		}
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
		virtual property bool saved
		{
			bool get() { return helper.saved; }
			void set(bool s);
		}
		virtual property int score
		{
			int get()
			{
				return helper.score;
			}
		}

		virtual property Platform::String^ author
		{
			Platform::String^ get() 
			{
				return helper.author;
			}
		}
		property mdblock::refMDElements^ mdElements
		{
			mdblock::refMDElements^ get()
			{
				if (mdCmds == nullptr)
				{
					cacheMDElements();
				}
				return mdCmds;
			}
		}
		property Platform::String^ markdowntext
		{
			Platform::String^ get()
			{
				return helper.markdown;
			}
		}
		property bool isMine
		{
			bool get()
			{
				return helper.isMine;
			}
		}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;
		
		virtual ~CommentUWPitem();

		// Inherited via IRedditTypeIdentifier
		virtual property RedditType rType
		{
			RedditType get()
			{
				return RedditType::comment;
			}
		}

		// Inherited via VotableInterface
		virtual property Windows::Foundation::Uri ^ link
		{
			Windows::Foundation::Uri^ get()
			{
				return ref new Windows::Foundation::Uri(L"https://www.reddit.com", helper.permalink);
			}
		}
		virtual property unsigned int Golds
		{
			unsigned int get()
			{
				return helper.gilded;
			}
		}
};
	public ref struct moreComments sealed
	{
	public:
		property unsigned int count
		{
			unsigned int get() { return c; }
		}
		void clickedFunc();
		moreComments(Windows::Data::Json::JsonObject^ moreobject);
	internal:
		Platform::WeakReference parentTreeNode;
		std::wstring morelist;
	private:
		unsigned int c;
		
	};
	struct commentUWPlisting
	{
		moreComments^ more = nullptr;
		subpostUWP^ parent = nullptr;
		std::vector<account::CommentUWPitem^> commentList;
	};
}