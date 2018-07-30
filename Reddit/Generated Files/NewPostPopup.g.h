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
                ref class TextBox;
                ref class TextBlock;
                ref class ToggleSwitch;
            }
        }
    }
}
namespace mdblock {
    ref class MDFullEnhancedEditor;
}

namespace Reddit
{
    [::Windows::Foundation::Metadata::WebHostHidden]
    partial ref class NewPostPopup : public ::Windows::UI::Xaml::Controls::ContentDialog, 
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
    
        class NewPostPopup_obj1_Bindings;
    
        ::XamlBindingInfo::XamlBindings^ Bindings;
        private: ::Windows::UI::Xaml::Controls::TextBox^ titleBox;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ charCounter;
        private: ::Windows::UI::Xaml::Controls::TextBox^ subbox;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ subErrorBox;
        private: ::Windows::UI::Xaml::Controls::ToggleSwitch^ NSFWSwitch;
        private: ::Windows::UI::Xaml::Controls::TextBlock^ contentHeader;
        private: ::Windows::UI::Xaml::Controls::ToggleSwitch^ editorSwitch;
        private: ::Windows::UI::Xaml::Controls::TextBox^ mdEditor;
        private: ::mdblock::MDFullEnhancedEditor^ WYSIWYG;
    };
}
