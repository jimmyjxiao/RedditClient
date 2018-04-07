#pragma once
#include "md4c.h"
#include <vector>
#include <variant>
namespace mdblock
{
	public ref struct LinkData sealed
	{
	private:
		std::variant<const wchar_t*, MD_REDDIT_SLASH_DETAIL*> dataPtr;
	internal:
		LinkData(std::variant<const wchar_t*, MD_REDDIT_SLASH_DETAIL*> v)
		{
			dataPtr = std::move(v);
		}
	public:
		property char16 RedditInfo
		{
			char16 get() {
				if (!std::holds_alternative<MD_REDDIT_SLASH_DETAIL*>(dataPtr))
					return L'f';
				else
				{
					if (std::get<MD_REDDIT_SLASH_DETAIL*>(dataPtr)->type == MD_REDDIT_SUBREDDIT)
						return L's';
					else
						return L'u';
				}
			}
		}
		Platform::String^ getLink()
		{
			return ref new Platform::String(std::get<const wchar_t*>(dataPtr));
		}
	};
	public delegate void linkLoadedeventhandler(Windows::Foundation::Uri^ link, Windows::UI::Xaml::Documents::InlineCollection^ inlineCollection, unsigned int index, Windows::UI::Xaml::Documents::Hyperlink^ sender);
	const MD_SPAN_A_DETAIL refDelete = { { nullptr, 999, nullptr, nullptr },{ nullptr, 999, nullptr, nullptr } };
	bool spanDetailCompare(const MD_SPAN_A_DETAIL* a, const MD_SPAN_A_DETAIL* b, bool shallow);
	bool mdAttrCompare(const MD_ATTRIBUTE & a, const MD_ATTRIBUTE & b, bool shallow);
	struct mdParseCmd
	{
		enum class CommandType : char
		{
			EnterBlock, LeaveBlock, EnterSpan, LeaveSpan, text, Debug
		} Command;
		union detailUnion
		{

			nullptr_t n;
			unsigned char Hlevel;
			MD_ALIGN TAlign;
			MD_REDDIT_SLASH_DETAIL* slashlink;
			unsigned int index;
			detailUnion() { n = nullptr; }
			explicit detailUnion(unsigned char c) :Hlevel(c) {}
			detailUnion(MD_ALIGN T) :TAlign(T) {}
			detailUnion(unsigned int p) : index(p){ }
			~detailUnion() {}
		}detail;
		union
		{
			MD_SPANTYPE spantype : 8;
			MD_BLOCKTYPE blocktype : 8;
			MD_TEXTTYPE texttype : 8;
		};
		
		static mdParseCmd EnterSlashLinkCommand(MD_REDDIT_SLASH_DETAIL* i)
		{
			mdParseCmd returning = { CommandType::EnterSpan };
			returning.detail.slashlink = i;
			returning.spantype = MD_REDDIT_SLASH_LINK;
			return returning;
		}
		static mdParseCmd LeaveSlashLinkCommand(MD_REDDIT_SLASH_DETAIL* i)
		{
			mdParseCmd returning = { CommandType::LeaveSpan };
			returning.detail.slashlink= i;
			returning.spantype = MD_REDDIT_SLASH_LINK;
			return returning;
		}
		static mdParseCmd enterBlockCommand(MD_BLOCKTYPE type)
		{
			mdParseCmd returning;
			returning.Command = { CommandType::EnterBlock };
			returning.blocktype = type;
			return returning;
		}
		static mdParseCmd LeaveBlockCommand(MD_BLOCKTYPE type)
		{
			mdParseCmd returning = { CommandType::LeaveBlock };
			returning.blocktype = type;
			return returning;
		}
		static mdParseCmd EnterSpanCommand( MD_SPANTYPE type)
		{
			mdParseCmd returning = { CommandType::EnterSpan };
			returning.spantype = type;
			return returning;
		}
		static mdParseCmd EnterASpanCommand(unsigned int det)
		{
			return  { CommandType::EnterSpan, det, MD_SPAN_A };
		}
		
		static mdParseCmd LeaveASpanCommand(unsigned int det)
		{
			return { CommandType::LeaveSpan, det, MD_SPAN_A };
		}
		static mdParseCmd LeaveSpanCommand( MD_SPANTYPE type)
		{
			mdParseCmd returning = { CommandType::LeaveSpan };
			returning.spantype = type;
			return returning;
		}
		static mdParseCmd textCommand(unsigned int index, MD_TEXTTYPE type)
		{
			mdParseCmd returning = { CommandType::text, index };
			returning.texttype = type;
			return returning;
		}
	};
	
		
		//std::vector<Platform::String^> Strdeletes;
	public ref struct refMDElements sealed
	{
	internal:
		std::vector<MD_REDDIT_SLASH_DETAIL> slashdet;
		std::vector<Platform::String^> strs;
		std::vector<mdParseCmd> commands;
		Platform::Collections::Vector<Windows::Foundation::Uri^>^ links = ref new Platform::Collections::Vector<Windows::Foundation::Uri^>();
	public:
		property Windows::Foundation::Collections::IVector<Windows::Foundation::Uri^>^ Links
		{
			Windows::Foundation::Collections::IVector<Windows::Foundation::Uri^>^ get()
			{
				return links;
			}
		}
		refMDElements(Platform::String^ md);
		virtual ~refMDElements();

	};
	private class mdRichblock sealed 
    {
    public:
        mdRichblock(Platform::WeakReference r, Platform::String^ md);
		//void resetIntoBlock(Platform::String^ md);
		static std::vector<mdParseCmd> getElements(Platform::String^ md, std::vector<Platform::String^> * strptrs, Platform::Collections::Vector<Windows::Foundation::Uri^>^ links, std::vector<MD_REDDIT_SLASH_DETAIL>* slashes);
		static void setElementsIntoBlockfromUIThread(refMDElements^ value, Windows::UI::Xaml::Controls::RichTextBlock^ richBlock);
	private:

		Platform::WeakReference richblock;
		~mdRichblock();
		//std::vector<Platform::String^> cstrings;

		Platform::String^ md;
		static concurrency::task<std::vector<mdParseCmd>> getElementsAsync(Platform::String^ md, std::vector<Platform::String^> * strptrs, Platform::Collections::Vector<Windows::Foundation::Uri^>^ links , std::vector<MD_REDDIT_SLASH_DETAIL>* slashes, concurrency::cancellation_token t = concurrency::cancellation_token::none());
		static int EnterBlock(MD_BLOCKTYPE type, void* detail, void* userdata);
		static int LeaveBlock(MD_BLOCKTYPE type, void* detail, void* userdata);
		static int EnterSpan(MD_SPANTYPE type, void* detail, void* userdata);
		static int LeaveSpan(MD_SPANTYPE type, void* detail, void* userdata);
		static int textWrite(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);
		static void MDDebuglog(const char* msg, void* userdata);
		
    };
	public ref class mdRichProperties sealed : public Windows::UI::Xaml::DependencyObject
	{
	private: 
		static Windows::UI::Xaml::DependencyProperty^ mdProperty;
		static Windows::UI::Xaml::DependencyProperty^ mdElementsProperty;
		static std::unordered_map<int, concurrency::cancellation_token_source> cancelationTokens;
	public:
		static event linkLoadedeventhandler^ LinkLoaded;
		static property Windows::UI::Xaml::DependencyProperty^ MDElementsProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get()
			{
				return mdElementsProperty;
			}
		}
		static property Windows::UI::Xaml::DependencyProperty^ MDProperty
		{
			Windows::UI::Xaml::DependencyProperty^ get() {
				return mdProperty;
			}
		}
		static Platform::String^ GetMD(Windows::UI::Xaml::UIElement^ element)
		{
			return static_cast<Platform::String^>(element->GetValue(mdProperty));
		}



		static void SetMDElements(Windows::UI::Xaml::UIElement^ element, refMDElements^ value)
		{
			//__debugbreak();
			if (value != nullptr)
			{
				mdRichblock::setElementsIntoBlockfromUIThread(value, static_cast<Windows::UI::Xaml::Controls::RichTextBlock^>(element));
			}
		}
		static refMDElements^ GetMDElements(Windows::UI::Xaml::UIElement^ element)
		{
			return static_cast<refMDElements^>(element->GetValue(mdElementsProperty));
		}


		static void SetMD(Windows::UI::Xaml::UIElement^ element, Platform::String^ value)
		{
			element->SetValue(mdProperty, value);
			if (value != nullptr)
			{
				auto valua = ref new refMDElements(value);
				mdRichblock::setElementsIntoBlockfromUIThread(valua, static_cast<Windows::UI::Xaml::Controls::RichTextBlock^>(element));
				
			}
		}
		static void OnMDChanged(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e)
		{
			__debugbreak();
			
		}
		static void registerProperties();
	internal:
		static void raiseLinkLoaded(Windows::Foundation::Uri^ l, Windows::UI::Xaml::Documents::InlineCollection^ i, unsigned int ii, Windows::UI::Xaml::Documents::Hyperlink^ s)
		{
			LinkLoaded(l, i, ii,s);
		}
	};
}
