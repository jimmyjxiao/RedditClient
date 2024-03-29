#pragma once
#include "TreeNode.h"
#include "ViewModel.h"
#include "TreeViewItem.h"

namespace TreeViewControl {
    public ref class TreeViewItemClickEventArgs sealed
    {
    public:
        TreeViewItemClickEventArgs() {}

        property Object^ ClickedItem
        {
            Object^ get() { return clickedItem; };
            void set(Object^ value) { clickedItem = value; };
        }

        property bool IsHandled
        {
            bool get() { return isHandled; };
            void set(bool value) { isHandled = value; };
        }
    private:
        Object^ clickedItem = nullptr;
        bool isHandled = false;
    };

    ref class TreeView;
    [Windows::Foundation::Metadata::WebHostHidden]
    public delegate void TreeViewItemClickHandler(TreeView^ sender, TreeViewItemClickEventArgs^ args);

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class TreeView sealed : Windows::UI::Xaml::Controls::ListView
    {
    public:
        TreeView();

        //This event is used to expose an alternative to itemclick to developers.
        event TreeViewItemClickHandler^ TreeViewItemClick;
		Windows::UI::Xaml::DependencyObject^ PublicGetTemplateChild(Platform::String^ String)
		{
			return GetTemplateChild(std::move(String));
		}
        //This RootNode property is used by the TreeView to handle additions into the TreeView and
        //accurate VectorChange with multiple 'root level nodes'. This node will not be placed
        //in the flatViewModel, but has it's vectorchanged event hooked up to flatViewModel's
        //handler.
        property TreeNode^ RootNode
        {
            TreeNode^ get() { return rootNode; };
			void set(TreeNode^ newRoot);
        }

        void TreeView_OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ args);

        void ExpandNode(TreeNode^ targetNode);

        void CollapseNode(TreeNode^ targetNode);

    protected:
        void PrepareContainerForItemOverride(DependencyObject^ element, Object^ item) override;
        Windows::UI::Xaml::DependencyObject^ GetContainerForItemOverride() override;

    private:
        TreeNode^ rootNode;
        ViewModel^ flatViewModel;

    };
}

