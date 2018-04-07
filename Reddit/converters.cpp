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

		auto x = safe_cast<Platform::IBox<bool>^>(value);
		if (x == nullptr)
			return false;
		else
			return !x->Value;

	}
	Platform::Object ^ downvotechecked::ConvertBack(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		throw ref new Platform::NotImplementedException();
		// TODO: insert return statement here
	}
	Platform::Object ^ upvotechecked::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto x = safe_cast<Platform::IBox<bool>^>(value);
		if (x == nullptr)
			return false;
		else
			return x->Value;
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
	Platform::Object ^ ScoreText::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language) {
		if (static_cast<int>(value) < (INT_MIN + 4))
		{
			return ref new Platform::String(L"Score Hidden");
		}
		else return value->ToString() + " points";
	}
	Platform::Object ^ HasPreview::Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language)
	{
		auto && a = static_cast<account::postContentType>(value);
		if (a == account::postContentType::linktype)
			return Windows::UI::Xaml::Visibility::Collapsed;
		else if (a == account::postContentType::selftype)
		{
			if(parameter == nullptr || static_cast<Platform::String^>(parameter) == "")
				return Windows::UI::Xaml::Visibility::Collapsed;
			else
				return Windows::UI::Xaml::Visibility::Visible;
		}
		else
		{
			return Windows::UI::Xaml::Visibility::Visible;
		}
	}
}