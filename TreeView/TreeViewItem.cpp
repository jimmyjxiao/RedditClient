#pragma once
#include "pch.h"
#include "TreeViewItem.h"
#include "TreeViewItemAutomationPeer.h"

using namespace Windows::UI::Xaml;
using namespace Platform;

namespace TreeViewControl {
    TreeViewItem::TreeViewItem()
    {
    }

    TreeViewItem::~TreeViewItem()
    {
    }

    Windows::UI::Xaml::Controls::ListView^ TreeViewItem::GetAncestorListView(TreeViewItem^ targetItem)
    {
        DependencyObject^ TreeViewItemAncestor = (DependencyObject^)this;
        Windows::UI::Xaml::Controls::ListView^ ancestorListView = nullptr;
        while (TreeViewItemAncestor != nullptr && ancestorListView == nullptr)
        {
            TreeViewItemAncestor = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(TreeViewItemAncestor);
            ancestorListView = dynamic_cast<Windows::UI::Xaml::Controls::ListView^>(TreeViewItemAncestor);
        }
        return ancestorListView;
    }

   

    Windows::UI::Xaml::Automation::Peers::AutomationPeer^ TreeViewItem::OnCreateAutomationPeer()
    {
        return ref new TreeViewItemAutomationPeer(this);
    }
}

