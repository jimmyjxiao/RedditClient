//
// CommentView.xaml.cpp
// Implementation of the CommentView class
//

#include "pch.h"
#include "CommentView.xaml.h"
#include "commentUWPitem.h"
#include "globalvars.h"
#include "converters.h"
#include "ReplyBox.xaml.h"
#include <ppl.h>
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
	reportingReasons = ref new Platform::Collections::Vector<account::reportReason>();
	Resources->Insert("reportReasonsResource", reportingReasons);
	InitializeComponent();
	

}
void Reddit::CommentView::setHeaderpost(account::subpostUWP ^ post)
{
	this->commentTree->Header = post;

}
void Reddit::CommentView::SetVec(std::vector<account::CommentUWPitem^> vec)
{
	//std::thread t([&vec, this]() {
	/*Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Idle,
		ref new Windows::UI::Core::DispatchedHandler([this, vec]()
	{*/
	auto newroot = ref new TreeViewControl::TreeNode();
		for (auto x : vec)
		{
			auto node = ref new TreeViewControl::TreeNode();

			node->Data = x;
			if (x->hasReplies)
			{
				if (!x->reply_is_only_more)
				{
					unsigned int currentmax = 1;
					for (auto ve : x->replies->commentList)
					{
						unsigned int _c = 0;
						recursiveNode(node, ve, _c);
						if (_c > currentmax)
							currentmax = _c;
					}
					if (currentmax > max_depth)
						max_depth = currentmax;
				}
				else
				{
					auto moreNode = ref new TreeViewControl::TreeNode();
					x->replies->more->parentTreeNode = node;
					moreNode->Data = x->replies->more;
					node->Append(moreNode);
				}
			}
			//commentTree->RootNode->Append(node);
			newroot->Append(node);
		}
		addThreadlines();
		commentTree->RootNode = newroot;
	//}));

	//});
	//t.detach();
}



void Reddit::CommentView::addThreadlines()
{
	auto c = static_cast<Grid^>(commentTree->PublicGetTemplateChild("rootygrid"));
	for (unsigned int i = 1; i != max_depth; i++)
	{
		if ((i & 1))
		{
			auto rectangle = ref new Windows::UI::Xaml::Shapes::Rectangle();
			rectangle->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
			rectangle->Margin = Windows::UI::Xaml::ThicknessHelper::FromLengths(i * 20, 0, 0, 0);
			static auto lightgray = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::LightGray);
			rectangle->Fill = lightgray;
			rectangle->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Stretch;
			rectangle->Width = 20;
			c->Children->InsertAt(0, std::move(rectangle));
		}
	}
}

ExpanderControl::Expander^ Reddit::CommentView::findExpanderRecurse(Windows::UI::Xaml::FrameworkElement^ parent)
{
	if (parent == nullptr)
	{
		return nullptr;
	}
	if (parent->Name == "contentExpander")
	{
		return (ExpanderControl::Expander^)parent;
	}
	unsigned int count = VisualTreeHelper::GetChildrenCount(parent);
	ExpanderControl::Expander^ returning = nullptr;
	for (int i = 0; i < count; i++)
	{
		FrameworkElement^ child = (FrameworkElement^)VisualTreeHelper::GetChild(parent, i);
		auto z = findExpanderRecurse(child);
		if (z != nullptr)
		{
			returning = z;
			break;
		}
	}
	return returning;
}

void Reddit::CommentView::recursiveNode(TreeViewControl::TreeNode ^ par, account::CommentUWPitem ^ com, unsigned int& depthcounter)
{
	depthcounter++;
	auto newnode = ref new TreeViewControl::TreeNode();
	newnode->Data = com;
	par->Append(newnode);
	if (com->hasReplies)
	{
		if (!com->reply_is_only_more)
		{
			unsigned int currentmax = depthcounter + 1;
			for (auto v : com->replies->commentList)
			{
				if (v->hasReplies)
				{
					unsigned int _c = depthcounter;
					recursiveNode(newnode, v, _c);
					if (_c > currentmax)
						currentmax = _c;
				}
				else
				{
					auto noChildNode = ref new TreeViewControl::TreeNode();
					noChildNode->Data = v;
					newnode->Append(noChildNode);
				}
			}
			depthcounter = currentmax;
			if (com->replies->more != nullptr)
			{
				auto moreNode = ref new TreeViewControl::TreeNode();
				com->replies->more->parentTreeNode = newnode;
				moreNode->Data = com->replies->more;
				newnode->Append(moreNode);
			}
		}
		else
		{
			auto moreNode = ref new TreeViewControl::TreeNode();
			moreNode->Data = com->replies->more;
			com->replies->more->parentTreeNode = newnode;
			newnode->Append(moreNode);
		}
	}
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
				newitem->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
				newitem->HorizontalContentAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
				newitem->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Black);
				newitem->BorderThickness = itemBorder;
				
				newitem->ContentTemplate = commentTemplate;
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
				newitem->ContentTemplate = loadmorebutton;
				newitem->Tag = false;
				args->ItemContainer = newitem;
			}
		}
	}
	else
		__debugbreak();
	args->IsContainerPrepared = true;
}




void Reddit::CommentView::commentTree_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (commentTree->Header != nullptr)
	{
		auto x = findExpanderRecurse(commentTree);
		auto v = static_cast<account::subpostUWP^>(commentTree->Header);
		if (v->contentType != account::postContentType::selftype && v->contentType != account::postContentType::linktype)
		{
			x->IsExpanded = true;
		}
		else
		{
			x->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			x->IsExpanded = false;
		}
	}
}


void Reddit::CommentView::replyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto rbutton = static_cast<AppBarToggleButton^>(sender);
	if (rbutton->IsChecked->Value)
	{
		if (rbutton->Tag == nullptr)
		{
			int i = 0;
			DependencyObject^ x = (DependencyObject^)sender;
			for (int ii = 0; i < 8; i++)
			{
				x = VisualTreeHelper::GetParent(x);
			}

			auto g = static_cast<Grid^>(x);
			auto node = static_cast<TreeViewControl::TreeNode^>(g->Tag);
			auto item = static_cast<account::CommentUWPitem^>(node->Data);
			auto vvvvv = &item->helper;
			
			auto rbox = ref new ReplyBox(vvvvv);
			rbox->Tag = node;
			rbox->CommentSubmitted += ref new Windows::Foundation::EventHandler<account::CommentUWPitem ^>([s = Platform::WeakReference(sender)](Platform::Object ^ sender, account::CommentUWPitem ^) {
				s.Resolve<AppBarToggleButton>()->IsChecked = false;
				static_cast<ReplyBox^>(sender)->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			});
			rbox->CommentSubmitted += ref new Windows::Foundation::EventHandler<account::CommentUWPitem ^>(this, &Reddit::CommentView::OnCommentSubmitted);
			g->Children->Append(rbox);
			g->SetRow(rbox, 3);
			rbutton->Tag = rbox;
		}
		else
		{
			static_cast<ReplyBox^>(rbutton->Tag)->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
	}
	else
	{
		if (rbutton->Tag != nullptr)
		{
			static_cast<ReplyBox^>(rbutton->Tag)->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
	}
}




void Reddit::CommentView::OnCommentSubmitted(Platform::Object ^sender, account::CommentUWPitem ^args)
{
	auto node = static_cast<TreeViewControl::TreeNode^>(static_cast<ReplyBox^>(sender)->Tag);
	auto data = static_cast<account::CommentUWPitem^>(node->Data);
	if (data->replies == nullptr)
	{
		data->replies = new account::commentUWPlisting();
	}
	//data->replies->commentList.push_front(args);
	auto newnode = ref new TreeViewControl::TreeNode();
	newnode->Data = args;
	node->InsertAt(0, newnode);

	
}

Platform::Object ^ Reddit::DepthToColor::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
{
	if (static_cast<int>(value)&1)
	{
		static auto brush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::LightGray);
		return brush;
	}
	else
	{
		static auto whitebrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::White);
		return whitebrush;
	}
}
