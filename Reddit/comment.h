#pragma once
#include "VotableThing.h"
namespace account
{

	class comment :
		public VotableThing
	{
	public:
		//Platform::String^ htmltext;
		Platform::String^ plink_id;
		Platform::String^ markdown;
		Platform::String^ parent_subreddit;
		bool controversial;
		comment(Windows::Data::Json::JsonObject^ json, Platform::String^ plink = nullptr, Platform::String^ subreddit = nullptr);
		~comment();
	};

}