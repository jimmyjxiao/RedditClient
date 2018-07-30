﻿#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

#include "XamlBindingInfo.g.h"

namespace Windows {
    namespace UI {
        namespace Xaml {
            ref class AdaptiveTrigger;
        }
    }
}
namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class CommandBar;
                ref class StackPanel;
                ref class SplitView;
                ref class WebView;
                ref class GridView;
                ref class ListView;
                ref class AppBarSeparator;
                ref class TextBlock;
                ref class AppBarToggleButton;
                ref class AppBarButton;
            }
        }
    }
}

namespace Reddit
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class SubRedditViewPage : public ::Reddit::NavPage, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector,
        public ::Windows::UI::Xaml::Markup::IComponentConnector2
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
        virtual ::Windows::UI::Xaml::Markup::IComponentConnector^ GetBindingConnector(int connectionId, ::Platform::Object^ target);
    
    private:
        void UnloadObject(::Windows::UI::Xaml::DependencyObject^ dependencyObject);
        void DisconnectUnloadedObject(int connectionId);
    
    private:
        bool _contentLoaded;
    
        class SubRedditViewPage_obj1_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        private: ::Windows::UI::Xaml::AdaptiveTrigger^ mystate;
        private: ::Windows::UI::Xaml::Controls::CommandBar^ commandBar;
        private: ::Windows::UI::Xaml::Controls::StackPanel^ subInfopanel;
        private: ::Windows::UI::Xaml::Controls::SplitView^ splitSide;
        private: ::Windows::UI::Xaml::Controls::WebView^ sidebarWebview;
        private: ::Windows::UI::Xaml::Controls::GridView^ listGrid;
        private: ::Windows::UI::Xaml::Controls::ListView^ listView;
        private: ::Windows::UI::Xaml::Controls::AppBarSeparator^ seppy;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ subTextblock;
        private: ::Windows::UI::Xaml::Controls::AppBarToggleButton^ sideBarButton;
        private: ::Windows::UI::Xaml::Controls::AppBarButton^ timeButton;
        private: ::Windows::UI::Xaml::Controls::ListView^ timeSelector;
        private: ::Windows::UI::Xaml::Controls::ListView^ sortSelector;
    };
}

