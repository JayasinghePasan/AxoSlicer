using AxoSlicer_Ui.Interop;
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using System.Windows.Controls;
using AxoSlicer_Ui.ViewModels;
using AxoSlicer_Ui.Utilities;
using System.Diagnostics;

namespace AxoSlicer_Ui.Views
{
    public partial class D3DView : UserControl
    {
        private readonly D3DImage _d3dimage = new();
        private iMainView? _mainView;
        private IntPtr _surface = IntPtr.Zero;

        private Point _lastPos;
        private bool _rotating;
        private bool _panning;
        private eViewDirection _translateViewDir;

        public enum MouseMode { Navigate, Pick }
        public MouseMode CurrentMode { get; set; } = MouseMode.Navigate;
        private Guid _pickModeGeometryId;

        public D3DView()
        {
            InitializeComponent();
            Loaded += OnLoaded;
            CompositionTarget.Rendering += OnRendering;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            var source = (HwndSource)PresentationSource.FromVisual(this);
            var hwnd = source.Handle;
            if (NativeMethods.createMainView(hwnd, out _mainView) != 0)
            {
                return;
            }

            iGeometryManager nativeGeometryManager;
            if (NativeMethods.createGeometryManager(out nativeGeometryManager) != 0)
            {
                return;
            }
            MainViewModel.Instance.initializeMainView(_mainView, nativeGeometryManager);

            ViewHost.SizeChanged += OnHostSizeChanged;
            ViewHost.MouseWheel += OnMouseWheel;
            ViewHost.MouseDown += OnMouseDown;
            ViewHost.MouseUp += OnMouseUp;
            ViewHost.MouseMove += OnMouseMove;

            Dispatcher.BeginInvoke(new Action(() =>
            {
                ResizeAndSetSurface((int)ViewHost.ActualWidth, (int)ViewHost.ActualHeight);
            }), DispatcherPriority.Loaded);
        }

        private void OnHostSizeChanged(object sender, SizeChangedEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                ResizeAndSetSurface((int)e.NewSize.Width, (int)e.NewSize.Height);
            }), DispatcherPriority.Loaded);
        }

        private void OnRendering(object sender, EventArgs e)
        {
            if (_mainView == null)
                return;

            _mainView.render();

            if (_d3dimage.PixelWidth == 0 || _d3dimage.PixelHeight == 0)
                return;

            _d3dimage.Lock();
            _d3dimage.AddDirtyRect(new Int32Rect(0, 0, _d3dimage.PixelWidth, _d3dimage.PixelHeight));
            _d3dimage.Unlock();
        }

        private void ResizeAndSetSurface(int width, int height)
        {
            if (_mainView == null || width == 0 || height == 0)
                return;

            var dpi = VisualTreeHelper.GetDpi(this);

            _d3dimage.Lock();
            if (_surface != IntPtr.Zero)
            {
                _d3dimage.SetBackBuffer(D3DResourceType.IDirect3DSurface9, IntPtr.Zero);
                Marshal.Release(_surface);
                _surface = IntPtr.Zero;
            }
            _d3dimage.Unlock();
            _mainView.resize(width, height, (float)dpi.DpiScaleX);

            if (_mainView.getSurface(out var surface) == 0 && surface != IntPtr.Zero)
            {
                _d3dimage.Lock();
                _d3dimage.SetBackBuffer(D3DResourceType.IDirect3DSurface9, surface);
                _d3dimage.Unlock();

                if (ViewImage.Source != _d3dimage)
                    ViewImage.Source = _d3dimage;

                _surface = surface;
            }
        }

        private void OnMouseWheel(object sender, System.Windows.Input.MouseWheelEventArgs e)
        {
            if (CurrentMode == MouseMode.Navigate)
                _mainView?.zoom(e.Delta);
        }

        private void OnMouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (_mainView == null)
                return;

            _lastPos = e.GetPosition(ViewHost);

            // doucle clicks switch the mode to pick if double clicked on a geometry
            if (e.ClickCount >= 2)
            {
                if (CurrentMode == MouseMode.Navigate)
                {
                    _mainView.pickGeom((int)_lastPos.X, (int)_lastPos.Y, out Guid geomId);
                    if (geomId != Guid.Empty)
                    {
                        MainViewModel.Instance?.geometryManager?.EnablePickMode(geomId, true);
                        _pickModeGeometryId = geomId;
                        CurrentMode = MouseMode.Pick;
                    }
                }
                else
                {
                    MainViewModel.Instance?.geometryManager?.EnablePickMode(Guid.Empty, false);
                    _pickModeGeometryId = Guid.Empty;
                    CurrentMode = MouseMode.Navigate;
                }
                return;
            }

            // single clicks
            if (CurrentMode == MouseMode.Pick)
            {
                var pos = e.GetPosition(ViewHost);
                if (_pickModeGeometryId != Guid.Empty)
                {
                    _mainView.pickGeomArrow((int)pos.X, (int)pos.Y, _pickModeGeometryId, out _translateViewDir);
                    if (_translateViewDir != eViewDirection.Invalid)
                    {
                        ViewHost.CaptureMouse();
                        return;
                    }
                }
            }

            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                _rotating = true;
            if (e.RightButton == System.Windows.Input.MouseButtonState.Pressed)
                _panning = true;

            ViewHost.CaptureMouse();
        }

        private void OnMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            _rotating = _panning = false;
            _translateViewDir = eViewDirection.Invalid;
            ViewHost.ReleaseMouseCapture();
        }

        private void OnMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (_mainView == null)
                return;
            var pos = e.GetPosition(ViewHost);
            var dx = (float)(pos.X - _lastPos.X);
            var dy = (float)(pos.Y - _lastPos.Y);
            _lastPos = pos;
            if (CurrentMode == MouseMode.Navigate || (CurrentMode == MouseMode.Pick && _translateViewDir == eViewDirection.Invalid ))
            {
                if (_rotating)
                    MainViewModel.Instance.Rotate(dx, dy);
                else if (_panning)
                    _mainView.pan(dx, dy);
            }
            else if ( CurrentMode == MouseMode.Pick && _pickModeGeometryId != Guid.Empty && _translateViewDir != eViewDirection.Invalid)
            {
                switch (_translateViewDir)
                {
                    case eViewDirection.X_pos:
                    case eViewDirection.X_neg:
                        MainViewModel.Instance?.geometryManager.TranslateGeometry(_pickModeGeometryId, eViewDirection.X_pos, dx);
                        break;
                    case eViewDirection.Y_pos:
                    case eViewDirection.Y_neg:
                        MainViewModel.Instance?.geometryManager.TranslateGeometry(_pickModeGeometryId, eViewDirection.Y_pos, dy);
                        break;
                    case eViewDirection.Z_pos:
                    case eViewDirection.Z_neg:
                        MainViewModel.Instance?.geometryManager.TranslateGeometry(_pickModeGeometryId, eViewDirection.Z_pos, dx);
                        break;
                }
            }
        }
    }
}
