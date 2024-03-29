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
                ref class RowDefinition;
                ref class Grid;
                ref class Frame;
                ref class AutoSuggestBox;
                ref class ComboBox;
            }
        }
    }
}
namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Shapes {
                ref class Rectangle;
            }
        }
    }
}

namespace Reddit
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class rootWindowGrid : public ::Windows::UI::Xaml::Controls::Grid, 
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
    
        class rootWindowGrid_obj7_Bindings;
        class rootWindowGrid_obj14_Bindings;
        class rootWindowGrid_obj18_Bindings;
        class rootWindowGrid_obj1_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        internal: ::Windows::UI::Xaml::Controls::RowDefinition^ titleRowDef;
        private: ::Windows::UI::Xaml::Controls::Grid^ titleGrid;
        internal: ::Windows::UI::Xaml::Controls::Frame^ rootFrame;
        private: ::Windows::UI::Xaml::Shapes::Rectangle^ titleRec;
        private: ::Windows::UI::Xaml::Controls::AutoSuggestBox^ searchBox;
        private: ::Windows::UI::Xaml::Controls::ComboBox^ accountBox;
    };
}

