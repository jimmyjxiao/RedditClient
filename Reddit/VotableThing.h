#pragma once

namespace account
{
	struct RedditCreated
	{
		time_t createdUTC;
		RedditCreated(Windows::Data::Json::JsonObject^ json);
	};
	class VotableThing
	{
	public:
		bool isMine = false;
		concurrency::task<void> reply(Platform::String^ markdown);
		Platform::String^ id;
		//concurrency::task<void> reply(Platform::String^ markdown, AccountInterface* replyas);
		concurrency::task<void> vote(Platform::IBox<bool>^ direction);
		Platform::IBox<bool>^ myvote;
		int score;
		bool saved;
		Platform::String^ author;
		concurrency::task<void> save();
		concurrency::task<void> unsave();
		VotableThing(Windows::Data::Json::JsonObject^ json);
		~VotableThing();
	private:
		Platform::String^ fullname;
	};

}