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
    internal class SettingsViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        void OnPropertyChanged([CallerMemberName] string? name = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));

        public ICommand OpenSTL { get; }
        public ICommand DeleteGeometry { get; }
        public ICommand ToggleVisibility { get; }

        public SettingsViewModel() 
        {
            OpenSTL = new RelayCommand( _ => OpenGeometry());

            DeleteGeometry = new RelayCommand(g =>
            {
                if (g is Utilities.Geometry geom)
                    MainViewModel.Instance.GeometryManager.RemoveGeometry(geom.geometryId);
            });

            ToggleVisibility = new RelayCommand(g =>
            {
                if (g is Utilities.Geometry geom)
                    MainViewModel.Instance.GeometryManager.ToggleVisibility(geom.geometryId);
            });
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

        private bool _isOrthographic;
        public bool IsOrthographic
        {
            get => _isOrthographic;
            set
            {
                if (_isOrthographic != value)
                {
                    _isOrthographic = value;
                    OnPropertyChanged();
                    MainViewModel.Instance.nativeMainView?.setProjection(_isOrthographic ? 1 : 0);
                    MainViewModel.Instance.nativeMainView?.resetView();
                }
            }
        }

        public System.Collections.ObjectModel.ObservableCollection<Utilities.Geometry> Geometries
            => MainViewModel.Instance.GeometryManager?.Geometries;
    }
}
