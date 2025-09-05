using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using System.Windows;

namespace AxoSlicer_Ui.Interop
{
    public static class NativeMethods
    {
        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createMainView(IntPtr hwnd, out iMainView mainview);

        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createGeometryManager(out iGeometryManager geometryManager);

        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createGeometry(IntPtr buffer, int length, out iGeometry geometry);

        [DllImport("SlicerCore.dll", CallingConvention = CallingConvention.StdCall)]
        internal static extern int createViewCube(IntPtr hwnd, out iViewCube viewCube);

    }
}
