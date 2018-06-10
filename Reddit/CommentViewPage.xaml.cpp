//
// CommentViewPage.xaml.cpp
// Implementation of the CommentViewPage class
//

#include "pch.h"
#include "CommentViewPage.xaml.h"
#include "ApplicationDataHelper.h"
#include "converters.h"
#include "serviceHelpers.h"
#include <concurrent_vector.h>
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

CommentViewPage::CommentViewPage()
{/*
	reportReasons = ref new Platform::Collections::Vector<account::reportReason>();
	Resources->Insert(ref new Platform::String(L"reportReasonsResource"), reportReasons);*/
	InitializeComponent();
}

void Reddit::CommentViewPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	navIndex = static_cast<unsigned char>(e->Parameter);
	nav = static_cast<commentNavstate*>(globalvars::NavState.at(navIndex).second);
	nav->pageState = this;
	auto list = globalvars::currentacc->getCommentAsyncVec(nav->postID, nav->sort);
	const char16_t* substr = (const char16_t*)nav->post->subreddit->Data();
	
	try
	{
		ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(substr, commentControl->reportingReasons);
	}
	catch (ApplicationDataHelper::cacheMiss<std::vector<account::reportReason>> m)
	{

		m.retrieveTask.then([this](std::vector<account::reportReason> z) {
			commentControl->reportingReasons->ReplaceAll(Platform::ArrayReference<account::reportReason>(z.data(), z.size()));
		});
	}
	if (nav->post != nullptr)
	{
		try {
			_subInfo = ApplicationDataHelper::subredditHelpers::trysubredditInfoCache(substr);
			
		}
		catch (ApplicationDataHelper::cacheMiss<account::subredditInfo> m)
		{
			m.retrieveTask.then([this](account::subredditInfo v){
				_subInfo = v;
			});
		}
		
		this->commentControl->setHeaderpost(nav->post);

	}
	if (nav->sort != account::commentSort::default)
	{
		sortSelector->SelectedIndex = (int)nav->sort;
	}
	list.then([this](account::commentUWPlisting z) {

		
		concurrency::parallel_for_each(z.commentList.begin(), z.commentList.end(), [](account::CommentUWPitem^ item) {
			try
			{
				item->cacheMDElements();
				for (auto && a : item->mdElements->Links)
				{
					//linkHandler::urlLookupTasks.insert(std::make_pair<int, concurrency::task<std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*>>>(a->GetHashCode(), account::serviceHelpers::urlHelper(a)));
				}
			}
			catch (...)
			{
				__debugbreak();
			}});
	
		this->listing = z;

		if (nav->sort == account::commentSort::default)
		{
			if (z.parent->helper.suggestedsort == account::commentSort::default)
			{
				nav->sort = account::commentSort::best;
				sortSelector->SelectedIndex = (int)account::commentSort::best;
			}
			else
			{
				nav->sort = z.parent->helper.suggestedsort;
				sortSelector->SelectedIndex = (int)nav->sort;
			}
			sortSelector->SelectedIndex = (int)nav->sort;
		}
		if (nav->post == nullptr)
		{
			try {
				_subInfo = ApplicationDataHelper::subredditHelpers::trysubredditInfoCache((const char16_t*)z.parent->subreddit->Data());
			}
			catch (...)
			{
				__debugbreak();
			}
			commentControl->setHeaderpost(z.parent);
		}

		commentControl->SetVec(z.commentList);
	});
}

void Reddit::CommentViewPage::OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs ^ e)
{
	nav->pageState = this;
}


void Reddit::CommentViewPage::sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	sort = static_cast<account::commentSort>(static_cast<ListView^>(sender)->SelectedValue);
}
void Reddit::CommentViewPage::sort::set(account::commentSort a)
{
	if (a != nav->sort)
	{
		commentControl->clear();
		auto newlist = globalvars::currentacc->getCommentAsyncVec(nav->postID, a);
		nav->sort = a;
		newlist.then([this](account::commentUWPlisting x) {this->listing = x; commentControl->SetVec(x.commentList); });
	}
}




void Reddit::CommentViewPage::AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
}


void Reddit::CommentViewPage::OnhtmlLoadingFinished(Platform::Object ^sender, Windows::UI::Xaml::Controls::RichTextBlock ^args)
{
}
