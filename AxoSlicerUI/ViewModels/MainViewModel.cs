using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AxoSlicer_Ui.Interop;
using AxoSlicer_Ui.Utilities;

namespace AxoSlicer_Ui.ViewModels
{
    internal class MainViewModel
    {
        public static MainViewModel Instance { get; } = new MainViewModel();

        private iMainView _nativeMainView;
        public iMainView nativeMainView => _nativeMainView;

        public void initializeMainView(iMainView native_mainView, iGeometryManager native_geomManager)
        {
            _nativeMainView = native_mainView;
            _nativeMainView.setGeometryManager(native_geomManager);
            GeometryManager = new GeometryManager(native_geomManager);
        }

        private SettingsViewModel settingsVM;

        public GeometryManager GeometryManager = null;

        public void AddGeometry(string filepath)
        {
            GeometryManager.AddGeometry(filepath);
            _nativeMainView?.resetView();
        }
    }
}
