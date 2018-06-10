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
            namespace Controls {
                ref class ControlTemplate;
            }
        }
    }
}
namespace Windows {
    namespace UI {
        namespace Xaml {
            ref class DataTemplate;
        }
    }
}

namespace Reddit
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class MyResources : public ::Windows::UI::Xaml::ResourceDictionary, 
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
    
        class MyResources_obj7_Bindings;
        class MyResources_obj24_Bindings;
        class MyResources_obj33_Bindings;
        class MyResources_obj44_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        private: ::Windows::UI::Xaml::Controls::ControlTemplate^ inlineExpanderButtoning;
        internal: ::Windows::UI::Xaml::DataTemplate^ listSelfPost;
    };
}

