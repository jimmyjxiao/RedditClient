#pragma once
#include "pch.h"
#include "TreeView.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Controls;

namespace TreeViewControl {

    TreeView::TreeView()
    {
        flatViewModel = ref new ViewModel;
        rootNode = ref new TreeNode();
		flatViewModel->ExpandNode(rootNode);
		rootNode->VectorChanged += ref new BindableVectorChangedEventHandler(flatViewModel, &ViewModel::TreeNodeVectorChanged);
        ItemClick += ref new Windows::UI::Xaml::Controls::ItemClickEventHandler(this, &TreeView::TreeView_OnItemClick);
		//ItemsSource flatViewModel;
    }

    void TreeView::TreeView_OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ args)
    {
        TreeViewItemClickEventArgs^ treeViewArgs = ref new TreeViewItemClickEventArgs();
        treeViewArgs->ClickedItem = args->ClickedItem;

        TreeViewItemClick(this, treeViewArgs);

      /*  if (!treeViewArgs->IsHandled)
        {
            TreeNode^ targetNode = (TreeNode^)args->ClickedItem;
            if (targetNode->IsExpanded)
            {
                flatViewModel->CollapseNode(targetNode);
            }
            else
            {
                flatViewModel->ExpandNode(targetNode);                
            }
        }*/
    }
	void TreeView::RootNode::set(TreeNode^ newroot)
	{
		flatViewModel = ref new ViewModel(newroot->childrenVector);
		rootNode = newroot;
		flatViewModel->ExpandNode(rootNode);
		rootNode->VectorChanged += ref new BindableVectorChangedEventHandler(flatViewModel, &ViewModel::TreeNodeVectorChanged);
		ItemClick += ref new Windows::UI::Xaml::Controls::ItemClickEventHandler(this, &TreeView::TreeView_OnItemClick);
		ItemsSource = flatViewModel;
	}
   



    void TreeView::ExpandNode(TreeNode^ targetNode)
    {
        flatViewModel->ExpandNode(targetNode);
    }

    void TreeView::CollapseNode(TreeNode^ targetNode)
    {
        flatViewModel->CollapseNode(targetNode);
    }

    void TreeView::PrepareContainerForItemOverride(DependencyObject^ element, Object^ item)
    {
        ((UIElement^)element)->AllowDrop = true;

        ListView::PrepareContainerForItemOverride(element, item);
    }

    DependencyObject^ TreeView::GetContainerForItemOverride()
    {
        TreeViewItem^ targetItem = ref new TreeViewItem();
        return (DependencyObject^)targetItem;
    }

}