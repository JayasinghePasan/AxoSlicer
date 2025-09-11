using AxoSlicer_Ui.Interop;
using AxoSlicer_Ui.ViewModels;
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;

namespace AxoSlicer_Ui.Views
{
    public partial class ViewCubeView : UserControl
    {
        private readonly D3DImage _d3dimage = new();
        private iViewCube? _view;
        private IntPtr _surface = IntPtr.Zero;
        private Point _lastPos;
        private bool _rotating;

        public ViewCubeView()
        {
            InitializeComponent();
            Loaded += OnLoaded;
            Unloaded += (_, __) =>
            {
                MainViewModel.Instance.RotationChanged -= OnMainViewRotated;
                MainViewModel.Instance.ViewModeChanged -= OnViewModeChanged;
                MainViewModel.Instance.ViewReset -= OnMainViewReset;
            };
            CompositionTarget.Rendering += OnRendering;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            var source = (HwndSource)PresentationSource.FromVisual(this);
            var hwnd = source.Handle;
            if (NativeMethods.createViewCube(hwnd, out _view) != 0)
                return;

            MainViewModel.Instance.RotationChanged += OnMainViewRotated;
            MainViewModel.Instance.ViewModeChanged += OnViewModeChanged;
            MainViewModel.Instance.ViewReset += OnMainViewReset;

            CubeHost.SizeChanged += OnHostSizeChanged;
            CubeHost.MouseDown += OnMouseDown;
            CubeHost.MouseUp += OnMouseUp;
            CubeHost.MouseMove += OnMouseMove;

            Dispatcher.BeginInvoke(new Action(() =>
            {
                ResizeAndSetSurface((int)CubeHost.ActualWidth, (int)CubeHost.ActualHeight);
            }), DispatcherPriority.Loaded);
        }

        private void OnHostSizeChanged(object? sender, SizeChangedEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                ResizeAndSetSurface((int)e.NewSize.Width, (int)e.NewSize.Height);
            }), DispatcherPriority.Loaded);
        }

        private void OnRendering(object? sender, EventArgs e)
        {
            if (_view == null)
                return;
            _view.render();
            if (_d3dimage.PixelWidth == 0 || _d3dimage.PixelHeight == 0)
                return;
            _d3dimage.Lock();
            _d3dimage.AddDirtyRect(new Int32Rect(0, 0, _d3dimage.PixelWidth, _d3dimage.PixelHeight));
            _d3dimage.Unlock();
        }

        private void ResizeAndSetSurface(int width, int height)
        {
            if (_view == null || width == 0 || height == 0)
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
            _view.resize(width, height, (float)dpi.DpiScaleX);
            if (_view.getSurface(out var surface) == 0 && surface != IntPtr.Zero)
            {
                _d3dimage.Lock();
                _d3dimage.SetBackBuffer(D3DResourceType.IDirect3DSurface9, surface);
                _d3dimage.Unlock();
                if (ViewCubeImage.Source != _d3dimage)
                    ViewCubeImage.Source = _d3dimage;
                _surface = surface;
            }
        }

        private void OnMouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            _lastPos = e.GetPosition(CubeHost);
            _rotating = true;
            CubeHost.CaptureMouse();
        }

        private void OnMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            _rotating = false;
            CubeHost.ReleaseMouseCapture();
            if ( _view != null )
            {
                var pos = e.GetPosition(CubeHost);
                if ( _view.pick( (int)pos.X, (int)pos.Y, out int faceId) == 0 && faceId > -1 && faceId < 6)
                {
                    MainViewModel.Instance.SetViewMode(faceId);
                }
            }
        }

        private void OnMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (_view == null)
                return;
            var pos = e.GetPosition(CubeHost);
            if (_rotating && e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
            {
                float dx = (float)(pos.X - _lastPos.X);
                float dy = (float)(pos.Y - _lastPos.Y);
                MainViewModel.Instance.Rotate(dx, dy);
            }
            else
            {
                int face;
                if (_view.pick((int)pos.X, (int)pos.Y, out face) == 0)
                {
                    uint mask = face >= 0 ? (1u << face) : 0u;
                    _view.setHighlight(mask);
                }
            }
            _lastPos = pos;
        }

        private void OnMainViewRotated(float dx, float dy)
        {
            _view?.rotate(dx, dy);
        }
        private void OnViewModeChanged(int mode)
        {
            _view?.setViewMode(mode);
        }
        private void OnMainViewReset()
        {
            _view?.resetView();
        }
    }
}