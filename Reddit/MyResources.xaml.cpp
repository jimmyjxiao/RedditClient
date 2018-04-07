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
	InitializeComponent();
}

Reddit::EXplaceHolder::EXplaceHolder()
{
	this->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Reddit::EXplaceHolder::OnLoaded);
}


void Reddit::EXplaceHolder::OnExpanding(Platform::Object ^, Platform::Object ^)
{
	//__debugbreak();
	//contentLoaded = true;
}

void Reddit::EXplaceHolder::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	//auto ParentExpander = static_cast<ExpanderControl::Expander^>(Parent);
	//ParentExpander->Expanding += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &Reddit::EXplaceHolder::OnExpanding);
	if (Content == nullptr)
	{
		auto post = s.Resolve<account::subpostUWP>();
		if (post->contentHelper != nullptr)
		{
			if (post->contentType != account::postContentType::selftype)
			{
				auto resizer = ref new contentResizer();
				resizer->Content = post->contentHelper->viewerControl();
				this->Content = resizer;
			}
			else
			{
				this->Content = post->contentHelper->viewerControl();
			}
		}
		else
		{
			concurrency::create_task(post->contentTask).then([this](account::serviceHelpers::previewHelperbase* r) {
				return Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
					Windows::UI::Core::CoreDispatcherPriority::High,
					ref new Windows::UI::Core::DispatchedHandler([this, r]()
				{
					auto resizer = ref new contentResizer();
					resizer->Content = r->viewerControl();
					this->Content = resizer;
				}));
			}).then([this]()
			{
				contentLoaded = true;
			});
		}
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
	auto && send = static_cast<account::subpostUWP^>(static_cast<Windows::UI::Xaml::Controls::AppBarButton^>( sender)->DataContext);
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
		item2->Tag = ref new Windows::Foundation::Uri("https://www.reddit.com", send->helper.permalink);
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
	rootWindowGrid::getCurrent()->rootFrame->Navigate(SubRedditViewPage::typeid, globalvars::addNav(new subredditNavstate(static_cast<Windows::UI::Xaml::Documents::Run^>(sender->Inlines->GetAt(1))->Text)));
}
