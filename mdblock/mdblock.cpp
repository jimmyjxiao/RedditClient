#include "pch.h"
#include "mdblock.h"
#include <ppltasks.h>
using namespace mdblock;
using namespace Platform;



mdblock::mdRichblock::mdRichblock(WeakReference r, Platform::String ^ md)
{
	

	this->md = md;
}


mdRichblock::~mdRichblock()
{
	__debugbreak();
	//cstrings.clear();
}

std::vector<mdParseCmd> mdblock::mdRichblock::getElements(Platform::String ^ md, std::vector<Platform::String^> * strptrs, Platform::Collections::Vector<Windows::Foundation::Uri^>^ links, std::vector<MD_REDDIT_SLASH_DETAIL>* slashes)
{
	const MD_RENDERER renderer =
	{
		&EnterBlock,
		&LeaveBlock,
		&EnterSpan,
		&LeaveSpan,
		&textWrite,

		/* Debug callback. Optional (may be NULL).
		*
		* If provided and something goes wrong, this function gets called.
		* This is intended for debugging and problem diagnosis for developers;
		* it is not intended to provide any errors suitable for displaying to an
		* end user.
		*/
		&MDDebuglog,

		/* Dialect options. Bitmask of MD_FLAG_xxxx values.
		*/
		//unsigned flags;
		MD_DIALECT_REDDIT
	};
	auto returning = std::make_tuple(std::vector<mdParseCmd>(), strptrs, links, slashes);
	md_parse(md->Data(), md->Length(), &renderer, &returning);
	return std::move(std::get<0>(returning));

}
void mdblock::mdRichblock::setElementsIntoBlockfromUIThread(refMDElements^ value, Windows::UI::Xaml::Controls::RichTextBlock^ richBlock)
{
//	__debugbreak();
	richBlock->Blocks->Clear();
	//Windows::UI::Xaml::Documents::InlineCollection^ currentBlock = nullptr;
	Windows::UI::Xaml::Controls::RichTextBlock^ alternateBlock = nullptr;
	Windows::UI::Xaml::Documents::InlineCollection^ currentcol = nullptr;
	//Windows::UI::Xaml::Documents::Span^ currentSpan = nullptr;
	struct tupleParent
	{
		MD_SPANTYPE type;
		Windows::UI::Xaml::Documents::InlineCollection^ parent;
		tupleParent(MD_SPANTYPE s,  Windows::UI::Xaml::Documents::InlineCollection^ e) : type(s), parent(e) {}
	};
	Windows::UI::Xaml::Documents::Paragraph^ _CurrentBlock;
	std::vector<tupleParent> spanParents;
	for(std::vector<mdblock::mdParseCmd>::const_iterator it = value->commands.cbegin(); it!= value->commands.cend(); it++)
	{
		switch (it->Command)
		{
		case mdParseCmd::CommandType::EnterBlock:
		{
			switch (it->blocktype)
			{
			case MD_BLOCKTYPE::MD_BLOCK_QUOTE:
			{
				Windows::UI::Xaml::Controls::Border^ border = ref new Windows::UI::Xaml::Controls::Border();
				border->BorderThickness = Windows::UI::Xaml::ThicknessHelper::FromLengths(3, 0, 0, 0);
				border->Padding = Windows::UI::Xaml::ThicknessHelper::FromLengths(6, 0, 0, 0);
				border->BorderBrush = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::DarkGray);
				auto blk = ref new Windows::UI::Xaml::Documents::Paragraph();
				auto ca = ref new Windows::UI::Xaml::Documents::InlineUIContainer();
				ca->Child = border;
				blk->Inlines->Append(ca);
				richBlock->Blocks->Append(blk);
				if((it+1)->Command == mdParseCmd::CommandType::EnterBlock && (it+2)->Command == mdParseCmd::CommandType::text && (it+3)->Command == mdParseCmd::CommandType::LeaveBlock  && (it+4)->Command == mdParseCmd::CommandType::LeaveBlock && (it+4)->blocktype == MD_BLOCK_QUOTE)
				{
					auto textBlock = ref new Windows::UI::Xaml::Controls::TextBlock();
					textBlock->Text = value->strs[((it+2)->detail.index)];
					if ((it + 1)->blocktype == MD_BLOCK_H)
					{
						switch ((it + 1)->detail.Hlevel)
						{
						case 1:
							textBlock->FontSize = 20.5f;
							textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
							break;
						case 2:
							textBlock->FontSize = 20.5f;
							textBlock->FontWeight = Windows::UI::Text::FontWeights::SemiBold;
							break;
						case 3:
							textBlock->FontSize = 18.3f;
							textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
							break;
						case 4:
							textBlock->FontSize = 20.5f;
							textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
							break;
						case 5:
							textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
							break;
						case 6:
							textBlock->TextDecorations = Windows::UI::Text::TextDecorations::Underline;
							break;
						}
					}
					border->Child = textBlock;
					it += 4;
				}
				else
				{
					byte numberofblocks = 0;
					std::vector<mdParseCmd>::const_iterator endQuote = value->commands.cbegin();
					for (std::vector<mdParseCmd>::const_iterator i = it + 1; i != value->commands.cend(); i++)
					{
						if ((i->Command == mdParseCmd::CommandType::LeaveBlock) && (i->blocktype == MD_BLOCK_QUOTE))
						{
							endQuote = i;
							break;
						}
						else if (i->Command == mdParseCmd::CommandType::EnterBlock)
						{
							numberofblocks++;
							if (numberofblocks > 1)
								break;
						}
					}
					if (numberofblocks == 1)
					{
						auto textBlock = ref new Windows::UI::Xaml::Controls::TextBlock();
						currentcol = textBlock->Inlines;
						for (std::vector<mdParseCmd>::const_iterator i = it + 1; i != endQuote; i++)
						{
							switch (i->Command)
							{
							case mdParseCmd::CommandType::EnterBlock:
								if (i->blocktype == MD_BLOCK_H)
								{
									switch (i->detail.Hlevel)
									{
									case 1:
										textBlock->FontSize = 20.5f;
										textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
										break;
									case 2:
										textBlock->FontSize = 20.5f;
										textBlock->FontWeight = Windows::UI::Text::FontWeights::SemiBold;
										break;
									case 3:
										textBlock->FontSize = 18.3f;
										textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
										break;
									case 4:
										textBlock->FontSize = 20.5f;
										textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
										break;
									case 5:
										textBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
										break;
									case 6:
										textBlock->TextDecorations = Windows::UI::Text::TextDecorations::Underline;
										break;
									}
								}
								break;
							case mdParseCmd::CommandType::LeaveBlock:
								break;
							case mdParseCmd::CommandType::EnterSpan:
								goto entersCmd;
								break;
							case mdParseCmd::CommandType::LeaveSpan:
								goto leavescommand;
								break;
							case mdParseCmd::CommandType::text:
								goto textcmd;
								break;
							}
						}
						it = endQuote;
						border->Child = textBlock;
					}
					else
					{
						alternateBlock = richBlock;
						richBlock = ref new Windows::UI::Xaml::Controls::RichTextBlock();
						border->Child = richBlock;
						continue;
					}
				}
				

			    /*_CurrentBlock = ref new Windows::UI::Xaml::Documents::Paragraph();
				static auto b = ref new Windows::UI::Xaml::Media::SolidColorBrush(Windows::UI::Colors::LightGray);
				_CurrentBlock->Foreground = b;
				it++;
				if (it->Command != mdParseCmd::CommandType::EnterBlock)
				{
					__debugbreak();
				}
				else
				{
					if (it->blocktype == MD_BLOCK_H)
						goto blockhf;
				}
				richBlock->Blocks->Append(_CurrentBlock);
				currentcol = _CurrentBlock->Inlines;*/
			}
				break;
			case MD_BLOCKTYPE::MD_BLOCK_P:
			{
				_CurrentBlock = ref new Windows::UI::Xaml::Documents::Paragraph();
				richBlock->Blocks->Append(_CurrentBlock);
				currentcol = _CurrentBlock->Inlines;
				//richBlock->Blocks->Append(currentcol);
			}
				break;
			case MD_BLOCKTYPE::MD_BLOCK_DOC:
				break;
			case MD_BLOCKTYPE::MD_BLOCK_H:
			{
				_CurrentBlock = ref new Windows::UI::Xaml::Documents::Paragraph();
				currentcol = _CurrentBlock->Inlines;
				richBlock->Blocks->Append(_CurrentBlock);
				switch (it->detail.Hlevel)
				{
				case 1:
					_CurrentBlock->FontSize = 20.5f;
					_CurrentBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
					break;
				case 2:
					_CurrentBlock->FontSize = 20.5f;
					_CurrentBlock->FontWeight = Windows::UI::Text::FontWeights::SemiBold;
					break;
				case 3:
					_CurrentBlock->FontSize = 18.3f;
					_CurrentBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
					break;
				case 4:
					_CurrentBlock->FontSize = 20.5f;
					_CurrentBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
					break;
				case 5:
					_CurrentBlock->FontWeight = Windows::UI::Text::FontWeights::Bold;
					break;
				case 6:
					_CurrentBlock->TextDecorations = Windows::UI::Text::TextDecorations::Underline;
					break;
				}
			}
				break;
			default:
			{
				_CurrentBlock = ref new Windows::UI::Xaml::Documents::Paragraph();
				richBlock->Blocks->Append(_CurrentBlock);
				currentcol = _CurrentBlock->Inlines;
				//richBlock->Blocks->Append(currentcol);
			}
				break;
			}



		}break;
		case mdParseCmd::CommandType::LeaveBlock:
		{
			if (it->blocktype == MD_BLOCK_QUOTE && alternateBlock != nullptr)
			{
				richBlock = alternateBlock;
				alternateBlock = nullptr;
			}
			//currentcol = nullptr;
			break;
		}
		case mdParseCmd::CommandType::EnterSpan:
		{
			entersCmd:
			//__debugbreak();
			Windows::UI::Xaml::Documents::Span^ newspan;
			switch (it->spantype)
			{
			case MD_SPAN_EM:
				newspan = ref new Windows::UI::Xaml::Documents::Italic();
				break;
			case MD_SPAN_STRONG:
				newspan = ref new Windows::UI::Xaml::Documents::Bold();
				break;
			case MD_SPAN_DEL:
				newspan = ref new Windows::UI::Xaml::Documents::Span();
				newspan->TextDecorations = Windows::UI::Text::TextDecorations::Strikethrough;
				break;
			case MD_SPAN_A:
			{
				auto link = ref new Windows::UI::Xaml::Documents::Hyperlink();
				newspan = link;

			}break;
			case MD_REDDIT_SLASH_LINK:
			{
				auto rlink = ref new Windows::UI::Xaml::Documents::Hyperlink();
				newspan = rlink;
				
			}break;
			}
			currentcol->Append(newspan);
			if (it->spantype == MD_SPAN_A)
			{
				mdRichProperties::raiseLinkLoaded(value->links->GetAt(it->detail.index), currentcol, currentcol->Size, static_cast<Windows::UI::Xaml::Documents::Hyperlink^>(newspan));
			}
			spanParents.emplace_back(it->spantype, currentcol);

			currentcol = newspan->Inlines;

			/*if (currentSpan != nullptr)
			{
				spanParents.emplace_back(it->spantype, false, currentSpan);
				currentSpan->Inlines->Append(newspan);
				if (it->spantype == MD_SPAN_A || it->spantype == MD_REDDIT_SLASH_LINK)
					mdRichProperties::raiseLinkLoaded(static_cast<Windows::UI::Xaml::Documents::Hyperlink^>(newspan), currentSpan->Inlines, currentSpan->Inlines->Size);
				
				currentSpan = newspan;
			}
			else
			{
				currentcol->Append(newspan);
				if (it->spantype == MD_SPAN_A || it->spantype == MD_REDDIT_SLASH_LINK)
					mdRichProperties::raiseLinkLoaded(static_cast<Windows::UI::Xaml::Documents::Hyperlink^>(newspan), currentcol, currentcol->Size);
				
				currentSpan = newspan;
				spanParents.emplace_back(it->spantype, true, _CurrentBlock);
			}*/
		}break;
		case mdParseCmd::CommandType::LeaveSpan:
		{
			leavescommand:
			auto rit = spanParents.rbegin();
			while (true)
			{

				if (rit->type == it->spantype)
				{
					/*if (rit->parentISBlock)
					{
						currentSpan = nullptr;
						currentcol = static_cast<Windows::UI::Xaml::Documents::Paragraph^>(rit->parent)->Inlines;
					}
					else
					{
						currentSpan = static_cast<Windows::UI::Xaml::Documents::Span^>(rit->parent);
					}*/
					currentcol = rit->parent;
					if (rit != spanParents.rbegin())
						spanParents.erase((rit + 1).base(), spanParents.end());
					break;
				}
				else {
					rit++;
					if (rit == spanParents.rend())
					{
						__debugbreak();
						break;
					}
				}
			}
			
		}break;
		case mdParseCmd::CommandType::text:
		{
			textcmd:
			auto newrun = ref new Windows::UI::Xaml::Documents::Run();
			if (it->texttype == MD_TEXT_SOFTBR)
			{
				newrun->Text = L" ";
			}
			else
			{
				newrun->Text = value->strs[it->detail.index];
			}
			try
			{
				currentcol->Append(newrun);
			}
			catch (Platform::NullReferenceException^ e)
			{
				currentcol = (ref new Windows::UI::Xaml::Documents::Paragraph())->Inlines;
				currentcol->Append(newrun);
			}
			/*if (currentSpan != nullptr)
			{
				currentSpan->Inlines->Append(newrun);
			}
			else if (currentcol != nullptr)
			{
				currentcol->Append(newrun);
			}
			else
			{
				currentcol = (ref new Windows::UI::Xaml::Documents::Paragraph())->Inlines;
				currentcol->Append(newrun);
			}*/
		}break;
		}
	}
}
int mdRichblock::EnterBlock(MD_BLOCKTYPE type, void * detail, void * userdata)
{
	//__debugbreak();
	auto data = static_cast<std::tuple<std::vector<mdParseCmd>, std::vector<Platform::String^>*, Platform::Collections::Vector<Windows::Foundation::Uri^>^, std::vector<MD_REDDIT_SLASH_DETAIL>*>*>(userdata);
	auto mycmd = mdParseCmd::enterBlockCommand(type);
	if (type == MD_BLOCK_H)
	{
		mycmd.detail.Hlevel = static_cast<unsigned char>(static_cast<MD_BLOCK_H_DETAIL*>(detail)->level);
	}
	else if (type == MD_BLOCK_TD || type == MD_BLOCK_TH)
	{
		mycmd.detail.TAlign = static_cast<MD_BLOCK_TD_DETAIL*>(detail)->align;
	}
	else
	{
		mycmd.detail.n = nullptr;
	}
	std::get<0>(*data).push_back(std::move(mycmd));
	return 0;
}

concurrency::task<std::vector<mdParseCmd>> mdblock::mdRichblock::getElementsAsync(Platform::String^ md, std::vector<Platform::String^> * strptrs, Platform::Collections::Vector<Windows::Foundation::Uri^>^ links, std::vector<MD_REDDIT_SLASH_DETAIL>* slashes, concurrency::cancellation_token t)
{
	return concurrency::create_task([&]() {return getElements(md,strptrs,links,slashes ); }, t);
}

int mdRichblock::LeaveBlock(MD_BLOCKTYPE type, void * detail, void * userdata)
{
	auto data = static_cast<std::tuple<std::vector<mdParseCmd>, std::vector<Platform::String^>*, Platform::Collections::Vector<Windows::Foundation::Uri^>^, std::vector<MD_REDDIT_SLASH_DETAIL>*>*>(userdata);
	auto mycmd = mdParseCmd::LeaveBlockCommand(type);
	if (type == MD_BLOCK_H)
	{
		mycmd.detail.Hlevel = static_cast<unsigned char>(static_cast<MD_BLOCK_H_DETAIL*>(detail)->level);
	}
	else if (type == MD_BLOCK_TD || type == MD_BLOCK_TH)
	{
		mycmd.detail.TAlign = static_cast<MD_BLOCK_TD_DETAIL*>(detail)->align;
	}
	
	std::get<0>(*data).push_back(std::move(mycmd));
	return 0;
}

int mdRichblock::EnterSpan(MD_SPANTYPE type, void * detail, void * userdata)
{
	//__debugbreak();
	auto data = static_cast<std::tuple<std::vector<mdParseCmd>, std::vector<Platform::String^>*, Platform::Collections::Vector<Windows::Foundation::Uri^>^, std::vector<MD_REDDIT_SLASH_DETAIL>*>*>(userdata);
	if (type == MD_SPAN_A)
	{
		std::get<0>(*data).push_back(mdParseCmd::EnterASpanCommand(std::get<2>(*data)->Size));
		std::get<2>(*data)->Append(ref new Windows::Foundation::Uri(ref new Platform::String(static_cast<MD_SPAN_A_DETAIL*>(detail)->href.text, static_cast<MD_SPAN_A_DETAIL*>(detail)->href.size)));
	}
	else if (type == MD_REDDIT_SLASH_LINK)
	{
		auto z = static_cast<MD_REDDIT_SLASH_DETAIL*>(detail);
		MD_REDDIT_SLASH_DETAIL v;
		v.type = z->type;
		v.size = z->size;
		v.name = new wchar_t[100];
		try {
			wcsncpy_s(v.name, z->size + 1, z->name, z->size);
		}
		catch (...)
		{
			__debugbreak();
		}
		std::get<3>(*data)->push_back(std::move(v));
		std::get<0>(*data).push_back(std::move(mdParseCmd::EnterSlashLinkCommand(&std::get<3>(*data)->back())));
	}
	else
	{
		std::get<0>(*data).push_back(mdParseCmd::EnterSpanCommand(type));
	}

	return 0;
}

int mdRichblock::LeaveSpan(MD_SPANTYPE type, void * detail, void * userdata)
{
	auto data = static_cast<std::tuple<std::vector<mdParseCmd>, std::vector<Platform::String^>*, Platform::Collections::Vector<Windows::Foundation::Uri^>^, std::vector<MD_REDDIT_SLASH_DETAIL>*>*>(userdata);


	if (type == MD_SPAN_A)
	{
			std::get<0>(*data).push_back(mdParseCmd::LeaveASpanCommand(NAN));
	}
	else if (type == MD_REDDIT_SLASH_LINK)
	{
		auto det = static_cast<MD_REDDIT_SLASH_DETAIL*>(detail);
		auto it = std::find_if(std::get<3>(*data)->rbegin(), std::get<3>(*data)->rend(), [det](MD_REDDIT_SLASH_DETAIL &a) {
			return ((det->size == a.size) && (det->type == a.type) && (wcsncmp(det->name, a.name, det->size) == 0));
		});
		if (it == std::get<3>(*data)->rend())
		{
			__debugbreak();
		}
		else
		{
			std::get<0>(*data).push_back(std::move(mdParseCmd::LeaveSlashLinkCommand(&(*it))));
		}
	}
	else
	{
		std::get<0>(*data).push_back(mdParseCmd::LeaveSpanCommand(type));
	}

	return 0;
}

int mdRichblock::textWrite(MD_TEXTTYPE type, const MD_CHAR * text, MD_SIZE size, void * userdata)
{
	auto data = static_cast<std::tuple<std::vector<mdParseCmd>, std::vector<Platform::String^>*, Platform::Collections::Vector<Windows::Foundation::Uri^>^, std::vector<MD_REDDIT_SLASH_DETAIL>*>*>(userdata);
	if (type != MD_TEXT_SOFTBR)
	{
		

		//Platform::String^ uwchar = Platform::String^(new wchar_t[size + 1]);
		//wchar_t* nullTerminatedText = new wchar_t[size + 1];
		//std::wstring wstr = std::wstring(text, size);
		//wcsncpy_s(nullTerminatedText, size + 1, text, size);
		//wcsncpy_s(uwchar.get(), size + 1, text, size);
		
		std::get<1>(*data)->push_back(ref new Platform::String(text, size));
		std::get<0>(*data).push_back(std::move(mdParseCmd::textCommand(std::get<1>(*data)->size()-1, type)));
		
	}
	else
	{
		std::get<0>(*data).push_back(std::move(mdParseCmd::textCommand(-1, type)));
	}
	return 0;
}

void mdRichblock::MDDebuglog(const char * msg, void * userdata)
{
	__debugbreak();
}
Windows::UI::Xaml::DependencyProperty^ mdblock::mdRichProperties::mdProperty;
Windows::UI::Xaml::DependencyProperty^ mdblock::mdRichProperties::mdElementsProperty;

std::unordered_map<int, concurrency::cancellation_token_source> mdblock::mdRichProperties::cancelationTokens;
void mdblock::mdRichProperties::registerProperties()
{
	
		 mdProperty = Windows::UI::Xaml::DependencyProperty::RegisterAttached("MD", Platform::String::typeid, mdRichProperties::typeid, ref new Windows::UI::Xaml::PropertyMetadata(ref new Windows::UI::Xaml::PropertyChangedCallback(OnMDChanged)));
			mdElementsProperty = Windows::UI::Xaml::DependencyProperty::RegisterAttached("MDElements", refMDElements::typeid, mdRichProperties::typeid, ref new Windows::UI::Xaml::PropertyMetadata(nullptr));
		
}

bool mdblock::spanDetailCompare(const MD_SPAN_A_DETAIL * a, const MD_SPAN_A_DETAIL * b, bool shallow)
{
	if (shallow)
	{
		return (mdAttrCompare(a->href, b->href, true) && mdAttrCompare(a->title, b->title, true));
	}
	else
	{
		return (mdAttrCompare(a->href, b->href, false) && mdAttrCompare(a->title, b->title, false));
	}
}

bool mdblock::mdAttrCompare(const MD_ATTRIBUTE & a, const MD_ATTRIBUTE & b, bool shallow)
{
	if(shallow)
		return ((a.size == b.size) && (a.text == b.text) && (a.substr_offsets == b.substr_offsets) && (a.substr_types == b.substr_types));
	else
	{
		if (a.size != b.size)
			return false;
		if (*a.substr_offsets != *a.substr_offsets)
			return false;
		if (*a.substr_types != *b.substr_types)
			return false;
		if (wmemcmp(a.text, b.text, a.size) != 0)
			return false;
		else return true;
	}
}

mdblock::refMDElements::refMDElements(Platform::String ^ md)
{
	commands = std::move(mdblock::mdRichblock::getElements(md, &strs, links, &slashdet ));
}
static void destructATTR (MD_ATTRIBUTE & a)
{
	free(const_cast<MD_CHAR*>(a.text));
	free(const_cast<MD_TEXTTYPE*>(a.substr_types));
	free(const_cast<MD_OFFSET*>(a.substr_offsets));
};
mdblock::refMDElements::~refMDElements()
{
	for (auto &&a : slashdet)
	{
		delete[]a.name;
	}
}
