using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AxoSlicer_Ui.Interop
{
    [ComImport, Guid("b8c243af-ad61-41e5-a8a0-89579b988b1d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iGeometryManager
    {
        void AddGeometry(iGeometry Geometry);
        void RemoveGeometry(Guid Geometry);
        void RenderGeometries();
    }
}
