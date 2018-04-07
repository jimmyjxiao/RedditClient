#pragma once
#include "VotableThing.h"
namespace account
{

	class comment :
		public VotableThing, RedditCreated
	{
	public:
		Platform::String^ htmltext;
		Platform::String^ plink_id;
		Platform::String^ markdown;
		bool controversial;
		comment(Windows::Data::Json::JsonObject^ json, Platform::String^ plink = nullptr);
		~comment();
	};

}