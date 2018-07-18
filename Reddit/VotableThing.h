#pragma once
#include "accountEnums.h"
namespace account
{
	public enum class DistinguishedAccountTypes 
	{
		 admin, mod, op , none, me
	};
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
		property int Liked;
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
		property DistinguishedAccountTypes  DistinguishedAuthorType
		{
			DistinguishedAccountTypes get();
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
	class VotableThing : RedditCreated
	{
	public:
		Platform::String^& getFullname();
		int getMyVote();
		int GetScore();
		Platform::String^& Getpermalink();
		Platform::String^& Getauthor();
		bool getIsMine();
		bool getIsSaved();
		unsigned int GetGilded();
		DistinguishedAccountTypes getAuthorIsDistinguished();
		Platform::String^& getId();
		//concurrency::task<void> reply(Platform::String^ markdown, AccountInterface* replyas);
		concurrency::task<void> vote(int8_t direction);
		
		concurrency::task<void> save();
		concurrency::task<void> unsave();
		VotableThing(Windows::Data::Json::JsonObject^ json);
		~VotableThing();
		concurrency::task<void> giveGold();
		concurrency::task<CommentUWPitem^> reply(Platform::String^ markdown);
	private:
		Platform::String^  fullname;
		Platform::String^  id;
		Platform::String^ permalink;
		Platform::String^ author;
		int32_t score;
		unsigned int gilded : 25;
		bool isMine : 1;
		bool saved : 1;
		int8_t myvote : 2;
		DistinguishedAccountTypes is_special : 3;
	};

}