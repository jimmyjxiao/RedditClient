#include "pch.h"
#include "converters.h"
namespace converters
{
	Windows::UI::Xaml::FrameworkElement ^ GetChildByName(Windows::UI::Xaml::DependencyObject ^ parent, Platform::String ^ name)
	{
		int i = 0;
		for (; i < Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(parent); ++i)
		{
			auto child = Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(parent, i);

			if (child != nullptr)
			{
				if (static_cast<Windows::UI::Xaml::FrameworkElement^>(child)->Name == name)
				{
					return static_cast<Windows::UI::Xaml::FrameworkElement^>(child);
				}
				/*else
				{
				FrameworkElement^ control = GetChildByName(child, name);
				if (control != nullptr)
				return control;
				}*/
			}

		}
		return nullptr;
	}
	Platform::Object ^ unformatHtml::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		return Windows::Data::Html::HtmlUtilities::ConvertToText(static_cast<Platform::String^>(value));
	}
	Platform::Object ^ unformatHtml::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Platform::Object ^ InverseBool::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		return !static_cast<bool>(value);
	}
	Platform::Object ^ InverseBool::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		return !static_cast<bool>(value);
	}
	Platform::Object ^ BrushToColor::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{

		auto i = static_cast<Windows::UI::Color>(value);
		if (i.Equals(Windows::UI::Colors::Transparent))
		{
			__debugbreak();
			return static_cast<Windows::UI::Xaml::Media::SolidColorBrush^>(Windows::UI::Xaml::Application::Current->Resources->Lookup("SystemControlBackgroundChromeMediumBrush"));
		}
		else
			return ref new Windows::UI::Xaml::Media::SolidColorBrush(i);
	}
	Platform::Object ^ BrushToColor::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Platform::Object ^ NullableValueConverter::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		return static_cast<Platform::IBox<bool>^>(value)->Value;
	}
	Platform::Object ^ NullableValueConverter::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		return ref new Platform::Box<bool>(static_cast<bool>(value));
	}
	Platform::Object ^ add15::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		double i = safe_cast<double>(value);
		return i + 15;
	}
	Platform::Object ^ add15::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Platform::Object ^ downvotechecked::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{

		auto x = safe_cast<int>(value);
		if (x == 0 || x == 1)
			return false;
		else
			return true;


	}
	Platform::Object ^ downvotechecked::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Platform::Object ^ upvotechecked::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto x = safe_cast<int>(value);
		if (x == 0 || x == -1)
			return false;
		else
			return true;

	}
	Platform::Object ^ upvotechecked::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	std::size_t MyObjectHash::operator()(Platform::Object ^ _Left) const
	{
		return _Left->GetHashCode();
	};
	Platform::Object ^ ThemeConverter::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto &color = static_cast<Windows::UI::Color>(value);
		if ((5 * color.G + 2 * color.R + color.B) <= 8 * 128)
			return Windows::UI::Xaml::ElementTheme::Dark;
		else
			return Windows::UI::Xaml::ElementTheme::Light;
	}

	Platform::Object ^ HasPreview::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto && a = static_cast<account::postContentType>(value);
		if (a == account::postContentType::linktype)
			return Windows::UI::Xaml::Visibility::Collapsed;
		else if (a == account::postContentType::selftype)
		{
			if (parameter == nullptr || static_cast<Platform::String^>(parameter) == "")
				return Windows::UI::Xaml::Visibility::Collapsed;
			else
				return Windows::UI::Xaml::Visibility::Visible;
		}
		else
		{
			return Windows::UI::Xaml::Visibility::Visible;
		}
	}

	Platform::Object ^ DistinguishedAuthorBadge::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto x = static_cast<account::DistinguishedAccountTypes>(value);
		if (x != account::DistinguishedAccountTypes::none)
		{
			auto border = ref new Windows::UI::Xaml::Controls::Border();
			static auto bluecolor = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Blue);
			static auto whitebrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::White);
			if (x != account::DistinguishedAccountTypes::me)
			{
				auto textblock = ref new Windows::UI::Xaml::Controls::TextBlock();
				Windows::UI::Xaml::Media::SolidColorBrush^* backgroundcolor;
				switch (x)
				{
				case account::DistinguishedAccountTypes::mod:
					textblock->Text = L"[M]";
					static auto zzz = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Green);
					backgroundcolor = &zzz;
					break;
				case account::DistinguishedAccountTypes::admin:
					static auto zz = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::Red);
					backgroundcolor = &zz;
					textblock->Text = L"[A]";
					break;
				case account::DistinguishedAccountTypes::op:
					textblock->Text = L"[OP]";
					backgroundcolor = &bluecolor;
					break;
				default:
					__assume(0);
				}
				textblock->Foreground = whitebrush;
				border->Child = std::move(textblock);
				border->Background = *backgroundcolor;
			}
			else
			{
				border->Background = bluecolor;
				auto symbol = ref new Windows::UI::Xaml::Controls::SymbolIcon(Windows::UI::Xaml::Controls::Symbol::WebCam);
				symbol->Foreground = whitebrush;
				border->Child = std::move(symbol);
			}
			return border;
		}
		else
		{
			return nullptr;
		}
	}
}