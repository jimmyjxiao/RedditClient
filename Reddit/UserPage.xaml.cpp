﻿//
// UserPage.xaml.cpp
// Implementation of the UserPage class
//

#include "pch.h"
#include "UserPage.xaml.h"

using namespace Reddit;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238
#define _NAV static_cast<UserNavState*>(baseRef)
UserPage::UserPage()
{
	InitializeComponent();
}

void Reddit::UserPage::OnNavigatedToPageCode()
{
	globalvars::currentacc->getMixedCollection("/user/" + _NAV->account.username + "/overview").then([this](Platform::Collections::Vector<account::IRedditTypeIdentifier^>^ v) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, v]() {
			Lview->ItemsSource = v;
		}));
	});
}