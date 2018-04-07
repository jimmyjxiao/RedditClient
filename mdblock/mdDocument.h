#pragma once
namespace mdblock
{

	public ref class mdDocument sealed
	{
	public:
		static void getDocument(Platform::String^ x, Windows::UI::Text::ITextDocument^ doc);
		static Platform::String^ getMDfromDoc(Windows::UI::Text::ITextDocument^ doc);
	private:
		class docRenderer
		{
		public:
			docRenderer(Windows::UI::Text::ITextDocument^ z);
			static void getDocument(Platform::String^ md, Windows::UI::Text::ITextDocument^ doc);

		private:

			Windows::UI::Text::ITextRange^ range;
			Windows::UI::Text::ITextDocument^ OutputDoc;
			static int EnterBlock(MD_BLOCKTYPE type, void* detail, void* userdata);
			static int LeaveBlock(MD_BLOCKTYPE type, void* detail, void* userdata);
			static int EnterSpan(MD_SPANTYPE type, void* detail, void* userdata);
			static int LeaveSpan(MD_SPANTYPE type, void* detail, void* userdata);
			static int textWrite(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata);
		};
	};
}
