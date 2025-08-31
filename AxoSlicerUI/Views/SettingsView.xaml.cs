using System.Windows.Controls;
using AxoSlicer_Ui.ViewModels;

namespace AxoSlicer_Ui.Views
{
    public partial class SettingsPanel : UserControl
    {
        public SettingsPanel()
        {
            InitializeComponent();
            DataContext = new SettingsViewModel();
        }
    }
}
