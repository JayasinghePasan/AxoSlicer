using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using System.Windows;

namespace AxoSlicer_Ui.Interop
{
    [ComImport, Guid("17941d20-efb9-494b-b8d7-a23b97b76d4e"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iRenderView
    {
        [PreserveSig] int render();
        [PreserveSig] int resize(int widthPixels, int heightPixels, float dpiScale);
        [PreserveSig] int getSurface( out IntPtr ppSurface);
    }

    [ComImport, Guid("d2f8c1b4-3e5a-4b0c-9f6d-7e1c8f3b2a1e"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iMainView : iRenderView
    {
        // parent methods
        int render();
        int resize(int widthPixels, int heightPixels, float dpiScale);
        int getSurface(out IntPtr ppSurface);

        int setGeometryManager(iGeometryManager nativeGeometryManager);
    }

    public static class NativeMethods
    {
        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createMainView(IntPtr hwnd, out iMainView mainview);

        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createGeometryManager(out iGeometryManager geometryManager);

        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createGeometry(IntPtr buffer, int length, out iGeometry geometry);

    }
}
