//
// App.xaml.h
// Declaration of the App class.
//

#pragma once
#include "converters.h"
#include "App.g.h"
#include "subpostUWP.h"
#include "MyResources.xaml.h"
#include "NoToggleBehaviorButton.h"
#include "contentResizer.h"
#include "rootWindowGrid.xaml.h"
namespace Reddit
{
	/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	public interface struct NavIndexed
	{
		property int NavigationIndex
		{
			int get();
		}
		property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get();
		}
	};
	ref class App sealed
	{
	public:
		
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		rootWindowGrid ^ navCore;
		App();
		MyResources^ listTemplateRes;
		static Windows::UI::Xaml::Navigation::PageStackEntry^ CurrentManualFrameContent;
	private:
		bool IgnoreNav = true;
		void OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
		
		void App::App_BackRequested(
			Platform::Object^ sender,
			Windows::UI::Core::BackRequestedEventArgs^ e);
		void recursiveInline(Windows::UI::Xaml::Documents::InlineCollection^ in, std::vector<std::tuple<Windows::UI::Xaml::Documents::InlineCollection^, unsigned int, Windows::Foundation::Uri^>>& vec);
	};
}
