using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AxoSlicer_Ui.Utilities;

namespace AxoSlicer_Ui.ViewModels
{
    internal class MainViewModel
    {
        public static MainViewModel Instance { get; } = new MainViewModel();

        private SettingsViewModel settingsVM;

        public GeometryManager GeometryManager = new();

        public void AddGeometry(string filepath)
        {
            GeometryManager.AddGeometry(filepath);
        }
    }
}
