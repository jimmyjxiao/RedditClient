//
// subpostContentResizingPlaceholder.xaml.cpp
// Implementation of the subpostContentResizingPlaceholder class
//

#include "pch.h"
#include "subpostContentResizingPlaceholder.xaml.h"

using namespace Reddit;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

subpostContentResizingPlaceholder::subpostContentResizingPlaceholder()
{
	InitializeComponent();
	/*auto ParentExpaner = (ExpanderControl::Expander^)Parent;
	ParentExpaner->Expanding += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &subpostContentResizingPlaceholder::onParentExpanded);*/
}

void Reddit::subpostContentResizingPlaceholder::activatecontent()
{
	Windows::UI::Xaml::UIElement^ newelement = contentHelper->viewerControl();
	this->Content = newelement;
}


void Reddit::subpostContentResizingPlaceholder::onParentExpanded(Platform::Object ^, Platform::Object ^)
{
	if (!contentIntialized && contentHelper != nullptr)
		activatecontent();
}

void Reddit::subpostContentResizingPlaceholder::UserControl_ManipulationDelta(Platform::Object^ sender, Windows::UI::Xaml::Input::ManipulationDeltaRoutedEventArgs^ e)
{
	
}
