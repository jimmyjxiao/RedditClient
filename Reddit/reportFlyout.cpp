#include "pch.h"
#include "reportFlyout.h"
#include <ppl.h>
#include "subpostUWP.h"
#include "globalvars.h"
#include "ApplicationDataHelper.h"
using namespace account;

Reddit::reportFlyout::reportFlyout()
{
	this->Opening += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &Reddit::reportFlyout::OnOpening);
	
}





void Reddit::reportFlyout::load()
{
	if (!loaded)
	{
		auto rootpanel = ref new Windows::UI::Xaml::Controls::StackPanel();
		reasonsPanel = ref new Windows::UI::Xaml::Controls::StackPanel();
		auto subRulesblock = ref new Windows::UI::Xaml::Controls::TextBlock();
		subRulesblock->Style = static_cast<Windows::UI::Xaml::Style^>(App::Current->Resources->Lookup("SubtitleTextBlockStyle"));
		subRulesblock->Text = "Subreddit Rules";
		reasonsPanel->Children->Append(subRulesblock);
		for (unsigned int i = 0; i<_reasons->Size; i++)
		{
			auto & r = _reasons->GetAt(i);
			if (r.scope == reportApplicibility::bothAllowed || r.scope == applic)
			{
				auto radio = ref new Windows::UI::Xaml::Controls::RadioButton();
				radio->Content = r.shortdesc;
				radio->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &reportFlyout::radioChecked);
				reasonsPanel->Children->Append(std::move(radio));
			}
		}
		auto siteBlock = ref new Windows::UI::Xaml::Controls::TextBlock();
		siteBlock->Text = "Reddit Site Rules";
		siteBlock->Style = static_cast<Windows::UI::Xaml::Style^>(App::Current->Resources->Lookup("SubtitleTextBlockStyle"));
		reasonsPanel->Children->Append(siteBlock);
		auto spamRadio = ref new Windows::UI::Xaml::Controls::RadioButton();
		spamRadio->Content = "This is Spam";
		spamRadio->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &reportFlyout::radioChecked);
		reasonsPanel->Children->Append(spamRadio);
		rootpanel->Children->Append(reasonsPanel);
		auto otherRadio = ref new Windows::UI::Xaml::Controls::RadioButton();
		otherRadio->Content = "Other Reason:";
		reasonsPanel->Children->Append(otherRadio);
		textBox = ref new Windows::UI::Xaml::Controls::TextBox();
		textBox->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		textBox->TextChanged += ref new Windows::UI::Xaml::Controls::TextChangedEventHandler(this, &reportFlyout::otherTextChanged);
		otherRadio->Checked += ref new Windows::UI::Xaml::RoutedEventHandler([d = Platform::WeakReference(textBox)](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
			try { d.Resolve<Windows::UI::Xaml::Controls::TextBox>()->Visibility = Windows::UI::Xaml::Visibility::Visible; }
			catch (Platform::NullReferenceException^ n)
			{
				__debugbreak();
			}
		});
		otherRadio->Checked += ref new Windows::UI::Xaml::RoutedEventHandler(this, &reportFlyout::radioChecked);
		otherRadio->Unchecked += ref new Windows::UI::Xaml::RoutedEventHandler([d = Platform::WeakReference(textBox)](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
			try { d.Resolve<Windows::UI::Xaml::Controls::TextBox>()->Visibility = Windows::UI::Xaml::Visibility::Collapsed; }
			catch (Platform::NullReferenceException^ n)
			{
				__debugbreak();
			}
		});
		textRadio = otherRadio;
		rootpanel->Children->Append(textBox);
		submitButton = ref new Windows::UI::Xaml::Controls::Button();
		submitButton->IsEnabled = false;
		submitButton->Content = L"Submit";
		rootpanel->Children->Append(submitButton);
		this->Content = rootpanel;
		loaded = true;

	}
}

void Reddit::reportFlyout::otherTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs ^ e)
{
	if (textRadio->IsChecked->Value == true)
	{
		if (textBox->Text == L"")
		{
			submitButton->IsEnabled = false;
		}
		else
		{
			submitButton->IsEnabled = true;
		}
	}
	
}

void Reddit::reportFlyout::radioChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^)
{
	
	const auto && s = static_cast<Windows::UI::Xaml::Controls::RadioButton^>(sender);
	if (Platform::Object::ReferenceEquals(s, textRadio))
	{
		if (textBox->Text == L"")
		{
			submitButton->IsEnabled = false;
		}
		else
		{
			submitButton->IsEnabled = true;
		}
	}
	else
	{
		submitButton->IsEnabled = true;
	}
}

void Reddit::reportFlyout::OnOpening(Platform::Object ^sender, Platform::Object ^args)
{
	
	if (_reasons == nullptr && !loaded)
	{
		auto it = std::find_if(globalvars::reportReasonCache.rbegin(), globalvars::reportReasonCache.rend(), [this](std::pair<Platform::String^, Platform::Collections::Vector<account::reportReason>^> & m) {
			return (m.first == _sub);
		});
		if (it != globalvars::reportReasonCache.rend())
		{
			_reasons = it->second;
			load();
		}
		else
		{
			try {
				_reasons = ApplicationDataHelper::subredditHelpers::trysubredditRulesCache(reinterpret_cast<const char16_t*>(_sub->Data()));
				globalvars::reportReasonCache.emplace_back(_sub, _reasons);
				load();
			}
			catch (ApplicationDataHelper::cacheMiss < std::vector<account::reportReason>> e)
			{
				_reasons = ref new Platform::Collections::Vector<account::reportReason>();
				globalvars::reportReasonCache.emplace_back(_sub, _reasons);
				e.retrieveTask.then([this](std::vector<account::reportReason> v) {
					for (auto &a : v)
					{
						_reasons->Append(std::move(a));
					}
					Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
						load();
					}));
				});
			}
		}
	}
	else
		load();
}

