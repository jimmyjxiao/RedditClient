#pragma once
namespace converters
{
	struct MyObjectHash
	{
		std::size_t operator()(Platform::Object^ _Left) const
		{
			return _Left->GetHashCode();
		};
	};
	public ref class upvotechecked sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			auto x = safe_cast<Platform::IBox<bool>^>(value);
			if (x == nullptr)
				return false;
			else
				return x->Value;

		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException;
		}
	};
	public ref class downvotechecked sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			auto x = safe_cast<Platform::IBox<bool>^>(value);
			if (x == nullptr)
				return false;
			else
				return !x->Value;

		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			throw ref new Platform::NotImplementedException;

		}
	};
	public ref class add15 sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{

			double i = safe_cast<double>(value);
			return i + 15;
		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException;
		}
	};
	public ref class NullableValueConverter sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			auto i = static_cast<Platform::IBox<bool>^>(value);
			return i->Value;
		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			return ref new Platform::Box<bool>(static_cast<bool>(value));
		}
	};
	public ref class BrushToColor sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
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
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException();
		}
	};
	public ref class InverseBool sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			return !static_cast<bool>(value);
		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			return !static_cast<bool>(value);
		}
	};
	public ref class unformatHtml sealed : Windows::UI::Xaml::Data::IValueConverter
	{
	public:
		virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			return Windows::Data::Html::HtmlUtilities::ConvertToText(static_cast<Platform::String^>(value));
		}
		virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType,
			Platform::Object^ parameter, Platform::String^ language)
		{
			throw ref new Platform::NotImplementedException();
		}
	};
}