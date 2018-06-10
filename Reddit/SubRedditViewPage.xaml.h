//
// SubRedditViewPage.xaml.h
// Declaration of the SubRedditViewPage class
//

#pragma once
#include "SubRedditViewPage.g.h"
#include "converters.h"
#include "AccountInterface.h"
#include "subpost.h"
#include "subpostUWP.h"
#include "MyResources.xaml.h"
namespace Reddit
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	enum class displayType : unsigned char
	{
		content, self, ad
	};
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SubRedditViewPage sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged,  NavIndexed
	{
	internal:
		static std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> ContentGridItemRecycle;
		static std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> SelfGridItemRecycle;
		static std::vector<Windows::UI::Xaml::Controls::Primitives::SelectorItem^> AdGridItemRecycle;
		static std::vector<Windows::UI::Xaml::Controls::ListViewItem^> ListPostItemRecycle;
		static std::vector<Windows::UI::Xaml::Controls::ListViewItem^> ListAdItemRecycle;
	public:
		property bool SidebarUseCSS
		{
			bool get();
			void set(bool a);
		}
		virtual ~SubRedditViewPage();
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;
		SubRedditViewPage();
		property Windows::Foundation::Collections::IVector<account::subpostUWP^>^ posts
		{
			Windows::Foundation::Collections::IVector<account::subpostUWP^>^ get() {
				return _posts;
			}
		}
		property account::subredditInfo subInfo { account::subredditInfo get() { return _subInfo; }}
		property bool viewMode
		{
			bool get()
			{
				return static_cast<bool>(listingType);
			}
			void set(bool newa)
			{
				//if (pageLoaded && (newa != ((bool)listingType)))
				//{
					listingType = (ViewMode)newa;
					ContentGridItemRecycle.clear();
					SelfGridItemRecycle.clear();
					AdGridItemRecycle.clear();
					PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs("viewMode"));
				//}
			}
		}
		property Platform::String^ Subreddit
		{
			Platform::String^ get() { return _subreddit; }
		}
		property account::postSort Sort
		{
			account::postSort get() { return _sort; }
			void set(account::postSort newsort);
		}
		property account::timerange Range
		{
			account::timerange get() { return _rng; }
			void set(account::timerange newrange);
		}
		// Inherited via NavIndexed
		virtual property int NavigationIndex
		{
			int get()
			{
				return navIndex;
			}
		}
		// Inherited via NavIndexed
		virtual property Windows::UI::Xaml::Interop::TypeName PageType
		{
			Windows::UI::Xaml::Interop::TypeName get()
			{
				return SubRedditViewPage::typeid;
			}
		}
	protected:
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;
	private:
		int navIndex;
		void updateSidebar();
		bool useCss = true;
		std::unique_ptr<account::subredditlisting> lPtr;
		Platform::Collections::Vector<account::subpostUWP^>^ _posts;
		bool pageLoaded = false;
		enum class ViewMode : bool
		{
			grid = false, list = true
		};
		ViewMode listingType = ViewMode::list;
		
		account::subredditInfo _subInfo = account::subredditInfo();
		Platform::String^ _subreddit;
		account::timerange _rng = account::timerange::Default;
		subredditNavstate* nav = nullptr;
		account::postSort _sort = account::postSort::hot;
		void sortSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void rangeSelector_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void listGrid_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void listGrid_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args);
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		

		

		void listView_ChoosingItemContainer(Windows::UI::Xaml::Controls::ListViewBase^ sender, Windows::UI::Xaml::Controls::ChoosingItemContainerEventArgs^ args);
		void postButton_click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
};
	public ref class sortToTimeRangeVis sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			auto s = static_cast<account::postSort>(value);
			bool x= ((s == account::postSort::top) || (s == account::postSort::controversial));
			if (x)
				return Windows::UI::Xaml::Visibility::Visible;
			else
				return Windows::UI::Xaml::Visibility::Collapsed;
		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException;
		}
	};
	public ref class previewIconC sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	private:
		const std::wstring pathstr[9] =
		{
			L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z", //tochange
			L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z",
			L"M14.982 7C14.736 3.256 11.744 0.263 8 0.017V0H7.5H7v0.017C3.256 0.263 0.263 3.256 0.017 7H0v0.5   V8h0.017C0.263 11.744 3.256 14.736 7 14.982V15h0.5H8v-0.018c3.744-0.246 6.736-3.238 6.982-6.982H15V7.5V7H14.982z M4.695 1.635   C4.212 2.277 3.811 3.082 3.519 4H2.021C2.673 2.983 3.599 2.16 4.695 1.635z M1.498 5h1.758C3.122 5.632 3.037 6.303 3.01 7H1.019   C1.072 6.296 1.238 5.623 1.498 5z M1.019 8H3.01c0.027 0.697 0.112 1.368 0.246 2H1.498C1.238 9.377 1.072 8.704 1.019 8z    M2.021 11h1.497c0.292 0.918 0.693 1.723 1.177 2.365C3.599 12.84 2.673 12.018 2.021 11z M7 13.936   C5.972 13.661 5.087 12.557 4.55 11H7V13.936z M7 10H4.269C4.128 9.377 4.039 8.704 4.01 8H7V10z M7 7H4.01   c0.029-0.704 0.118-1.377 0.259-2H7V7z M7 4H4.55C5.087 2.443 5.972 1.339 7 1.065V4z M12.979 4h-1.496   c-0.293-0.918-0.693-1.723-1.178-2.365C11.4 2.16 12.327 2.983 12.979 4z M8 1.065C9.027 1.339 9.913 2.443 10.45 4H8V1.065z M8 5   h2.73c0.142 0.623 0.229 1.296 0.26 2H8V5z M8 8h2.99c-0.029 0.704-0.118 1.377-0.26 2H8V8z M8 13.936V11h2.45   C9.913 12.557 9.027 13.661 8 13.936z M10.305 13.365c0.483-0.643 0.885-1.447 1.178-2.365h1.496   C12.327 12.018 11.4 12.84 10.305 13.365z M13.502 10h-1.758c0.134-0.632 0.219-1.303 0.246-2h1.99   C13.928 8.704 13.762 9.377 13.502 10z M11.99 7c-0.027-0.697-0.112-1.368-0.246-2h1.758c0.26 0.623 0.426 1.296 0.479 2H11.99z",
			L"M545.487 269.909v7.995c0 65.811-53.546 119.338-119.344 119.338H331.24c0 0 22.023-14.931 17.235-46.589h77.668   c40.122 0 72.761-32.633 72.761-72.755v-7.995c0-40.125-32.645-72.761-72.761-72.761h-106.85l0 0h-28.176l16.443 41.632   c0.579 1.469 0.106 3.142-1.152 4.091c-1.266 0.957-3.003 0.951-4.256-0.018l-86.123-66.198c-0.872-0.665-1.374-1.696-1.374-2.784   c0-1.09 0.502-2.125 1.374-2.79l86.123-66.204c0.632-0.496 1.389-0.733 2.146-0.733c0.745 0 1.489 0.231 2.116 0.707   c1.259 0.952 1.731 2.627 1.146 4.093l-16.432 41.636h28.17v-0.006h106.844C491.941 150.562 545.487 204.104 545.487 269.909z    M243.34 302.628c-1.253-0.964-2.991-0.97-4.256-0.012c-1.259 0.951-1.731 2.63-1.149 4.09l16.438 41.63h-28.174l0 0H119.344   c-40.122 0-72.758-32.646-72.758-72.762v-7.997c0-40.117 32.642-72.759 72.758-72.759h77.667   c-4.788-31.649 17.233-46.586 17.233-46.586h-94.9C53.543 148.233 0 201.767 0 267.578v7.997   c0 65.811 53.543 119.345 119.344 119.345h106.843v-0.007h28.173l-16.438 41.63c-0.582 1.472-0.109 3.15 1.149 4.096   c0.63 0.479 1.375 0.71 2.119 0.71c0.75 0 1.513-0.236 2.143-0.733l86.12-66.2c0.875-0.668 1.377-1.696 1.377-2.79   c0-1.1-0.502-2.122-1.377-2.79L243.34 302.628z",
			L"M293.002 78.53C249.646 3.435 153.618-22.296 78.529 21.068C3.434 64.418-22.298 160.442 21.066 235.534     c43.35 75.095 139.375 100.83 214.465 57.47C310.627 249.639 336.371 153.62 293.002 78.53z M219.834 265.801     c-60.067 34.692-136.894 14.106-171.576-45.973C13.568 159.761 34.161 82.935 94.23 48.26     c60.071-34.69 136.894-14.106 171.578 45.971C300.493 154.307 279.906 231.117 219.834 265.801z M213.555 150.652l-82.214-47.949     c-7.492-4.374-13.535-0.877-13.493 7.789l0.421 95.174c0.038 8.664 6.155 12.191 13.669 7.851l81.585-47.103     C221.029 162.082 221.045 155.026 213.555 150.652z",
			L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z", //tochange
			L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z", //tochange
			L"M454.889 121.303v303.255H90.978V121.303H454.889 M485.211 90.977H60.651v363.909h424.56V90.977L485.211 90.977z    M121.304 363.905h303.257l-60.653-181.951l-90.976 121.302l-60.65-60.651L121.304 363.905z M151.628 181.954   c-16.762 0-30.324 13.565-30.324 30.327s13.562 30.324 30.324 30.324c16.762 0 30.327-13.562 30.327-30.324   S168.393 181.954 151.628 181.954z M0 30.326v363.91h30.327V60.65h394.235V30.326H0z",
			L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z" //tochange
		};
		const wchar_t selfpath[172] = L"M0 48v416h512V48H0z M480 432H32V80h448V432z M352 160c0 26.51 21.49 48 48 48s48-21.49 48-48s-21.49-48-48-48   S352 133.49 352 160z M448 400H64l96-256l128 160l64-48L448 400z";
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			auto contentType = (account::postContentType)value;
			return safe_cast<Windows::UI::Xaml::Media::PathGeometry^>(Windows::UI::Xaml::Markup::XamlReader::Load(Platform::StringReference((L"<Geometry xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
				+ pathstr[(int)contentType] + L"</Geometry>").data())));

		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			throw ref new Platform::NotImplementedException;

		}
	};
}
