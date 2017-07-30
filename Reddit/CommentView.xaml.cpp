//
// CommentView.xaml.cpp
// Implementation of the CommentView class
//

#include "pch.h"
#include "CommentView.xaml.h"
#include "commentUWPitem.h"
#include "globalvars.h"
#include "converters.h"
using namespace Reddit;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

CommentView::CommentView()
{
	InitializeComponent();
	CommentDataTemplate = static_cast<Windows::UI::Xaml::DataTemplate^>(this->Resources->Lookup("commentTemplate"));
	GetMoreDataTemplate = static_cast<Windows::UI::Xaml::DataTemplate^>(this->Resources->Lookup("loadmorebutton"));
}
void Reddit::CommentView::SetVec(std::vector<account::CommentUWPitem^> vec)
{
	//std::thread t([&vec, this]() {
		for (auto x : vec)
		{
			auto node = ref new TreeViewControl::TreeNode();
			
			node->Data = x;
			if (x->hasReplies)
			{
				if (!x->reply_is_only_more)
				{
					for (auto ve : x->replies->commentList)
					{
						recursiveNode(node, ve);
					}
				}
				else
				{
					auto moreNode = ref new TreeViewControl::TreeNode();
					moreNode->Data = x->replies->more;
					node->Append(moreNode);
				}
			}
			commentTree->RootNode->Append(node);
		}
	//});
	//t.detach();
}
void Reddit::CommentView::recursiveNode(TreeViewControl::TreeNode ^ par, account::CommentUWPitem ^ com)
{

	auto newnode = ref new TreeViewControl::TreeNode();
	newnode->Data = com;
	par->Append(newnode);
	if (com->hasReplies)
	{
		if (!com->reply_is_only_more)
		{
			for (auto v : com->replies->commentList)
			{
				if (v->hasReplies)
					recursiveNode(newnode, v);
				else
				{
					auto noChildNode = ref new TreeViewControl::TreeNode();
					noChildNode->Data = v;
					newnode->Append(noChildNode);
				}
			}
			if (com->replies->more != nullptr)
			{
				auto moreNode = ref new TreeViewControl::TreeNode();
				moreNode->Data = com->replies->more;
				newnode->Append(moreNode);
			}
		}
		else
		{
			auto moreNode = ref new TreeViewControl::TreeNode();
			moreNode->Data = com->replies->more;
			newnode->Append(moreNode);
		}
	}
}
void CommentView::postID::set(Platform::String^ in)
{
	
	_postID = in;
	globalvars::currentacc->getCommentAsyncVec(in).then([this](account::commentUWPlisting z) {
		for (auto x : z.commentList)
		{
			auto node = ref new TreeViewControl::TreeNode();
			
			node->Data = x;
			if (x->hasReplies)
			{
				if (!x->reply_is_only_more)
				{
					for (auto ve : x->replies->commentList)
					{
						recursiveNode(node, ve);
					}
				}
				else
				{
					auto moreNode = ref new TreeViewControl::TreeNode();
					moreNode->Data = x->replies->more;
					node->Append(moreNode);
				}
			}
			commentTree->RootNode->Append(node);
		}
	});
}

void Reddit::CommentView::commentTree_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args)
{

	if (dynamic_cast<account::CommentUWPitem^>(dynamic_cast<TreeViewControl::TreeNode^>(args->Item)->Data) != nullptr)
	{
		if (args->ItemContainer != nullptr)
		{
			if (static_cast<bool>(args->ItemContainer->Tag) == false)
			{
				
				moreRecycle.insert(args->ItemContainer);
				args->ItemContainer = nullptr;
				goto retrieveCommentTemplate;
			}
			else
			{
				commentRecycle.erase(args->ItemContainer);
			}
		}
		else
		{
		retrieveCommentTemplate:
			if (commentRecycle.size() > 0)
			{
				args->ItemContainer = static_cast<TreeViewControl::TreeViewItem^>(*commentRecycle.begin());
				commentRecycle.erase(commentRecycle.begin());
			}
			else
			{
				TreeViewControl::TreeViewItem^ newitem = ref new TreeViewControl::TreeViewItem();
				newitem->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Black);
				newitem->BorderThickness = itemBorder;
				
				newitem->ContentTemplate = CommentDataTemplate;
				newitem->Tag = true;
				args->ItemContainer = newitem;
			}
		}
	}
	else if (dynamic_cast<account::moreComments^>(dynamic_cast<TreeViewControl::TreeNode^>(args->Item)->Data) != nullptr)
	{
		if (args->ItemContainer != nullptr)
		{
			if (static_cast<bool>(args->ItemContainer->Tag) == true)
			{
				commentRecycle.insert(args->ItemContainer);
				args->ItemContainer = nullptr;
				goto retrieveMoreTemplate;
			}
			else
			{
				moreRecycle.erase(args->ItemContainer);
			}
		}
		else
		{
		retrieveMoreTemplate:
			if (moreRecycle.size() > 0)
			{
				args->ItemContainer = static_cast<TreeViewControl::TreeViewItem^>(*moreRecycle.begin());
				moreRecycle.erase(moreRecycle.begin());
			}
			else
			{
				TreeViewControl::TreeViewItem^ newitem = ref new TreeViewControl::TreeViewItem();
				newitem->ContentTemplate = GetMoreDataTemplate;
				newitem->Tag = false;
				args->ItemContainer = newitem;
			}
		}
	}
	else
		__debugbreak();
	args->IsContainerPrepared = true;
}
