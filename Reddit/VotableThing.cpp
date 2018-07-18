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
	concurrency::task<void> VotableThing::vote(int8_t direction)
	{
		if (myvote != direction)
		{
			if (myvote == 0)
			{
				if (myvote == 1)
				{
					score++;
				}
				else
				{
					score--;
				}
			}
			else if (myvote == 1)
			{
				if (direction == 0)
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
				if (direction == 0)
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
	VotableThing::VotableThing(Windows::Data::Json::JsonObject^ json) try : RedditCreated(json)
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
				if (nullwrapper->GetBoolean())
					myvote = 1;
				else
					myvote = -1;
			}
			else if (nullwrapper->ValueType == Windows::Data::Json::JsonValueType::Null)
			{
				myvote = 0;
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
			for (auto &a : globalvars::accounts)
			{
				if (author->Data() == a.first)
				{
					is_special = DistinguishedAccountTypes::me;
				}
			}
			if (is_special != DistinguishedAccountTypes::me)
			{
				if (const auto& x = json->GetNamedValue("distinguished"); x->ValueType == Windows::Data::Json::JsonValueType::String)
				{
					const auto &str = x->GetString();
					if (str == "admin")
					{
						is_special = DistinguishedAccountTypes::admin;
					}
					else if (str == "moderator")
					{
						is_special = DistinguishedAccountTypes::mod;
					}
					else
					{
						is_special = DistinguishedAccountTypes::none;
					}
				}
				else
				{
					is_special = DistinguishedAccountTypes::none;
				}
			}
		}
		catch (...)
		{
			__debugbreak();
		}
	}
	catch (Platform::COMException^ e)
	{
		__debugbreak();
	}

	VotableThing::~VotableThing()
	{
	}
	RedditCreated::RedditCreated(Windows::Data::Json::JsonObject ^ json)
	{
		createdUTC = json->GetNamedNumber("created_utc");
	}
	Platform::String ^& VotableThing::getFullname()
	{
		return fullname;
	}
	int VotableThing::getMyVote()
	{
		return myvote;
	}
	int VotableThing::GetScore()
	{
		return score;
	}
	Platform::String ^& VotableThing::Getpermalink()
	{
		return permalink;
	}
	Platform::String ^& VotableThing::Getauthor()
	{
		return author;
	}
	bool VotableThing::getIsMine()
	{
		return isMine;
	}
	bool VotableThing::getIsSaved()
	{
		return saved;
	}
	unsigned int VotableThing::GetGilded()
	{
		return gilded;
	}
	DistinguishedAccountTypes VotableThing::getAuthorIsDistinguished()
	{
		return is_special;
	}
	Platform::String ^& VotableThing::getId()
	{
		return id;
	}
}