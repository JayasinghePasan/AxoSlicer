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

        public event EventHandler GeometryManagerInitialized;

        public event Action<float, float> RotationChanged;
        public event Action<int> ViewModeChanged;
        public event Action ViewReset;

        public void initializeMainView(iMainView native_mainView, iGeometryManager native_geomManager)
        {
            _nativeMainView = native_mainView;
            _nativeMainView.setGeometryManager(native_geomManager);
            geometryManager = new GeometryManager(native_geomManager);
            GeometryManagerInitialized?.Invoke(this, EventArgs.Empty);
            ResetView();
        }

        public GeometryManager geometryManager { get; private set; } = null;

        public void AddGeometry(string filepath)
        {
            geometryManager.AddGeometry(filepath);
            ResetView();
        }

        public void Rotate(float dx, float dy)
        {
            _nativeMainView?.rotate(dx, dy);
            RotationChanged?.Invoke(dx, dy);
        }

        public void ResetView()
        {
            _nativeMainView?.resetView();
            ViewReset?.Invoke();
        }
        public void SetViewMode(int mode)
        {
            _nativeMainView?.setViewMode(mode);
            ViewModeChanged?.Invoke(mode);
        }
    }
}
