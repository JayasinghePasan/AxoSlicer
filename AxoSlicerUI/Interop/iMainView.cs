using AxoSlicer_Ui.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace AxoSlicer_Ui.Interop
{
    [ComImport, Guid("17941d20-efb9-494b-b8d7-a23b97b76d4e"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iRenderView
    {
        [PreserveSig] int render();
        [PreserveSig] int resize(int widthPixels, int heightPixels, float dpiScale);
        [PreserveSig] int getSurface(out IntPtr ppSurface);
    }

    [ComImport, Guid("d2f8c1b4-3e5a-4b0c-9f6d-7e1c8f3b2a1e"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iMainView : iRenderView
    {
        // parent methods
        int render();
        int resize(int widthPixels, int heightPixels, float dpiScale);
        int getSurface(out IntPtr ppSurface);

        int setGeometryManager(iGeometryManager nativeGeometryManager);
        int zoom(float delta);
        int rotate(float dx, float dy);
        int pan(float dx, float dy);
        int resetView();
        int setProjection(int mode);
        int setViewMode(int mode);
        int pickGeom(int x, int y, out Guid geomId);
        int pickGeomArrow(int x, int y, Guid geomId, out eViewDirection viewDir);
    }

    [ComImport, Guid("3b72ec08-c4b7-4a0d-bc3d-8b4382b8eefa"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iViewCube : iRenderView
    {
        // parent methods
        int render();
        int resize(int widthPixels, int heightPixels, float dpiScale);
        int getSurface(out IntPtr ppSurface);

        int rotate(float dx, float dy);
        int pick(int x, int y, out int faceId);
        int setHighlight(uint faceId);
        int resetView();
        int setViewMode(int mode);
    }
}
