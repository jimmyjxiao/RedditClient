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
            ref class DataTemplate;
        }
    }
}
namespace TreeViewControl {
    ref class TreeView;
}

namespace Reddit
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class CommentView : public ::Windows::UI::Xaml::Controls::UserControl, 
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
    
        class CommentView_obj4_Bindings;
        class CommentView_obj7_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        private: ::Windows::UI::Xaml::DataTemplate^ commentTemplate;
        private: ::Windows::UI::Xaml::DataTemplate^ loadmorebutton;
        internal: ::TreeViewControl::TreeView^ commentTree;
    };
}

