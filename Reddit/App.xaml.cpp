//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "serviceHelpers.h"
#include "ApplicationDataHelper.h"
#include "SubRedditViewPage.xaml.h"
#include "rootWindowGrid.xaml.h"
#include <deque>
using namespace Reddit;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
Windows::UI::Xaml::Navigation::PageStackEntry^ App::CurrentManualFrameContent = nullptr;
App::App()
{
    InitializeComponent();
	listTemplateRes = ref new MyResources();
	
	mdblock::mdRichProperties::registerProperties();
	//__debugbreak();
	globalvars::generalHttp = ref new Windows::Web::Http::HttpClient();
	globalvars::generalHttp->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue("I heard you liked user agents"));
	globalvars::imgurHttp = ref new Windows::Web::Http::HttpClient();
	globalvars::imgurHttp->DefaultRequestHeaders->Authorization = ref new Windows::Web::Http::Headers::HttpCredentialsHeaderValue("Client-ID", "713b63d6f4df36b");
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
	this->Resources->MergedDictionaries->Append(listTemplateRes);
	AppBarToggleColoredButton::registerDependencyProperty();
	navCore = dynamic_cast<rootWindowGrid^ > (Window::Current->Content);
   

	if (navCore == nullptr)
	{
		navCore = ref new rootWindowGrid();
		navCore->rootFrame->Navigated += ref new Windows::UI::Xaml::Navigation::NavigatedEventHandler(this, &App::OnNavigated);
		Window::Current->Content = navCore;
		Window::Current->Activate();
	}
    //if (rootFrame == nullptr)
    //{
    //    // Create a Frame to act as the navigation context and associate it with
    //    // a SuspensionManager key
 
    //}
    //else
    //{
    //    if (e->PrelaunchActivated == false)
    //    {
    //        if (rootFrame->Content == nullptr)
    //        {
    //            // When the navigation stack isn't restored navigate to the first page,
    //            // configuring the new page by passing required information as a navigation
    //            // parameter
				//if (e->Arguments == nullptr)
				//{
				//	rootFrame->Navigate(TypeName(SubRedditViewPage::typeid), globalvars::addNav(new subredditNavstate()));
				//}
				//else
				//{
				//	__debugbreak();
				//}
    //        }
    //        // Ensure the current window is active
    //        Window::Current->Activate();
    //    }
    //}
	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->
		BackRequested += ref new Windows::Foundation::EventHandler<
		Windows::UI::Core::BackRequestedEventArgs^>(
			this, &App::App_BackRequested);


}

void Reddit::App::OnNavigated(Platform::Object ^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	if (e->NavigationMode == Windows::UI::Xaml::Navigation::NavigationMode::New)
	{
		if (CurrentManualFrameContent != nullptr)
		{
			static_cast<Windows::UI::Xaml::Controls::Frame^>(sender)->BackStack->Append(std::move(CurrentManualFrameContent));
			CurrentManualFrameContent = nullptr;
		}
	}
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void) sender;  // Unused parameter
    (void) e;   // Unused parameter

    //TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}



void Reddit::App::App_BackRequested(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (navCore == nullptr)
		return;
	if (navCore->rootFrame->CanGoBack && e->Handled == false)
	{
		
		e->Handled = true;
		auto backstack = ref new Platform::Collections::Vector<Windows::UI::Xaml::Navigation::PageStackEntry^>(Windows::Foundation::Collections::begin(navCore->rootFrame->BackStack), Windows::Foundation::Collections::end(navCore->rootFrame->BackStack));
		auto forwardstack = ref new Platform::Collections::Vector<Windows::UI::Xaml::Navigation::PageStackEntry^>(Windows::Foundation::Collections::begin(navCore->rootFrame->ForwardStack), Windows::Foundation::Collections::end(navCore->rootFrame->ForwardStack));
		const auto & pagestack = navCore->rootFrame->BackStack->GetAt(navCore->rootFrame->BackStack->Size - 1);
		const auto & i = globalvars::NavState[static_cast<unsigned char>(pagestack->Parameter)].second;
		
		auto const & currentPage = static_cast<NavPage^>(navCore->rootFrame->Content);
		int currentIndex = currentPage->NavigationIndex;
		
		if (i->pageState == nullptr)
		{
			navCore->rootFrame->GoBack();
			if ((CurrentManualFrameContent != nullptr) && (currentIndex == static_cast<unsigned char>(CurrentManualFrameContent->Parameter)))
			{
				navCore->rootFrame->ForwardStack->InsertAt(0, CurrentManualFrameContent);
				CurrentManualFrameContent = nullptr;
			}
		}
		else
		{
			
			if ((CurrentManualFrameContent != nullptr) && (currentIndex == static_cast<unsigned char>(CurrentManualFrameContent->Parameter)))
			{
				navCore->rootFrame->ForwardStack->InsertAt(0, CurrentManualFrameContent);
			}
			else
			{
				navCore->rootFrame->ForwardStack->InsertAt(0, ref new Windows::UI::Xaml::Navigation::PageStackEntry(navCore->rootFrame->CurrentSourcePageType, static_cast<unsigned char>(currentIndex), nullptr));
			}
			CurrentManualFrameContent = pagestack;
			navCore->rootFrame->BackStack->RemoveAtEnd();
			navCore->rootFrame->Content = i->pageState;
		}
	}
}




void Reddit::App::recursiveInline(Windows::UI::Xaml::Documents::InlineCollection ^ in, std::vector<std::tuple<Windows::UI::Xaml::Documents::InlineCollection^, unsigned int, Windows::Foundation::Uri^>>& vec)
{
	unsigned int index = 0;
	for (auto &&x : in)
	{
		auto spanCast = dynamic_cast<Windows::UI::Xaml::Documents::Span^>(static_cast<Platform::Object^>(x));
		if (spanCast)
		{
			auto hcast = dynamic_cast<Windows::UI::Xaml::Documents::Hyperlink^>(spanCast);
			if (hcast)
			{
				vec.emplace_back(in, index, hcast->NavigateUri);
			}
			else
			{
				recursiveInline(spanCast->Inlines, vec);
			}
		}
		index++;
	}
}
