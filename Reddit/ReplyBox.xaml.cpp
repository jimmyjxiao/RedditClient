//
// ReplyBox.xaml.cpp
// Implementation of the ReplyBox class
//

#include "pch.h"
#include "ReplyBox.xaml.h"
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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

ReplyBox::ReplyBox(account::VotableThing * p)
{
	parent = p;
	InitializeComponent();
	if (globalvars::appSettingCache.replyModeSetting == globalvars::settings::defaultReplyMode::lastUsed)
	{
		piv->SelectedIndex = (int)globalvars::appSettingCache.initialReplyMode;
	}
	else
	{
		piv->SelectedIndex = (int)globalvars::appSettingCache.replyModeSetting;
	}
}

Platform::String ^ Reddit::ReplyBox::getMarkdown()
{
	if (piv->SelectedIndex == 0)
	{
		//return fancyBox->getMarkdown();

	}
	else
	{
		return textBox->Text;
	}
}


void Reddit::ReplyBox::Pivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (globalvars::appSettingCache.replyModeSetting == globalvars::settings::defaultReplyMode::lastUsed)
		globalvars::appSettingCache.initialReplyMode = static_cast<globalvars::settings::ReplyMode>(piv->SelectedIndex);
	if (static_cast<Pivot^>(sender)->SelectedIndex == 0)
	{
		fancyBox->Box->markdown = textBox->Text;
	}
	else
	{
		textBox->Text = fancyBox->Box->markdown;
	}
}


void Reddit::ReplyBox::submitButton_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto b = static_cast<Button^>(sender);
	b->IsEnabled = false;
	busy->IsActive = true;
	parent->reply(getMarkdown()).then([this](account::CommentUWPitem^ newcomment) {
		busy->IsActive = false;
		CommentSubmitted(this, newcomment);
	});
}
