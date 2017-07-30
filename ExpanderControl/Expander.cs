using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Markup;
using Windows.System;
namespace ExpanderControl
{
    /// <summary>
    /// The <see cref="Expander"/> control allows user to show/hide content based on a boolean state
    /// </summary>
    [TemplateVisualState(Name = "Expanded", GroupName = "ExpandedStates")]
    [TemplateVisualState(Name = "Collapsed", GroupName = "ExpandedStates")]
    [TemplatePart(Name = "PART_ExpanderToggleButton", Type = typeof(ToggleButton))]
    [ContentProperty(Name = "Content")]
    public sealed partial class Expander : ContentControl
    {
        public Expander()
        {
            DefaultStyleResourceUri = new Uri("ms-appx:///ExpanderControl/Expander.xaml");
            DefaultStyleKey = typeof(Expander);
        }

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            if (IsExpanded)
            {
                VisualStateManager.GoToState(this, "Expanded", false);
            }

            var button = (ToggleButton)GetTemplateChild("PART_ExpanderToggleButton");

            if (button != null)
            {
                button.KeyDown -= ExpanderToggleButtonPart_KeyDown;
                button.KeyDown += ExpanderToggleButtonPart_KeyDown;
            }
        }

        private void OnExpanded(EventArgs args)
        {
            Expanded?.Invoke(this, args);
        }

        private void OnCollapsed(EventArgs args)
        {
            Collapsed?.Invoke(this, args);
        }

        private void ExpanderToggleButtonPart_KeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key != VirtualKey.Enter)
            {
                return;
            }

            var button = sender as ToggleButton;

            if (button == null)
            {
                return;
            }

            IsExpanded = !IsExpanded;

            e.Handled = true;
        }

        private void ExpandControl()
        {
            Expanding?.Invoke(this,EventArgs.Empty);
            VisualStateManager.GoToState(this, "Expanded", true);
            OnExpanded(EventArgs.Empty);
        }

        private void CollapseControl()
        {
            VisualStateManager.GoToState(this, "Collapsed", true);
            OnCollapsed(EventArgs.Empty);
        }
        private static bool resourceInitialized = false;
    }

}
