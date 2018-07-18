//
// MyResources.xaml.cpp
// Implementation of the MyResources class
//

#include "pch.h"
#include "MyResources.xaml.h"
#include "ReplyBox.xaml.h"
#include "globalvars.h"
#include "SubRedditViewPage.xaml.h"
using namespace Reddit;
using namespace Microsoft::Advertising;
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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236
Windows::UI::Xaml::DependencyProperty^ AppBarToggleColoredButton::_brushProp;
Windows::UI::Xaml::Media::Brush^ AppBarToggleColoredButton::CheckedBrush::get()
{
	return static_cast<Windows::UI::Xaml::Media::Brush^>(GetValue(_brushProp));
}
void AppBarToggleColoredButton::CheckedBrush::set(Windows::UI::Xaml::Media::Brush^ s)
{
	auto ss = static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(s)->Color;
	SetValue(_brushProp, s);
}
MyResources::MyResources()
{
	nativeadsman = ref new Microsoft::Advertising::WinRT::UI::NativeAdsManager(L"d25517cb-12d4-4699-8bdc-52040c712cab", L"test");
	nativeadsman->AdReady += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &MyResources::NativeAdReady);
	nativeadsman->ErrorOccurred += ref new Windows::Foundation::EventHandler<Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^>([](Platform::Object^ sender, Microsoft::Advertising::WinRT::UI::AdErrorEventArgs^ e) {
		auto code = e->ErrorCode;
		auto message = e->ErrorMessage;
	});
	InitializeComponent();
}

void Reddit::MyResources::NativeAdReady(Platform::Object ^ s, Platform::Object ^ ad)
{
	while (true)
	{
		auto x = WaitingForNativeAds.front().Resolve<Windows::UI::Xaml::Controls::RelativePanel>();
		WaitingForNativeAds.pop();
		if (x != nullptr)
		{
			auto nativead = static_cast<Microsoft::Advertising::WinRT::UI::NativeAd^>(ad);
			auto text = nativead->Title;
			
			static_cast<Windows::UI::Xaml::Controls::TextBlock^>(x->FindName("title"))->Text = nativead->Title;
			static_cast<Windows::UI::Xaml::Controls::TextBlock^>(x->FindName("desc"))->Text = nativead->Description;
			auto whatiss = Platform::Object::ReferenceEquals(s, this->nativeadsman);
			/*auto newicon = nativead->AdIcon;
			newicon->SetValue(Windows::UI::Xaml::Controls::RelativePanel::AlignBottomWithPanelProperty, true);
			newicon->SetValue(Windows::UI::Xaml::Controls::RelativePanel::AlignRightWithPanelProperty, true);
			x->Children->Append(newicon);*/
			break;
		}
	}
	
}

Reddit::EXplaceHolder::EXplaceHolder()
{
	this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Reddit::EXplaceHolder::OnLoaded);
}

const uintptr_t MASK = ~0x03ULL;
Reddit::EXplaceHolder::EXplaceHolder(account::serviceHelpers::previewHelperbase* ptr)
{
	contentPtr = (void *)(((uintptr_t)ptr & MASK) | 2);
	this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Reddit::EXplaceHolder::OnLoaded);
}

void Reddit::EXplaceHolder::OnExpanding(Platform::Object ^, Platform::Object ^)
{
	//__debugbreak();
	//contentLoaded = true;
}
Windows::UI::Xaml::DataTemplate ^ Reddit::MixedContentSelector::SelectTemplateCore(Platform::Object ^ item)
{
	switch (static_cast<account::IRedditTypeIdentifier^>(item)->rType)
	{
	case account::RedditType::subpost:
		return SubPostDataTemplate;
		break;
	case account::RedditType::comment:
		return CommentDataTemplate;
		break;
	default:
		break;
	}
}

void Reddit::EXplaceHolder::post::set(account::subpostUWP^ sa)
{
	// store the tag
	contentPtr= (void *)(((uintptr_t)&sa->contentHelper & MASK) | 1);
}
void Reddit::EXplaceHolder::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	//auto ParentExpander = static_cast<ExpanderControl::Expander^>(Parent);
	//ParentExpander->Expanding += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &Reddit::EXplaceHolder::OnExpanding);
	if (Content == nullptr)
	{
		int tag = (uintptr_t)contentPtr & 0x03;
		if (tag == 1)
		{
			auto contentH = reinterpret_cast<std::variant<std::unique_ptr<account::serviceHelpers::previewHelperbase>, concurrency::task<account::serviceHelpers::previewHelperbase*>>*>((uintptr_t)contentPtr & MASK);
			if (std::holds_alternative<std::unique_ptr<account::serviceHelpers::previewHelperbase>>(*contentH))
			{
			nonccc:
				auto resizer = ref new contentResizer();
				resizer->Content = std::get<std::unique_ptr<account::serviceHelpers::previewHelperbase>>(*contentH)->viewerControl();
				Content = std::move(resizer);
			}
			else
			{
				try
				{
					std::get<concurrency::task<account::serviceHelpers::previewHelperbase*>>(*contentH).then([this](account::serviceHelpers::previewHelperbase* r) {
						return Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
							Windows::UI::Core::CoreDispatcherPriority::High,
							ref new Windows::UI::Core::DispatchedHandler([this, r]()
						{
							auto resizer = ref new contentResizer();
							resizer->Content = r->viewerControl();
							this->Content = resizer;
						}));
					});
				}
				catch (std::bad_variant_access&)
				{
					goto nonccc;
				}
			}
		}
		else
		{
			auto resizer = ref new contentResizer();
			resizer->Content = reinterpret_cast<account::serviceHelpers::previewHelperbase*>((uintptr_t)contentPtr & MASK)->viewerControl();
			this->Content = resizer;
		}
			/*concurrency::create_task(*contentTask).then([this](std::pair<account::postContentType, account::serviceHelpers::previewHelperbase*> r) {
				return Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
					Windows::UI::Core::CoreDispatcherPriority::High,
					ref new Windows::UI::Core::DispatchedHandler([this, r]()
				{
					auto resizer = ref new contentResizer();
					resizer->Content = r.second->viewerControl();
					this->Content = resizer;
				}));
			}).then([this]()
			{
				contentLoaded = true;
			});*/
	}
}


void Reddit::MyResources::contentExpander_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	auto v = static_cast<Windows::UI::Xaml::FrameworkElement^>(static_cast<Windows::UI::Xaml::FrameworkElement^>(sender->Parent)->Parent);
}


void Reddit::MyResources::contentExpander_Loading(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{


}


void Reddit::MyResources::TextBlock_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto send = static_cast<Windows::UI::Xaml::Controls::TextBlock^>(sender);
	auto context = static_cast<account::subpostUWP^>(send->DataContext);
	if (context != nullptr && context->Golds != 0 && send->Inlines->Size != 6)
	{
		auto run = ref new Windows::UI::Xaml::Documents::Run();
		run->FontSize = 22;
		run->Text = L"⭐";
		send->Inlines->Append(run);
		if (context->Golds > 1)
		{
			auto num = ref new Windows::UI::Xaml::Documents::Run();
			run->Text = L" x" + context->Golds;
			run->Foreground = ref new  Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Gray);
		}
	}
}



void setExpanderContent(Windows::UI::Xaml::Controls::ContentPresenter^ p)
{
	
}

void Reddit::MyResources::ContentPresenter_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	auto && send = static_cast<account::subpostUWP^>(sender->DataContext);
	send->PropertyChanged += ref new Windows::UI::Xaml::Data::PropertyChangedEventHandler([m = Platform::WeakReference(send)](Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e) {
		if (e->PropertyName == "contentType")
		{
			auto ptr = m.Resolve<Windows::UI::Xaml::Controls::ContentPresenter>();
			if(ptr != nullptr)
				setExpanderContent(std::move(ptr));
		}
	});
	setExpanderContent(static_cast<Windows::UI::Xaml::Controls::ContentPresenter^>(sender));
}





void Reddit::MyResources::AppBarButton_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	const auto & send = static_cast<account::subpostUWP^>(static_cast<Windows::UI::Xaml::Controls::AppBarButton^>( sender)->DataContext);
	if (send != nullptr && send->contentType == account::postContentType::selftype)
	{
		static_cast<Windows::UI::Xaml::Controls::AppBarButton^>(sender)->Click += ref new Windows::UI::Xaml::RoutedEventHandler([](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
		Windows::System::Launcher::LaunchUriAsync(static_cast<account::subpostUWP^>(static_cast<Windows::UI::Xaml::Controls::AppBarButton^>(sender)->DataContext)->link);

		});

	}
	else
	{
		auto flyout = ref new Windows::UI::Xaml::Controls::MenuFlyout();
		auto item1 = ref new Windows::UI::Xaml::Controls::MenuFlyoutItem();
		item1->Text = "Link";
		item1->Tag = send->link;
		item1->Click += ref new Windows::UI::Xaml::RoutedEventHandler([](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
			Windows::System::Launcher::LaunchUriAsync(static_cast<Windows::Foundation::Uri^>( static_cast<Windows::UI::Xaml::Controls::MenuFlyoutItem^>(sender)->Tag));
		});
		auto item2 = ref new Windows::UI::Xaml::Controls::MenuFlyoutItem();
		item2->Text = "Reddit Post";
		item2->Tag = ref new Windows::Foundation::Uri("https://www.reddit.com", send->helper.Getpermalink());
		item2->Click += ref new Windows::UI::Xaml::RoutedEventHandler([](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
			Windows::System::Launcher::LaunchUriAsync(static_cast<Windows::Foundation::Uri^>(static_cast<Windows::UI::Xaml::Controls::MenuFlyoutItem^>(sender)->Tag));
		});

		flyout->Items->Append(std::move(item1));
		flyout->Items->Append(std::move(item2));
		static_cast<Windows::UI::Xaml::Controls::AppBarButton^>(sender)->Flyout = flyout;
	}
}

#define pAr(x) VisualTreeHelper::GetParent(x)
void Reddit::MyResources::replyButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	const auto &sen = static_cast<Windows::UI::Xaml::Controls::AppBarToggleButton^>(sender);
	//note: using the Parent property yields nullptr for some reason so we find the Grid with VisualTreeHelper instead.
	//if only MSVC had a loop unroll pragma...
	if (sen->IsChecked->Value)
	{
		const auto &g = static_cast<Windows::UI::Xaml::Controls::Grid^>(pAr(pAr(pAr(pAr(pAr(pAr(pAr(pAr(static_cast<Windows::UI::Xaml::DependencyObject^>(sender))))))))));
		if (sen->Tag == nullptr)
		{
			g->RowDefinitions->Append(ref new Windows::UI::Xaml::Controls::RowDefinition());
			auto replyBox = ref new ReplyBox(&static_cast<account::subpostUWP^>(g->DataContext)->helper);
			replyBox->CommentSubmitted += ref new Windows::Foundation::EventHandler<account::CommentUWPitem^>([toggle = Platform::WeakReference(sender)](Platform::Object^ sende, account::CommentUWPitem^) {
				auto r = toggle.Resolve<Windows::UI::Xaml::Controls::AppBarToggleButton>();
				r->IsChecked = false;
				static_cast<ReplyBox^>(sende)->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			});
			replyBox->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Stretch;
			replyBox->SetValue(Windows::UI::Xaml::Controls::Grid::ColumnSpanProperty, 2);
			replyBox->SetValue(Windows::UI::Xaml::Controls::Grid::RowProperty, 3);
			sen->Tag = replyBox;
			g->Children->Append(std::move(replyBox));
		}
		else
		{
			static_cast<ReplyBox^>(sen->Tag)->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
	}
	else
	{
		static_cast<ReplyBox^>(sen->Tag)->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
}

void Reddit::AppBarToggleColoredButton::registerDependencyProperty()
{
	_brushProp = Windows::UI::Xaml::DependencyProperty::Register("CheckedBrush", Windows::UI::Xaml::Media::Brush::typeid, AppBarToggleColoredButton::typeid, nullptr);
}


void Reddit::MyResources::username_click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{

}


void Reddit::MyResources::subreddit_click(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{
	rootWindowGrid::getCurrent()->NavigateToNewPage(SubRedditViewPage::typeid, globalvars::addNav(SubRedditViewPage::typeid, new subredditNavstate(static_cast<Windows::UI::Xaml::Documents::Run^>(sender->Inlines->GetAt(1))->Text)));
}


void Reddit::MyResources::adcontainer_loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	if (extraAds.size() > 0)
	{
		sender->DataContext = extraAds.front();
		extraAds.pop();
	}
	else
	{
		WaitingForNativeAds.emplace(Platform::WeakReference(sender));
		nativeadsman->RequestAd();
	}
}


void Reddit::MyResources::AdControl_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
}


void Reddit::MyResources::GridTemplate_loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	const auto & context = static_cast<account::subpostUWP^>(sender->DataContext);
	if (context != nullptr)
	{
		if (context->self)
		{
			if (context->selftext != "")
			{

			}
		}
		else
		{
			auto contentGrid = ref new Windows::UI::Xaml::Controls::Grid();
			auto image = ref new Windows::UI::Xaml::Controls::Image();
			image->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(context->previewURI);
			contentGrid->Children->Append(std::move(image));
			auto path = ref new Windows::UI::Xaml::Controls::PathIcon();
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
			static std::array<Windows::UI::Xaml::Media::PathGeometry^, 9> paths = { nullptr,nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
			if (paths[((int)context->contentType)] == nullptr)
			{
				paths[((int)context->contentType)] = safe_cast<Windows::UI::Xaml::Media::PathGeometry^>(Windows::UI::Xaml::Markup::XamlReader::Load(Platform::StringReference((L"<Geometry xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
					+ pathstr[(int)context->contentType] + L"</Geometry>").data())));
			}
			path->Data = safe_cast<Windows::UI::Xaml::Media::PathGeometry^>(Windows::UI::Xaml::Markup::XamlReader::Load(Platform::StringReference((L"<Geometry xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>"
				+ pathstr[(int)context->contentType] + L"</Geometry>").data())));
			path->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
			path->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
			contentGrid->Children->Append(std::move(path));
			auto button = ref new Windows::UI::Xaml::Controls::Button();
			button->Width = 460;
			button->Padding = 0;
			button->Command = context->previewCommand;
			Windows::UI::Xaml::Controls::Grid::SetRow(button, 1);
			button->Content = contentGrid;
			static_cast<Windows::UI::Xaml::Controls::Grid^>(sender)->Children->Append(button);
		}
	}
}
