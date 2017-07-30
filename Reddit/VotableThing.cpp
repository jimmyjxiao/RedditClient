#include "pch.h"
#include "VotableThing.h"
#include "globalvars.h"
namespace account
{
	concurrency::task<void> VotableThing::reply(Platform::String ^ markdown)
	{
		throw ref new Platform::NotImplementedException();
	}
	/*concurrency::task<void> VotableThing::reply(Platform::String ^ markdown, AccountInterface * replyas)
	{
		return concurrency::task<void>();
	}*/
	concurrency::task<void> VotableThing::vote(Platform::IBox<bool>^ direction)
	{
		return globalvars::currentacc->vote(direction, fullname).then([this, direction]() {
			myvote = direction;
		});
	}
	concurrency::task<void> VotableThing::save()
	{
		return globalvars::currentacc->changeSave(fullname, true).then([this]() {
			saved = true;
		});
	}
	concurrency::task<void> VotableThing::unsave()
	{
		return globalvars::currentacc->changeSave(fullname, true).then([this]() {
			saved = false;
		});
	}
	VotableThing::VotableThing(Windows::Data::Json::JsonObject^ json)
	{
		fullname = json->GetNamedString("name");
		id = json->GetNamedString("id");
		score = json->GetNamedNumber("score");
		Windows::Data::Json::IJsonValue^ nullwrapper = json->GetNamedValue("likes");
		if (nullwrapper->ValueType != Windows::Data::Json::JsonValueType::Null)
		{

			myvote = nullwrapper->GetBoolean();
		}
		else if (nullwrapper->ValueType == Windows::Data::Json::JsonValueType::Null)
		{

			myvote = nullptr;
		}
		author = json->GetNamedString("author");
		auto authorData = author->Data();
		for (auto &a : globalvars::accounts)
		{
			if (a.first == authorData)
				isMine = true;
		}
		saved = json->GetNamedBoolean("saved");
	}


	VotableThing::~VotableThing()
	{
	}
	RedditCreated::RedditCreated(Windows::Data::Json::JsonObject ^ json)
	{
		createdUTC = json->GetNamedNumber("created_utc");
	}
}