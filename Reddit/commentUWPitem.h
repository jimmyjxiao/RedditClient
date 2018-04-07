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
		public Windows::UI::Xaml::Data::INotifyPropertyChanged, IRedditTypeIdentifier
	{
	private:
		mdblock::refMDElements^ mdCmds = nullptr;
		void _changedownvote(Platform::Object^);
		void _changeupvote(Platform::Object^);
	internal:
		comment helper;
		commentUWPlisting* replies = nullptr;
		void cacheMDElements();
		CommentUWPitem(Windows::Data::Json::JsonObject^ jsoncomment, Platform::String^ pid = nullptr, bool handleReplies = true);
	public:
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
		property Platform::IBox<bool>^ Liked
		{
			Platform::IBox<bool>^ get()
			{
				return helper.myvote;
			}
			void set(Platform::IBox<bool>^ newvalue);
		}
		property Windows::UI::Xaml::Input::ICommand^ changeupvote;
		property Windows::UI::Xaml::Input::ICommand^ changedownvote;
		property bool saved
		{
			bool get() { return helper.saved; }
			void set(bool s);
		}
		property int score
		{
			int get()
			{
				return helper.score;
			}
		}
		property Platform::String^ htmltext
		{
			Platform::String^ get()
			{
				return helper.htmltext;
			}
		}
		property Platform::String^ author
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