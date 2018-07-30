//
// NewPostPopup.xaml.cpp
// Implementation of the NewPostPopup class
//

#include "pch.h"
#include "NewPostPopup.xaml.h"

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

// The Content Dialog item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

Reddit::NewPostPopup::NewPostPopup()
{
	InitializeComponent();
}

Reddit::NewPostPopup::NewPostPopup(Platform::String ^ s)
{
	deducedName = s;
	Subreddit = s;
	sub_is_fine = true;
}

bool Reddit::NewPostPopup::SubmitEnabled()
{
	if (sub_is_fine && PostTitle->Length() < 300 && PostTitle != L"")
		IsPrimaryButtonEnabled = true;
	else IsPrimaryButtonEnabled = false;
	return IsPrimaryButtonEnabled;
}

void Reddit::NewPostPopup::updateDeducedName()
{
	std::wstring_view nm(Subreddit->Data(), Subreddit->Length() +1);
	auto res = nm.rfind(L'/');
	if (res != std::wstring_view::npos)
	{
		nm = nm.substr(res+1);
		deducedName = Platform::StringReference(Subreddit->Data() + res + 1);
	}
	else
	{
		deducedName = Subreddit;
	}
	if ((_wcsicmp(nm.data(), L"all") == 0) || (_wcsicmp(nm.data(), L"popular") == 0))
	{
		sub_is_fine = false;
		throw L"/r/all and /r/popular are not actual subreddits so you can't post to them";
	}
	if (deducedName != "")
	{
		if (Subreddit->Length() < 4)
		{
			if (Subreddit->Data()[0] == L'/')
				sub_is_fine = false;
			else if (Subreddit->Length() == 2 && Subreddit->Data()[0] == L'r' && Subreddit->Data()[1] == L'/')
				sub_is_fine = false;
			else
				sub_is_fine = true;
		}
		else
			sub_is_fine = true;
	}
	else
		sub_is_fine = false;
}

void Reddit::NewPostPopup::ContentDialog_PrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args)
{

}

void Reddit::NewPostPopup::ContentDialog_SecondaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args)
{
}


void Reddit::NewPostPopup::title_changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	charCounter->Text = titleBox->Text->Length() + L"/300";
	SubmitEnabled();
}


void Reddit::NewPostPopup::ContentDialog_Opened(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogOpenedEventArgs^ args)
{
	Bindings->Update();
}


void Reddit::NewPostPopup::subbox_PreviewKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Space)
	{
		e->Handled = true;
	}
}


void Reddit::NewPostPopup::WYSIWYG_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	static_cast<mdblock::MDFullEnhancedEditor^>(sender)->Box->markdown = md;
}


void Reddit::NewPostPopup::WYSIWYG_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	md = static_cast<mdblock::MDFullEnhancedEditor^>(sender)->Box->markdown;
}


void Reddit::NewPostPopup::mdEditor_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Bindings->Update();
}


void Reddit::NewPostPopup::mdEditor_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	md = static_cast<Windows::UI::Xaml::Controls::TextBox^>(sender)->Text;
}


void Reddit::NewPostPopup::subbox_BeforeTextChanging(Windows::UI::Xaml::Controls::TextBox^ sender, Windows::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs^ args)
{
	try {
		if (args->NewText->Length() > 20)
			throw L"Subreddit names are not longer than 20 characters";
		const wchar_t* beg = args->NewText->Data();
		for (unsigned int i = 0; i < args->NewText->Length(); i++)
		{

			if (wchar_t x = beg[i]; !isalnum(x))
			{
				if (x != L'_' && x != L'-')
				{
					if (x == L'/')
					{
						if (i != 0)
						{
							if (i == 2)
							{
								if (beg[1] != L'r')
									throw L"Slashes may only be used in the /r/subreddit or r/subreddit format";
							}
							else if (i == 1)
							{
								if (beg[0] != L'r')
									throw L"Slashes may only be used in the /r/subreddit or r/subreddit format";
							}
							else
								throw L"Slashes may only be used in the /r/subreddit or r/subreddit format";
						}
						else if (args->NewText->Length() > 1)
						{
							if (beg[1] != L'r')
							{
								throw L"Slashes may only be used in the /r/subreddit or r/subreddit format";
							}
							else if ((args->NewText->Length() > 2) && (beg[2] != L'/'))
							{
								throw L"Slashes may only be used in the /r/subreddit or r/subreddit format";
							}
						}
					}
					else
					{
						throw L"Invalid character in subreddit: only letters, numbers,'-', and '_' allowed.";
					}
				}
			}
		}
		subErrorBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	catch (const wchar_t* ProblemStr)
	{
		subErrorBox->Text = Platform::StringReference(ProblemStr);
		subErrorBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
		args->Cancel = true;
	}
}


void Reddit::NewPostPopup::subbox_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	try {
		Subreddit = subbox->Text;
		updateDeducedName();
	}
	catch (const wchar_t* x)
	{
		sub_is_fine = false;
		subErrorBox->Text = Platform::StringReference(x);
		subErrorBox->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	SubmitEnabled();
}
