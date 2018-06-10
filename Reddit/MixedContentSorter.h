#pragma once
#include "VotableThing.h"
namespace account
{

	Platform::Collections::Vector<IRedditTypeIdentifier^>^ MixedContentSorter(Windows::Data::Json::JsonArray^ contents);

}