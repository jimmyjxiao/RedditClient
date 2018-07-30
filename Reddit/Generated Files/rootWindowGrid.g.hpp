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

#include "rootWindowGrid.xaml.h"

void ::Reddit::rootWindowGrid::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///rootWindowGrid.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}


/// <summary>
/// Auto generated class for compiled bindings.
/// </summary>
class Reddit::rootWindowGrid::rootWindowGrid_obj7_Bindings 
    : public ::XamlBindingInfo::ValueTypeXamlBindings<::account::subredditInfo, ::XamlBindingInfo::XamlBindingTrackingBase>
{
            ::Windows::UI::Xaml::ResourceDictionary^ localResources;
            ::Platform::WeakReference converterLookupRoot;
public:
    rootWindowGrid_obj7_Bindings()
    {
    }

    void Connect(int __connectionId, ::Platform::Object^ __target)
    {
        switch(__connectionId)
        {
            case 7: // rootWindowGrid.xaml line 104
                this->obj7 = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
                break;
            case 8: // rootWindowGrid.xaml line 113
                this->obj8 = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
                break;
            case 9: // rootWindowGrid.xaml line 117
                this->obj9 = safe_cast<::Windows::UI::Xaml::Controls::SymbolIcon^>(__target);
                break;
            case 10: // rootWindowGrid.xaml line 115
                this->obj10 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
        }
    }

    void DisconnectUnloadedObject(int connectionId)
    {
        throw ref new ::Platform::InvalidArgumentException("No unloadable elements to disconnect.");
    }

    void Recycle()
    {
    }

    void ProcessBindings(::Platform::Object^ item, int itemIndex, int phase, int* nextPhase)
    {
        switch (phase)
        {
            case 0:
                *nextPhase = -1;
                this->SetDataRoot(static_cast<::account::subredditInfo>(item));
                if (this->_dataContextChangedToken.Value != 0)
                {
                    this->obj7.Resolve<::Windows::UI::Xaml::Controls::Grid>()->DataContextChanged -= this->_dataContextChangedToken;
                    this->_dataContextChangedToken.Value = 0;
                }
                this->_isInitialized = true;
                break;
            default:
                *nextPhase = -1;
                break;
        }
        this->Update_((::account::subredditInfo) item, 1 << phase);
    }

    void SetConverterLookupRoot(::Windows::UI::Xaml::FrameworkElement^ rootElement)
    {
        this->converterLookupRoot = rootElement;
    }

    ::Windows::UI::Xaml::Data::IValueConverter^ LookupConverter(::Platform::String^ key)
    {
        if (this->localResources == nullptr)
        {
            this->localResources = this->converterLookupRoot.Resolve<::Windows::UI::Xaml::FrameworkElement>()->Resources;
            this->converterLookupRoot = nullptr;
            }
        return safe_cast<::Windows::UI::Xaml::Data::IValueConverter^>(this->localResources->HasKey(key) ? this->localResources->Lookup(key) : ::Windows::UI::Xaml::Application::Current->Resources->Lookup(key));
    }

private:
    // Fields for each control that has bindings.
    ::Platform::WeakReference obj7;
    ::Windows::UI::Xaml::Controls::TextBlock^ obj8;
    ::Windows::UI::Xaml::Controls::SymbolIcon^ obj9;
    ::Windows::UI::Xaml::Documents::Run^ obj10;

    // Update methods for each path node used in binding steps.
    void Update_(::account::subredditInfo obj, int phase)
    {
            if ((phase & (NOT_PHASED | (1 << 0))) != 0)
            {
                this->Update_pname(obj.pname, phase);
                this->Update_subscribed(obj.subscribed, phase);
                this->Update_subscribers(obj.subscribers, phase);
            }
    }
    void Update_pname(::Platform::String^ obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 113
            Set_Windows_UI_Xaml_Controls_TextBlock_Text(this->obj8, obj, nullptr);
        }
    }
    void Update_subscribed(::default::int64 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 117
            Set_Windows_UI_Xaml_UIElement_Visibility(this->obj9, safe_cast<::Windows::UI::Xaml::Visibility>(this->LookupConverter("SubscribedVisibility")->Convert(obj, ::Windows::UI::Xaml::Visibility::typeid, nullptr, nullptr)));
        }
    }
    void Update_subscribers(::default::uint32 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 115
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj10, obj.ToString(), nullptr);
        }
    }
    static void Set_Windows_UI_Xaml_Controls_TextBlock_Text(::Windows::UI::Xaml::Controls::TextBlock^ obj, ::Platform::String^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = targetNullValue;
        }
        obj->Text = value;
    }
    static void Set_Windows_UI_Xaml_UIElement_Visibility(::Windows::UI::Xaml::UIElement^ obj, ::Windows::UI::Xaml::Visibility value)
    {
        obj->Visibility = value;
    }
    static void Set_Windows_UI_Xaml_Documents_Run_Text(::Windows::UI::Xaml::Documents::Run^ obj, ::Platform::String^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = targetNullValue;
        }
        obj->Text = value;
    }
};

/// <summary>
/// Auto generated class for compiled bindings.
/// </summary>
class Reddit::rootWindowGrid::rootWindowGrid_obj14_Bindings 
    : public ::XamlBindingInfo::ValueTypeXamlBindings<::account::AccountInfo, ::XamlBindingInfo::XamlBindingTrackingBase>
{
public:
    rootWindowGrid_obj14_Bindings()
    {
    }

    void Connect(int __connectionId, ::Platform::Object^ __target)
    {
        switch(__connectionId)
        {
            case 14: // rootWindowGrid.xaml line 41
                this->obj14 = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
                break;
            case 15: // rootWindowGrid.xaml line 51
                this->obj15 = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
                break;
            case 16: // rootWindowGrid.xaml line 54
                this->obj16 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
            case 17: // rootWindowGrid.xaml line 54
                this->obj17 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
        }
    }

    void DisconnectUnloadedObject(int connectionId)
    {
        throw ref new ::Platform::InvalidArgumentException("No unloadable elements to disconnect.");
    }

    void Recycle()
    {
    }

    void ProcessBindings(::Platform::Object^ item, int itemIndex, int phase, int* nextPhase)
    {
        switch (phase)
        {
            case 0:
                *nextPhase = -1;
                this->SetDataRoot(static_cast<::account::AccountInfo>(item));
                if (this->_dataContextChangedToken.Value != 0)
                {
                    this->obj14.Resolve<::Windows::UI::Xaml::Controls::Grid>()->DataContextChanged -= this->_dataContextChangedToken;
                    this->_dataContextChangedToken.Value = 0;
                }
                this->_isInitialized = true;
                break;
            default:
                *nextPhase = -1;
                break;
        }
        this->Update_((::account::AccountInfo) item, 1 << phase);
    }

private:
    // Fields for each control that has bindings.
    ::Platform::WeakReference obj14;
    ::Windows::UI::Xaml::Controls::TextBlock^ obj15;
    ::Windows::UI::Xaml::Documents::Run^ obj16;
    ::Windows::UI::Xaml::Documents::Run^ obj17;

    // Update methods for each path node used in binding steps.
    void Update_(::account::AccountInfo obj, int phase)
    {
            if ((phase & (NOT_PHASED | (1 << 0))) != 0)
            {
                this->Update_username(obj.username, phase);
                this->Update_comment_karma(obj.comment_karma, phase);
                this->Update_link_karma(obj.link_karma, phase);
            }
    }
    void Update_username(::Platform::String^ obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 51
            Set_Windows_UI_Xaml_Controls_TextBlock_Text(this->obj15, obj, nullptr);
        }
    }
    void Update_comment_karma(::default::int32 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 54
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj16, obj.ToString(), nullptr);
        }
    }
    void Update_link_karma(::default::int32 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 54
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj17, obj.ToString(), nullptr);
        }
    }
    static void Set_Windows_UI_Xaml_Controls_TextBlock_Text(::Windows::UI::Xaml::Controls::TextBlock^ obj, ::Platform::String^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = targetNullValue;
        }
        obj->Text = value;
    }
    static void Set_Windows_UI_Xaml_Documents_Run_Text(::Windows::UI::Xaml::Documents::Run^ obj, ::Platform::String^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = targetNullValue;
        }
        obj->Text = value;
    }
};

/// <summary>
/// Auto generated class for compiled bindings.
/// </summary>
class Reddit::rootWindowGrid::rootWindowGrid_obj18_Bindings 
    : public ::XamlBindingInfo::ValueTypeXamlBindings<::account::AccountInfo, ::XamlBindingInfo::XamlBindingTrackingBase>
{
public:
    rootWindowGrid_obj18_Bindings()
    {
    }

    void Connect(int __connectionId, ::Platform::Object^ __target)
    {
        switch(__connectionId)
        {
            case 18: // rootWindowGrid.xaml line 62
                this->obj18 = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
                break;
            case 19: // rootWindowGrid.xaml line 63
                this->obj19 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
            case 21: // rootWindowGrid.xaml line 63
                this->obj21 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
            case 22: // rootWindowGrid.xaml line 63
                this->obj22 = safe_cast<::Windows::UI::Xaml::Documents::Run^>(__target);
                break;
        }
    }

    void DisconnectUnloadedObject(int connectionId)
    {
        throw ref new ::Platform::InvalidArgumentException("No unloadable elements to disconnect.");
    }

    void Recycle()
    {
    }

    void ProcessBindings(::Platform::Object^ item, int itemIndex, int phase, int* nextPhase)
    {
        switch (phase)
        {
            case 0:
                *nextPhase = -1;
                this->SetDataRoot(static_cast<::account::AccountInfo>(item));
                if (this->_dataContextChangedToken.Value != 0)
                {
                    this->obj18.Resolve<::Windows::UI::Xaml::Controls::TextBlock>()->DataContextChanged -= this->_dataContextChangedToken;
                    this->_dataContextChangedToken.Value = 0;
                }
                this->_isInitialized = true;
                break;
            default:
                *nextPhase = -1;
                break;
        }
        this->Update_((::account::AccountInfo) item, 1 << phase);
    }

private:
    // Fields for each control that has bindings.
    ::Platform::WeakReference obj18;
    ::Windows::UI::Xaml::Documents::Run^ obj19;
    ::Windows::UI::Xaml::Documents::Run^ obj21;
    ::Windows::UI::Xaml::Documents::Run^ obj22;

    // Update methods for each path node used in binding steps.
    void Update_(::account::AccountInfo obj, int phase)
    {
            if ((phase & (NOT_PHASED | (1 << 0))) != 0)
            {
                this->Update_username(obj.username, phase);
                this->Update_link_karma(obj.link_karma, phase);
                this->Update_comment_karma(obj.comment_karma, phase);
            }
    }
    void Update_username(::Platform::String^ obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 63
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj19, obj, nullptr);
        }
    }
    void Update_link_karma(::default::int32 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 63
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj21, obj.ToString(), nullptr);
        }
    }
    void Update_comment_karma(::default::int32 obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED )) != 0)
        {
            // rootWindowGrid.xaml line 63
            Set_Windows_UI_Xaml_Documents_Run_Text(this->obj22, obj.ToString(), nullptr);
        }
    }
    static void Set_Windows_UI_Xaml_Documents_Run_Text(::Windows::UI::Xaml::Documents::Run^ obj, ::Platform::String^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = targetNullValue;
        }
        obj->Text = value;
    }
};

/// <summary>
/// Auto generated class for compiled bindings.
/// </summary>
class Reddit::rootWindowGrid::rootWindowGrid_obj1_Bindings 
    : public ::XamlBindingInfo::ReferenceTypeXamlBindings<::Reddit::rootWindowGrid, ::XamlBindingInfo::XamlBindingTrackingBase>
    , public ::XamlBindingInfo::IXamlBindingTracking
{
public:
    rootWindowGrid_obj1_Bindings()
    {
        InitializeTracking(this);
    }

private:
    ~rootWindowGrid_obj1_Bindings()
    {
        ReleaseAllListeners();
    }

public:

    void Connect(int __connectionId, ::Platform::Object^ __target)
    {
        switch(__connectionId)
        {
            case 12: // rootWindowGrid.xaml line 78
                this->obj12 = safe_cast<::Reddit::NoToggleBehaviorButton^>(__target);
                break;
        }
    }

    void DisconnectUnloadedObject(int connectionId)
    {
        throw ref new ::Platform::InvalidArgumentException("No unloadable elements to disconnect.");
    }

private:
    // Fields for each control that has bindings.
    ::Reddit::NoToggleBehaviorButton^ obj12;
    
    // Fields for binding tracking.
    ::Platform::WeakReference cachePC_;
    ::Windows::Foundation::EventRegistrationToken tokenPC_;

    // Update methods for each path node used in binding steps.
    void Update_(::Reddit::rootWindowGrid^ obj, int phase)
    {
        this->_bindingsTracking->UpdatePropertyChangedListener(obj, this->cachePC_, &tokenPC_);
        if (obj != nullptr)
        {
            if ((phase & (NOT_PHASED | DATA_CHANGED | (1 << 0))) != 0)
            {
                this->Update_Has_mail(obj->Has_mail, phase);
            }
        }
    }
    void Update_Has_mail(::Platform::Boolean obj, int phase)
    {
        if((phase & ((1 << 0) | NOT_PHASED | DATA_CHANGED)) != 0)
        {
            // rootWindowGrid.xaml line 78
            Set_Windows_UI_Xaml_Controls_Primitives_ToggleButton_IsChecked(this->obj12, obj, nullptr);
        }
    }

    virtual void ReleaseAllListeners() override
    {
        this->_bindingsTracking->UpdatePropertyChangedListener(nullptr, this->cachePC_, &tokenPC_);
    }

    virtual void PropertyChanged(Platform::Object^ sender, ::Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e) override
    {
        if (this->GetDataRoot() != nullptr && this->GetDataRoot()->Equals(sender))
        {
            ::Platform::String^ propName = e->PropertyName;
            ::Reddit::rootWindowGrid^ obj = safe_cast<::Reddit::rootWindowGrid^>(sender);
            if (propName == nullptr || propName->IsEmpty())
            {
                if (obj != nullptr)
                {
                    this->Update_Has_mail(obj->Has_mail, DATA_CHANGED);
                }
            }
            else if (propName == "Has_mail")
            {
                if (obj != nullptr)
                {
                    this->Update_Has_mail(obj->Has_mail, DATA_CHANGED);
                }
            }
        }
    }

    void CollectionChanged(::Platform::Object^ sender, ::Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs^ e)
    {
    }

    void VectorChanged(::Platform::Object^ sender, ::Windows::Foundation::Collections::IVectorChangedEventArgs^ e)
    {
    }

    void MapChanged(::Platform::Object^ sender, ::Windows::Foundation::Collections::IMapChangedEventArgs<::Platform::String^>^ e)
    {
    }

    void DependencyPropertyChanged(::Windows::UI::Xaml::DependencyObject^ sender, ::Windows::UI::Xaml::DependencyProperty^ prop)
    {
        if (sender != nullptr)
        {
        }
    }
    static void Set_Windows_UI_Xaml_Controls_Primitives_ToggleButton_IsChecked(::Windows::UI::Xaml::Controls::Primitives::ToggleButton^ obj, ::Platform::IBox<::Platform::Boolean>^ value, ::Platform::String^ targetNullValue)
    {
        if (value == nullptr && targetNullValue != nullptr)
        {
            value = (::Platform::IBox<::Platform::Boolean>^) ::Windows::UI::Xaml::Markup::XamlBindingHelper::ConvertValue(::Platform::IBox<::Platform::Boolean>::typeid, targetNullValue);
        }
        obj->IsChecked = value;
    }
};

void ::Reddit::rootWindowGrid::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
    case 2:
        {
            this->titleRowDef = safe_cast<::Windows::UI::Xaml::Controls::RowDefinition^>(__target);
        }
        break;
    case 3:
        {
            this->titleGrid = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
        }
        break;
    case 4:
        {
            this->rootFrame = safe_cast<::Windows::UI::Xaml::Controls::Frame^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Frame^>(this->rootFrame))->Navigated += ref new ::Windows::UI::Xaml::Navigation::NavigatedEventHandler(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Navigation::NavigationEventArgs^))&rootWindowGrid::rootFrame_Navigated);
        }
        break;
    case 5:
        {
            this->titleRec = safe_cast<::Windows::UI::Xaml::Shapes::Rectangle^>(__target);
        }
        break;
    case 6:
        {
            this->searchBox = safe_cast<::Windows::UI::Xaml::Controls::AutoSuggestBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::AutoSuggestBox^>(this->searchBox))->TextChanged += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::Controls::AutoSuggestBox^, ::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^>(this, (void (::Reddit::rootWindowGrid::*)
                (::Windows::UI::Xaml::Controls::AutoSuggestBox^, ::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^))&rootWindowGrid::AutoSuggestBox_TextChanged);
            (safe_cast<::Windows::UI::Xaml::Controls::AutoSuggestBox^>(this->searchBox))->QuerySubmitted += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::Controls::AutoSuggestBox^, ::Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^>(this, (void (::Reddit::rootWindowGrid::*)
                (::Windows::UI::Xaml::Controls::AutoSuggestBox^, ::Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^))&rootWindowGrid::AutoSuggestBox_QuerySubmitted);
        }
        break;
    case 11:
        {
            this->accountBox = safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->accountBox))->Loaded += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&rootWindowGrid::Grid_Loaded);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->accountBox))->SelectionChanged += ref new ::Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::SelectionChangedEventArgs^))&rootWindowGrid::ComboBox_SelectionChanged);
        }
        break;
    case 12:
        {
            ::Reddit::NoToggleBehaviorButton^ element12 = safe_cast<::Reddit::NoToggleBehaviorButton^>(__target);
            (safe_cast<::Reddit::NoToggleBehaviorButton^>(element12))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&rootWindowGrid::Flyout_Open);
        }
        break;
    case 13:
        {
            ::Windows::UI::Xaml::Controls::Flyout^ element13 = safe_cast<::Windows::UI::Xaml::Controls::Flyout^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Flyout^>(element13))->Opening += ref new ::Windows::Foundation::EventHandler<::Platform::Object^>(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Platform::Object^))&rootWindowGrid::Flyout_Opening);
        }
        break;
    case 20:
        {
            ::Windows::UI::Xaml::Documents::Hyperlink^ element20 = safe_cast<::Windows::UI::Xaml::Documents::Hyperlink^>(__target);
            (safe_cast<::Windows::UI::Xaml::Documents::Hyperlink^>(element20))->Click += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::Documents::Hyperlink^, ::Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^>(this, (void (::Reddit::rootWindowGrid::*)
                (::Windows::UI::Xaml::Documents::Hyperlink^, ::Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^))&rootWindowGrid::Hyperlink_Click);
        }
        break;
    case 24:
        {
            ::Windows::UI::Xaml::Controls::MenuFlyoutItem^ element24 = safe_cast<::Windows::UI::Xaml::Controls::MenuFlyoutItem^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::MenuFlyoutItem^>(element24))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Reddit::rootWindowGrid::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&rootWindowGrid::MenuFlyoutItem_Click);
        }
        break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::Reddit::rootWindowGrid::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    ::XamlBindingInfo::XamlBindings^ bindings = nullptr;
    switch (__connectionId)
    {
        case 1: // rootWindowGrid.xaml line 1
            {
                ::Windows::UI::Xaml::Controls::Grid^ element1 = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
                rootWindowGrid_obj1_Bindings* objBindings = new rootWindowGrid_obj1_Bindings();
                objBindings->SetDataRoot(this);
                bindings = ref new ::XamlBindingInfo::XamlBindings(objBindings);
                this->Bindings = bindings;
                element1->Loading += ref new ::Windows::Foundation::TypedEventHandler<::Windows::UI::Xaml::FrameworkElement^, ::Platform::Object^>(bindings, &::XamlBindingInfo::XamlBindings::Loading);
            }
            break;
        case 7: // rootWindowGrid.xaml line 104
            {
                ::Windows::UI::Xaml::Controls::Grid^ element7 = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
                rootWindowGrid_obj7_Bindings* objBindings = new rootWindowGrid_obj7_Bindings();
                objBindings->SetDataRoot(element7->DataContext);
                objBindings->SetConverterLookupRoot(this);
                bindings = ref new ::XamlBindingInfo::XamlBindings(objBindings);
                bindings->SubscribeForDataContextChanged(element7);
                ::Windows::UI::Xaml::DataTemplate::SetExtensionInstance(element7, bindings);
                ::Windows::UI::Xaml::Markup::XamlBindingHelper::SetDataTemplateComponent(element7, bindings);
            }
            break;
        case 14: // rootWindowGrid.xaml line 41
            {
                ::Windows::UI::Xaml::Controls::Grid^ element14 = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
                rootWindowGrid_obj14_Bindings* objBindings = new rootWindowGrid_obj14_Bindings();
                objBindings->SetDataRoot(element14->DataContext);
                bindings = ref new ::XamlBindingInfo::XamlBindings(objBindings);
                bindings->SubscribeForDataContextChanged(element14);
                ::Windows::UI::Xaml::DataTemplate::SetExtensionInstance(element14, bindings);
                ::Windows::UI::Xaml::Markup::XamlBindingHelper::SetDataTemplateComponent(element14, bindings);
            }
            break;
        case 18: // rootWindowGrid.xaml line 62
            {
                ::Windows::UI::Xaml::Controls::TextBlock^ element18 = safe_cast<::Windows::UI::Xaml::Controls::TextBlock^>(__target);
                rootWindowGrid_obj18_Bindings* objBindings = new rootWindowGrid_obj18_Bindings();
                objBindings->SetDataRoot(element18->DataContext);
                bindings = ref new ::XamlBindingInfo::XamlBindings(objBindings);
                bindings->SubscribeForDataContextChanged(element18);
                ::Windows::UI::Xaml::DataTemplate::SetExtensionInstance(element18, bindings);
                ::Windows::UI::Xaml::Markup::XamlBindingHelper::SetDataTemplateComponent(element18, bindings);
            }
            break;
    }
    return bindings;
}

#pragma warning(pop)


