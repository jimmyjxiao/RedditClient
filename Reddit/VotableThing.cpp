#include "pch.h"
#include "VotableThing.h"
#include "globalvars.h"
#include "commentUWPitem.h"
namespace account
{
	concurrency::task<CommentUWPitem^> VotableThing::reply(Platform::String ^ markdown)
	{
		return globalvars::currentacc->comment(fullname, markdown).then([](Windows::Web::Http::HttpResponseMessage^ m) {
			Platform::String^ debug = m->Content->ToString();
			m->EnsureSuccessStatusCode();
			auto newcommentjs = Windows::Data::Json::JsonObject::Parse(debug);
			return ref new CommentUWPitem(newcommentjs->GetNamedObject("json")->GetNamedObject("data")->GetNamedArray("things")->GetObjectAt(0)->GetNamedObject("data"));
		});
	}
	/*concurrency::task<void> VotableThing::reply(Platform::String ^ markdown, AccountInterface * replyas)
	{
		return concurrency::task<void>();
	}*/
	concurrency::task<void> VotableThing::vote(Platform::IBox<bool>^ direction)
	{
		if (myvote != direction)
		{
			if (myvote == nullptr)
			{
				if (direction->Value)
				{
					score++;
				}
				else
				{
					score--;
				}
			}
			else if (myvote->Value)
			{
				if (direction == nullptr)
				{
					score--;
				}
				else
				{
					score -= 2;
				}
			}
			else
			{
				if (direction == nullptr)
				{
					score++;
				}
				else
				{
					score += 2;
				}
			}
		}
		myvote = direction;
		return globalvars::currentacc->vote(direction, fullname).then([this, direction]() {

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
		return globalvars::currentacc->changeSave(fullname, false).then([this]() {
			saved = false;
		});
	}
	VotableThing::VotableThing(Windows::Data::Json::JsonObject^ json)
	{
		try
		{
			fullname = json->GetNamedString("name");
			id = json->GetNamedString("id");
			
			if (json->HasKey("score_hidden"))
			{
				if (json->GetNamedBoolean("score_hidden"))
					score = globalvars::hiddenScore;
				else
					goto nullscore;
			}
			else if (json->GetNamedBoolean("hide_score"))
			{
				score = globalvars::hiddenScore;
			}
			else
			{
				nullscore:
				score = json->GetNamedNumber("score");
			}
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
			gilded = json->GetNamedNumber("gilded");
			auto authorData = author->Data();
			for (auto &a : globalvars::accounts)
			{
				if (a.first == authorData)
					isMine = true;
			}
			saved = json->GetNamedBoolean("saved");
			permalink = json->GetNamedString("permalink");
		}
		catch (...)
		{
			__debugbreak();
		}
	}


	VotableThing::~VotableThing()
	{
	}
	RedditCreated::RedditCreated(Windows::Data::Json::JsonObject ^ json)
	{
		createdUTC = json->GetNamedNumber("created_utc");
	}
}