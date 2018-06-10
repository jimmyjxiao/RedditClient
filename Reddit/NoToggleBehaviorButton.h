//
// NoToggleBehaviorButton.h
// Declaration of the NoToggleBehaviorButton class.
//

#pragma once

namespace Reddit
{
	public ref class NoToggleBehaviorButton sealed : Windows::UI::Xaml::Controls::Primitives::ToggleButton
	{
	public:
		NoToggleBehaviorButton();
	protected:
		void OnToggle() override;

		
	};

}
