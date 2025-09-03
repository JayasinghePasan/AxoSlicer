using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using AxoSlicer_Ui.Commands;
using Microsoft.Win32;

namespace AxoSlicer_Ui.ViewModels
{
    internal class SettingsViewModel
    {
        public ICommand OpenSTL { get; }

        public SettingsViewModel() 
        {
            OpenSTL = new RelayCommand( _ => OpenGeometry());
        }

        private void OpenGeometry()
        {
            var dialog = new OpenFileDialog
            {
                Filter = "STL files (*.stl) | *.stl",
                Title  = "Add Geometry"
            };

            if (dialog.ShowDialog() == true)
            {
                MainViewModel.Instance.AddGeometry(dialog.FileName);
            }
        }
    }
}
