#pragma once
#include "comment.h"
namespace account
{
	struct message : comment
	{
		Platform::String^ subject;
		bool is_new;
	};
}