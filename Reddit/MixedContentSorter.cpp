#include "pch.h"
#include "MixedContentSorter.h"
#include "commentUWPitem.h"
#include "subpostUWP.h"
#include "ppl.h"
namespace account
{
	Platform::Collections::Vector<IRedditTypeIdentifier^>^ MixedContentSorter(Windows::Data::Json::JsonArray^ contents)
	{
		auto returning = ref new Platform::Collections::Vector<IRedditTypeIdentifier^>(contents->Size);
		if (contents->Size > 5)
		{
			
			concurrency::parallel_transform(Windows::Foundation::Collections::begin(contents), Windows::Foundation::Collections::end(contents), Windows::Foundation::Collections::begin(returning), [](Windows::Data::Json::IJsonValue^  value) {
				return AccountInterface::ambiguousJsonResolver(value->GetObject());
			});
		}
		else
		{
			std::transform(Windows::Foundation::Collections::begin(contents), Windows::Foundation::Collections::end(contents), Windows::Foundation::Collections::begin(returning), [](Windows::Data::Json::IJsonValue^  value){
				return AccountInterface::ambiguousJsonResolver(value->GetObject());
			});
		}
		return returning;
	}
}