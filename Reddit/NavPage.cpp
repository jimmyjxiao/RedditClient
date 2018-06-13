#include "pch.h"
#include "NavStates.h"

void Reddit::NavPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	navIndex = static_cast<unsigned char>(e->Parameter);
	baseRef = static_cast<subredditNavstate*>(globalvars::NavState.at(navIndex).second);
	baseRef->pageState = this;
	OnNavigatedToPageCode();
}
