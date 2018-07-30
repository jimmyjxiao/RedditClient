//
// ReplyBox.xaml.h
// Declaration of the ReplyBox class
//

#pragma once

#include "ReplyBox.g.h"
#include "VotableThing.h"
#include "commentUWPitem.h"
namespace Reddit
{


	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ReplyBox sealed
	{
	public:
		event Windows::Foundation::EventHandler<account::CommentUWPitem^>^ CommentSubmitted;
		Platform::String^ getMarkdown();
	internal:
		ReplyBox(account::VotableThing * p);
		account::VotableThing * parent;
	private:
		void Pivot_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void submitButton_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
