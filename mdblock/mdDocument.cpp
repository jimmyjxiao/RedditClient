#include "pch.h"
#include "mdDocument.h"
using namespace Windows::UI::Text;
using namespace mdblock;


void mdblock::mdDocument::docRenderer::getDocument(Platform::String ^ md, Windows::UI::Text::ITextDocument^ doc)
{
	std::unique_ptr<docRenderer> rndr = std::make_unique<docRenderer>(doc);

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

		/* Dialect options. Bitmask of MD_FLAG_xxxx values.
		*/
		//unsigned flags;
		nullptr,
		MD_DIALECT_REDDIT
	};
	md_parse(md->Data(), md->Length(), &renderer, rndr.get());
	// TODO: insert return statement here
}

mdblock::mdDocument::docRenderer::docRenderer(Windows::UI::Text::ITextDocument ^ z)
{
	OutputDoc = z;
	range = OutputDoc->GetRange(0, 0);
	range->CharacterFormat = OutputDoc->GetDefaultCharacterFormat();
}


int mdblock::mdDocument::docRenderer::EnterBlock(MD_BLOCKTYPE type, void * detail, void * userdata)
{

	if (type == MD_BLOCKTYPE::MD_BLOCK_H)
	{
		auto rnder = static_cast<mdDocument::docRenderer*>(userdata);
		rnder->range->EndOf(TextRangeUnit::Story, false);
		auto det = static_cast<MD_BLOCK_H_DETAIL*>(detail);
		if(det->level == 5)
			rnder->range->CharacterFormat->Bold = FormatEffect::On;
		else if(det->level == 6)
			rnder->range->CharacterFormat->Underline = UnderlineType::Single;
		else
		{
			float Fontsize = 0;
			int weight = 0;
			switch (det->level)
			{
			case 1:
				Fontsize = 20.5f, weight = 600;
				break;
			case 2:
				Fontsize = 20.5f, weight = 500;
				break;
			case 3:
				Fontsize = 18.3f, weight = 600;
				break;
			case 4:
				Fontsize = 20.5f, weight = 600;
				break;
			}
			rnder->range->CharacterFormat->Size = Fontsize;
			rnder->range->CharacterFormat->Weight = weight;
		}
	}
	return 0;
}

int mdblock::mdDocument::docRenderer::LeaveBlock(MD_BLOCKTYPE type, void * detail, void * userdata)
{

	if (type == MD_BLOCKTYPE::MD_BLOCK_P || type == MD_BLOCKTYPE::MD_BLOCK_H)
	{
		auto rnder = static_cast<mdDocument::docRenderer*>(userdata);
		rnder->range->EndOf(TextRangeUnit::Story, false);
		rnder->range->CharacterFormat = rnder->OutputDoc->GetDefaultCharacterFormat();
		rnder->range->SetText(TextSetOptions::ApplyRtfDocumentDefaults, L"\n");

	}
	return 0;
}

int mdblock::mdDocument::docRenderer::EnterSpan(MD_SPANTYPE type, void * detail, void * userdata)
{
	auto rnder = static_cast<mdDocument::docRenderer*>(userdata);
	rnder->range->EndOf(TextRangeUnit::Story, false);
	switch (type)
	{
	case MD_SPANTYPE::MD_SPAN_EM:
		rnder->range->CharacterFormat->Italic = FormatEffect::On;
		break;
	case MD_SPANTYPE::MD_SPAN_STRONG:
		rnder->range->CharacterFormat->Bold = FormatEffect::On;
		break;
	}

	return 0;
}

int mdblock::mdDocument::docRenderer::LeaveSpan(MD_SPANTYPE type, void * detail, void * userdata)
{
	auto rnder = static_cast<mdDocument::docRenderer*>(userdata);
	rnder->range->EndOf(TextRangeUnit::Story, false);
	switch (type)
	{
	case MD_SPANTYPE::MD_SPAN_EM:
		rnder->range->CharacterFormat->Italic = FormatEffect::Off;
		break;
	case MD_SPANTYPE::MD_SPAN_STRONG:
		rnder->range->CharacterFormat->Bold = FormatEffect::Off;
		break;
	}
	return 0;
}

int mdblock::mdDocument::docRenderer::textWrite(MD_TEXTTYPE type, const MD_CHAR * text, MD_SIZE size, void * userdata)
{
	auto rnder = static_cast<mdDocument::docRenderer*>(userdata);
	rnder->range->SetText(TextSetOptions::None,ref new Platform::String(text, size));
	rnder->range->EndOf(TextRangeUnit::Story, false);
	return 0;
}

void mdblock::mdDocument::getDocument(Platform::String^ x, Windows::UI::Text::ITextDocument^ doc)
{
	 docRenderer::getDocument(x, doc);
}

Platform::String ^ mdblock::mdDocument::getMDfromDoc(Windows::UI::Text::ITextDocument ^ doc)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}
