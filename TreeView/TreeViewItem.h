#pragma once

namespace TreeViewControl {
    [Windows::Foundation::Metadata::WebHostHidden]
    [Windows::UI::Xaml::Data::Bindable]
    public ref class TreeViewItem sealed : Windows::UI::Xaml::Controls::ListViewItem
    {
    public:
        TreeViewItem();

        virtual ~TreeViewItem();

    private:
        Windows::UI::Xaml::Controls::ListView^ GetAncestorListView(TreeViewItem^ targetItem);

    protected:

        Windows::UI::Xaml::Automation::Peers::AutomationPeer^ OnCreateAutomationPeer() override;
	};
}