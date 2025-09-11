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
    [ComImport, Guid("b8c243af-ad61-41e5-a8a0-89579b988b1d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iGeometryManager
    {
        void AddGeometry(iGeometry Geometry);
        void RemoveGeometry(Guid Geometry);
        void RenderGeometries();
        void GetGlobalBoundingBox(out BoundingBox box);
        void getGeometryCount(out int count);
        void SetVisibility(Guid geometryID, bool visible);
        void setTranslateBox(Guid geomId, bool render);
        void PickGeometry(int x, int y, out Guid pickGeomId, IntPtr renderState);
        void PickGeomArrow(int x, int y, Guid geomId, out eViewDirection viewDir);
    }
}
