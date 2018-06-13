//
// rootWindowGrid.xaml.h
// Declaration of the rootWindowGrid class
//

#pragma once
#include "NavStates.h"
#include "rootWindowGrid.g.h"
#include "NoToggleBehaviorButton.h"
#include "linkHandler.h"

namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public value struct subredditSuggestion
	{
		Platform::String^ name;
		unsigned int subscribers;
		bool subscribed;
	};
	public value struct userSuggestion
	{
		Platform::String^ name;
		int link_karma;
		int comment_karma;
	};
	public ref class comboboxTemplateSelector sealed :Windows::UI::Xaml::Controls::DataTemplateSelector
	{
	public:
		property Windows::UI::Xaml::DataTemplate^ listTemplate;
		property Windows::UI::Xaml::DataTemplate^ closedTemplate;
		property Windows::UI::Xaml::DataTemplate^ nullTemplate;
	protected:
		Windows::UI::Xaml::DataTemplate^  SelectTemplateCore(Platform::Object^ item, Windows::UI::Xaml::DependencyObject^ container) override;
	};
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class rootWindowGrid sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		property bool Has_mail {bool get(); }
		rootWindowGrid();
		static rootWindowGrid^ getCurrent();
		void updateAccountsToComboBox();
		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
		void NavigateToNewPage(const Windows::UI::Xaml::Interop::TypeName&const type, Platform::Object ^args);
	private:

		linkHandler ^ _linkHandler = linkHandler::getInstance();
		static rootWindowGrid ^ singleT;
		bool _isSearch = true;
		static bool wstrValidGo(const wchar_t* str, size_t size);
		Windows::UI::Xaml::Controls::SymbolIcon^ searchIcon;
		Windows::UI::Xaml::Controls::SymbolIcon^ goIcon;
		void CoreTitleBar_IsVisibleChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);
		void CoreTitleBar_LayoutMetricsChanged(Windows::ApplicationModel::Core::CoreApplicationViewTitleBar^ sender, Platform::Object^ args);
		void AutoSuggestBox_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void AutoSuggestBox_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void rootFrame_Navigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void Grid_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		
		void Flyout_Opening(Platform::Object^ sender, Platform::Object^ e);
		void Flyout_Open(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Hyperlink_Click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
	internal:
		template <typename func> bool TryNavigateToCachedPage(func&& CompareFunc);
		
		
	};

		
	template<typename func>
	inline bool rootWindowGrid::TryNavigateToCachedPage(func && CompareFunc)
	{
		{
			if (globalvars::NavState.size() > 1)
			{
				std::vector<navVariant>::const_reverse_iterator rend;
				if (globalvars::NavState.size() > 3)
				{
					rend = globalvars::NavState.crbegin() + 3;
				}
				else
				{
					rend = globalvars::NavState.crend();
				}
				auto res = std::find_if(globalvars::NavState.crbegin(), rend, CompareFunc);
				if (res != rend)
				{
					if (App::CurrentManualFrameContent != nullptr)
					{
						rootWindowGrid::getCurrent()->rootFrame->BackStack->Append(App::CurrentManualFrameContent);
					}
					else
					{
						rootWindowGrid::getCurrent()->rootFrame->BackStack->Append(ref new Windows::UI::Xaml::Navigation::PageStackEntry(rootWindowGrid::getCurrent()->rootFrame->CurrentSourcePageType, static_cast<unsigned char>(static_cast<NavPage^>(rootWindowGrid::getCurrent()->rootFrame->Content)->NavigationIndex), nullptr));
					}
					rootWindowGrid::getCurrent()->rootFrame->Content = res->second->pageState;
					App::CurrentManualFrameContent = ref new Windows::UI::Xaml::Navigation::PageStackEntry(res->first, static_cast<unsigned char>(std::distance(globalvars::NavState.cbegin(), res.base())), nullptr);
					return true;
				}
				else
				{
					return false;
				}
			}
			else
				return false;
		}
	}

}
