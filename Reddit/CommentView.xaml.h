//
// CommentView.xaml.h
// Declaration of the CommentView class
//

#pragma once
#include "converters.h"
#include "CommentView.g.h"
#include "commentUWPitem.h"
#include <unordered_set>
namespace Reddit
{
	

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CommentView sealed
	{
	public:
		CommentView();
		property Platform::String^ postID
		{
			void set(Platform::String^ in);
		}
	internal:
		void SetVec(std::vector<account::CommentUWPitem^> vec);
	private:
		const Windows::UI::Xaml::Thickness itemBorder =
		{-8,0,0,0};
		std::unordered_set<Windows::UI::Xaml::UIElement^, converters::MyObjectHash> commentRecycle;
		std::unordered_set<Windows::UI::Xaml::UIElement^, converters::MyObjectHash> moreRecycle;
		static void recursiveNode(TreeViewControl::TreeNode^ par, account::CommentUWPitem^ com);
		Platform::String^ _postID;
		void commentTree_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args);
		Windows::UI::Xaml::DataTemplate^ CommentDataTemplate;
		Windows::UI::Xaml::DataTemplate^ GetMoreDataTemplate;
	};
}

