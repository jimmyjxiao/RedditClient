#include "pch.h"
#include "commentUWPitem.h"
#include <ppl.h>
#include "globalvars.h"
#include "DelegateCommand.h"
void account::CommentUWPitem::_changedownvote(Platform::Object ^)
{
	if (helper.myvote == nullptr || helper.myvote->Value == true)
		Liked = false;
	else if (helper.myvote->Value == false)
		Liked = nullptr;
}
void account::CommentUWPitem::_changeupvote(Platform::Object ^)
{
	//__debugbreak();
	if (helper.myvote == nullptr || helper.myvote->Value == false)
		Liked = true;
	else if (helper.myvote->Value == true)
		Liked = nullptr;
}
bool account::CommentUWPitem::reply_is_only_more::get()
{
	if (hasReplies)
	{
		return ((replies->commentList.size() == 0) && (replies->more != nullptr));
	}
	else return false;
}
account::CommentUWPitem::CommentUWPitem(Windows::Data::Json::JsonObject^ jsoncomment) : helper(jsoncomment)
{
		auto replyValue = jsoncomment->GetNamedValue("replies");
		if (replyValue->ValueType == Windows::Data::Json::JsonValueType::Object)
		{
			this->replies = new commentUWPlisting();
			auto replydata = replyValue->GetObject()->GetNamedObject("data");
			/*auto beforevalue = replydata->GetNamedValue("before");
			auto aftervalue = replydata->GetNamedValue("after");*/
			/*if (beforevalue->ValueType == Windows::Data::Json::JsonValueType::String)
				replies->before = beforevalue->GetString();
			if (aftervalue->ValueType == Windows::Data::Json::JsonValueType::String)
				replies->after = aftervalue->GetString();*/
			auto replies = replydata->GetNamedArray("children");
			auto lastObject = (*(Windows::Foundation::Collections::end(replies) - 1))->GetObject();
			if (lastObject->GetNamedString("kind") == "more")
			{
				this->replies->commentList = std::vector<CommentUWPitem^>(replies->Size - 1);

				this->replies->more = ref new moreComments();
				for (auto &&x : lastObject->GetNamedObject("data")->GetNamedArray("children"))
				{
					this->replies->more->morelist.push(x->GetString());
				}
				replies->RemoveAtEnd();
			}
			else
				this->replies->commentList = std::vector<CommentUWPitem^>(replies->Size);
			if (replies->Size > 4)
			{
				concurrency::parallel_transform(Windows::Foundation::Collections::begin(replies), Windows::Foundation::Collections::end(replies), this->replies->commentList.begin(), [](Windows::Data::Json::IJsonValue^ child) {
					return ref new CommentUWPitem(child->GetObject()->GetNamedObject("data"));
				});
			}
			else
			{
				std::transform(Windows::Foundation::Collections::begin(replies), Windows::Foundation::Collections::end(replies), this->replies->commentList.begin(), [](Windows::Data::Json::IJsonValue^ child) {
					return ref new CommentUWPitem(child->GetObject()->GetNamedObject("data"));
				});
			}
		}
		changedownvote = ref new DelegateCommand(ref new ExecuteDelegate(this, &CommentUWPitem::_changedownvote), nullptr);
		changeupvote = ref new DelegateCommand(ref new ExecuteDelegate(this, &CommentUWPitem::_changeupvote), nullptr);
		
}

account::CommentUWPitem::~CommentUWPitem()
{
	delete replies;
}

void account::CommentUWPitem::Liked::set(Platform::IBox<bool>^ newvalue)
{
	if (newvalue != helper.myvote)
	{
		auto orig = helper.myvote;
		helper.myvote = newvalue;
		helper.vote(newvalue); //Todo: error handling
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Liked"));

	}
}
void account::CommentUWPitem::saved::set(bool s)
{
	if (s != helper.saved)
	{
		if (s)
			helper.save();
		else
			helper.unsave();
	}
}