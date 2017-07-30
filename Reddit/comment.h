#pragma once
#include "VotableThing.h"
namespace account
{

	class comment :
		public VotableThing, RedditCreated
	{
	public:
		Platform::String^ htmltext;
		bool controversial;
		comment(Windows::Data::Json::JsonObject^ json);
	};

}