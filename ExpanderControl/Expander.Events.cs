using System;
namespace ExpanderControl
{
    public partial class Expander
    {
        /// <summary>
        /// Fires when the expander is opened
        /// </summary>
        /// 
        public event EventHandler<Object> Expanding;
        public event EventHandler<Object> Expanded;

        /// <summary>
        /// Fires when the expander is closed
        /// </summary>
        public event EventHandler<Object> Collapsed;
    }
}