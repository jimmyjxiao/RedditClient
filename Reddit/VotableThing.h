#pragma once
#include "accountEnums.h"
namespace account
{
	public interface struct IRedditTypeIdentifier
	{
		property RedditType rType
		{
			RedditType get();
		}
	};
	public interface struct VotableInterface
	{
		property bool saved;
		property int score
		{
			int get();
		}
		property Platform::IBox<bool>^ Liked;
		property Windows::UI::Xaml::Input::ICommand^ changeupvote
		{
			Windows::UI::Xaml::Input::ICommand^ get();
		}
		property Windows::UI::Xaml::Input::ICommand^ changedownvote
		{
			Windows::UI::Xaml::Input::ICommand^ get();
		}
		property Platform::String^ author
		{
			Platform::String^ get();
		}
		property Windows::Foundation::Uri^ link
		{
			Windows::Foundation::Uri^ get();
		}
		property unsigned int Golds
		{
			unsigned int get();
		}
	};
	partial ref class CommentUWPitem;
	struct RedditCreated
	{
		time_t createdUTC;
		RedditCreated(Windows::Data::Json::JsonObject^ json);
	};
	class VotableThing
	{
	public:
		concurrency::task<void> giveGold();
		bool isMine;
		bool saved;
		unsigned int gilded;
		concurrency::task<CommentUWPitem^> reply(Platform::String^ markdown);
		Platform::String^ id;
		//concurrency::task<void> reply(Platform::String^ markdown, AccountInterface* replyas);
		concurrency::task<void> vote(Platform::IBox<bool>^ direction);
		Platform::IBox<bool>^ myvote;
		int score;
		Platform::String^ permalink;
		Platform::String^ author;
		concurrency::task<void> save();
		concurrency::task<void> unsave();
		VotableThing(Windows::Data::Json::JsonObject^ json);
		~VotableThing();
	private:
		Platform::String^ fullname;
	};

}