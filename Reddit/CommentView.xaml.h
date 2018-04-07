//
// CommentView.xaml.h
// Declaration of the CommentView class
//

#pragma once
#include "converters.h"
#include "CommentView.g.h"
#include "commentUWPitem.h"
#include "subpostUWP.h"
#include <unordered_set>
#include "reportFlyout.h"
namespace Reddit
{
	

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommentView sealed
	{
	public:
		CommentView();
		
	internal:
		Platform::Collections::Vector<account::reportReason>^ reportingReasons;
		static void recursiveNode(TreeViewControl::TreeNode^ par, account::CommentUWPitem^ com);
		void setHeaderpost(account::subpostUWP^ post);
		void SetVec(std::vector<account::CommentUWPitem^> vec);
		void clear()
		{
			this->commentTree->RootNode->Clear();
		}
	protected:
		void OnApplyTemplate() override;
	private:
		std::function<void(account::commentUWPlisting*, account::CommentUWPitem^)> callbackfunc;
		static ExpanderControl::Expander^ findExpanderRecurse(Windows::UI::Xaml::FrameworkElement^ parent);
		bool contentExpanded = false;
		const Windows::UI::Xaml::Thickness itemBorder =
		{-8,0,0,0};
		std::unordered_set<Windows::UI::Xaml::UIElement^, converters::MyObjectHash> commentRecycle;
		std::unordered_set<Windows::UI::Xaml::UIElement^, converters::MyObjectHash> moreRecycle;
		
		void commentTree_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args);
		Windows::UI::Xaml::DataTemplate^ GetMoreDataTemplate;

		void commentTree_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void replyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnCommentSubmitted(Platform::Object ^sender, account::CommentUWPitem ^args);
	};
}

