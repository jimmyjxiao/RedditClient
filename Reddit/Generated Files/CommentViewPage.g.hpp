﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "CommentViewPage.xaml.h"
#include "CommentView.xaml.h"

void ::Reddit::CommentViewPage::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///CommentViewPage.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::Reddit::CommentViewPage::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
    case 1:
        {
            this->commandBar = safe_cast<::Windows::UI::Xaml::Controls::CommandBar^>(__target);
        }
        break;
    case 2:
        {
            this->commentControl = safe_cast<::Reddit::CommentView^>(__target);
        }
        break;
    case 3:
        {
            this->sideBarButton = safe_cast<::Windows::UI::Xaml::Controls::AppBarToggleButton^>(__target);
        }
        break;
    case 4:
        {
            this->sortSelector = safe_cast<::Windows::UI::Xaml::Controls::ListView^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::ListView^>(this->sortSelector))->SelectionChanged += ref new ::Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, (void (::Reddit::CommentViewPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::SelectionChangedEventArgs^))&CommentViewPage::sortSelector_SelectionChanged);
        }
        break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::Reddit::CommentViewPage::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}

#pragma warning(pop)


