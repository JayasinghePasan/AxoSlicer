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
    }
}
