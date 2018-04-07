#pragma once

namespace TreeViewControl {
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class IntegerToIndentationConverter sealed : Windows::UI::Xaml::Data::IValueConverter
    {
    public:

		IntegerToIndentationConverter() {};

        virtual Platform::Object^ Convert(Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);

        virtual Platform::Object^ ConvertBack(Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
    private:
        const int indentMultiplier = 20;
    };
}

