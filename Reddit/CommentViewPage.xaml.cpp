//
// CommentViewPage.xaml.cpp
// Implementation of the CommentViewPage class
//

#include "pch.h"
#include "CommentViewPage.xaml.h"
#include "NavStates.h"
#include "globalvars.h"
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
{
	InitializeComponent();
}

void Reddit::CommentViewPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto nav = static_cast<commentNavstate*>(globalvars::NavState.find(static_cast<unsigned char>(e->Parameter))->second);
	auto list = globalvars::currentacc->getCommentAsyncVec(nav->postID, nav->sort);
	list.then([this, nav](account::commentUWPlisting z) {
		commentControl->SetVec(z.commentList);
	});

}


void Reddit::CommentViewPage::sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

}
