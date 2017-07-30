//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "AccountInterface.h"
#include "globalvars.h"
#include "navstates.h"
#include "SubRedditViewPage.xaml.h"
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void Reddit::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	account::AccountInterface::LoginNewAcc().then([](account::AccountInterface* newaccount) {
		globalvars::currentacc = newaccount;
	});
}


void Reddit::MainPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto x = new subredditNavstate();
	x->_subreddit = "pics";
	globalvars::NavState.insert(std::make_pair(globalvars::navChar, (void*)x));
	globalvars::navChar++;
	testFrame->Navigate(SubRedditViewPage::typeid, unsigned char(globalvars::navChar - 1));
}
