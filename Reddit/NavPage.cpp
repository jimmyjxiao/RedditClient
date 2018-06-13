#include "pch.h"
#include "NavStates.h"

void Reddit::NavPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	navIndex = static_cast<unsigned char>(e->Parameter);
	baseRef = static_cast<subredditNavstate*>(globalvars::NavState.at(navIndex).second);
	baseRef->pageState = this;
	OnNavigatedToPageCode();
}

concurrency::cancellation_token Reddit::NavPage::RefreshSourceAndGetNewToken()
{
	refreshCancelationSource.cancel();
	refreshCancelationSource = concurrency::cancellation_token_source::create_linked_source(TaskCancellationSource.get_token());
	return refreshCancelationSource.get_token();
}
