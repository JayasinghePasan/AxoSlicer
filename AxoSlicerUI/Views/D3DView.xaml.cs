using AxoSlicer_Ui.Interop;
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using System.Windows.Controls;
using AxoSlicer_Ui.ViewModels;

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

        public enum MouseMode { Navigate, Pick }
        public MouseMode CurrentMode { get; set; } = MouseMode.Navigate;

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
            _lastPos = e.GetPosition(ViewHost);
            if (CurrentMode == MouseMode.Navigate)
            {
                if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                    _rotating = true;
                if (e.RightButton == System.Windows.Input.MouseButtonState.Pressed)
                    _panning = true;
            }
            ViewHost.CaptureMouse();
        }

        private void OnMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            _rotating = _panning = false;
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
            if (CurrentMode == MouseMode.Navigate)
            {
                if (_rotating)
                    _mainView.rotate(dx, dy);
                else if (_panning)
                    _mainView.pan(dx, dy);
            }
        }
    }
}
