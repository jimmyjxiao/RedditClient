//
// rootWindowGrid.xaml.h
// Declaration of the rootWindowGrid class
//

#pragma once

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
	public ref class rootWindowGrid sealed
	{
	public:
		rootWindowGrid();
		static rootWindowGrid^ getCurrent();
		void updateAccountsToComboBox();
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
	};
}
