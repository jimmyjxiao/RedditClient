#include "pch.h"
#include "commentUWPitem.h"
#include <ppl.h>
#include "globalvars.h"
#include "DelegateCommand.h"
#include "CommentView.xaml.h"
#include "linkHandler.h"
void account::CommentUWPitem::_changedownvote(Platform::Object ^)
{
	int cvote = helper.getMyVote();
	if (cvote == 0 || cvote == 1)
		Liked = -1;
	else if (cvote == -1)
		Liked = 0;
}
void account::CommentUWPitem::_changeupvote(Platform::Object ^)
{
	int cvote = helper.getMyVote();
	if (cvote == 0 || cvote == -1)
		Liked = true;
	else if (cvote == 1)
		Liked = 0;
}

void account::CommentUWPitem::cacheMDElements()
{
	try
	{
		mdCmds = ref new mdblock::refMDElements(std::move(helper.markdown));
		helper.markdown = nullptr;
		for (auto x : mdCmds->Links)
		{ 
			Reddit::linkHandler::urlLookupTasks.insert(std::make_pair(x->GetHashCode(), account::serviceHelpers::urlHelper(x)));
		}
	}
	catch (Platform::Exception^ e)
	{
		__debugbreak();
	}
	catch (std::exception e)
	{
		__debugbreak();
	}
	catch (...)
	{
		__debugbreak();
	}
}

void account::CommentUWPitem::recursivelyCacheMDElements()
{
	cacheMDElements();
	if (replies != nullptr)
	{
		concurrency::parallel_for_each(replies->commentList.begin(), replies->commentList.end(), [](CommentUWPitem^& a) {a->recursivelyCacheMDElements(); });
	}
}


bool account::CommentUWPitem::reply_is_only_more::get()
{
	if (hasReplies)
	{
		return ((replies->commentList.size() == 0) && (replies->more != nullptr));
	}
	else return false;
}
account::CommentUWPitem::CommentUWPitem(Windows::Data::Json::JsonObject^ jsoncomment, Platform::String^ pid,const bool handleReplies, Platform::String^ subreddit) : helper(jsoncomment, pid, std::move(subreddit))
{
	try {
		if (handleReplies)
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

					this->replies->more = ref new moreComments(lastObject->GetNamedObject("data"));
					replies->RemoveAtEnd();
				}
				else
					this->replies->commentList = std::vector<CommentUWPitem^>(replies->Size);
				if (replies->Size > 4)
				{
					concurrency::parallel_transform(Windows::Foundation::Collections::begin(replies), Windows::Foundation::Collections::end(replies), this->replies->commentList.begin(), [&s = helper.parent_subreddit](Windows::Data::Json::IJsonValue^ child) {
						return ref new CommentUWPitem(child->GetObject()->GetNamedObject("data"), nullptr, true, s);
					});
				}
				else
				{
					std::transform(Windows::Foundation::Collections::begin(replies), Windows::Foundation::Collections::end(replies), this->replies->commentList.begin(), [&s = helper.parent_subreddit](Windows::Data::Json::IJsonValue^ child) {
						return ref new CommentUWPitem(child->GetObject()->GetNamedObject("data"), nullptr, true, s);
					});
				}
			}
		}
		_changedownvoteCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &CommentUWPitem::_changedownvote), nullptr);
		_changeupvoteCommand = ref new DelegateCommand(ref new ExecuteDelegate(this, &CommentUWPitem::_changeupvote), nullptr);
	}
	catch (...)
	{
		__debugbreak();
	}
		
}

account::CommentUWPitem::~CommentUWPitem()
{
	delete replies;
}

void account::CommentUWPitem::Liked::set(int newvalue)
{
	if (newvalue != helper.getMyVote())
	{
		helper.vote(newvalue); //Todo: error handling
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("Liked"));
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("ScoreText"));
	}
}
void account::CommentUWPitem::saved::set(bool s)
{
	if (s != helper.getIsSaved())
	{
		if (s)
			helper.save();
		else
			helper.unsave();
	}
}

void account::moreComments::clickedFunc()
{
	auto p = parentTreeNode.Resolve<TreeViewControl::TreeNode>();
	try
	{
		
		auto data = static_cast<CommentUWPitem^>(p->Data);
		globalvars::currentacc->getmorecomments(this, data->helper.plink_id, data->helper.getId()).then([p,data](commentUWPlisting & a) {
			data->replies->commentList.reserve(data->replies->commentList.size() + a.commentList.size());
			std::move(a.commentList.begin(), a.commentList.end(), std::back_inserter(data->replies->commentList));
			data->replies->more = a.more;
			std::vector<TreeViewControl::TreeNode^> addingNodes(a.commentList.size());
			p->RemoveAtEnd();
			std::transform(a.commentList.begin(), a.commentList.end(), addingNodes.begin(), [p](CommentUWPitem^ i) {
				TreeViewControl::TreeNode^ newnode = ref new TreeViewControl::TreeNode();
				newnode->ParentNode = p;
				newnode->Data = i;
				if (i->hasReplies)
				{
					if (!i->reply_is_only_more)
					{
						for (auto &ve : i->replies->commentList)
						{
							unsigned int x;
							Reddit::CommentView::recursiveNode(newnode, ve, x);
						}
					}
					else
					{
						auto moreNode = ref new TreeViewControl::TreeNode();
						moreNode->Data = i->replies->more;
						newnode->Append(moreNode);
					}
				}
				return newnode;
			});
			std::move(addingNodes.begin(), addingNodes.end(), Windows::Foundation::Collections::back_inserter(p->getFoundationVec()));
		});
		
	}
	catch (Platform::NullReferenceException^ e)
	{
		__debugbreak();
	}
	catch (...)
	{
		__debugbreak();
	}
}

account::moreComments::moreComments(Windows::Data::Json::JsonObject ^ moreobject)
{
	auto arr = moreobject->GetNamedArray("children");
	c = moreobject->GetNamedNumber("count");
	try{
		for (Platform::Collections::VectorIterator<Windows::Data::Json::IJsonValue^> it = Windows::Foundation::Collections::begin(arr); it != Windows::Foundation::Collections::end(arr); it++)
		{
			if (it != Windows::Foundation::Collections::begin(arr))
			{
				morelist += L',';
			}
			morelist += (*it)->GetString()->Data();
		}
	}
	catch (...)
	{
		__debugbreak();
	}
}
