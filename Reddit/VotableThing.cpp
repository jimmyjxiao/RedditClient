#include "pch.h"
#include "VotableThing.h"
#include "globalvars.h"
#include "commentUWPitem.h"
namespace account
{
	concurrency::task<void> VotableThing::giveGold()
	{
		throw ref new Platform::NotImplementedException();
		//return globalvars::currentacc->giveGold()
	}
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
			JsonGetString(fullname, "name", json);
			JsonGetString(id, "id", json);
			if (json->HasKey("score_hidden"))
			{
				if (json->GetNamedBoolean("score_hidden"))
					score = globalvars::hiddenScore;
				else
					goto nullscore;
			}
			else
			{
				if (json->GetNamedBoolean("hide_score", false))
					score = globalvars::hiddenScore;
				else
				{
				nullscore:
					score = json->GetNamedNumber("score");
				}
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
			JsonGetString(author, "author", json);
			JsonGetNumberWithDefault(gilded, "gilded", json, 0);
			auto authorData = author->Data();
			for (auto &a : globalvars::accounts)
			{
				if (a.first == authorData)
					isMine = true;
			}
			JsonGetBooleanWithDefault(saved, "saved", json, false);
			//JsonGetString(permalink, "permalink", json);
			try {
				permalink = json->GetNamedString("permalink");
			}
			catch (Platform::Exception^ e)
			{
				__debugbreak();
			}
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