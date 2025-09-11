using AxoSlicer_Ui.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace AxoSlicer_Ui.Interop
{
    [ComImport, Guid("83398f8d-cd43-4388-a5f4-1401721aaa56"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface iGeometry
    {
        void getGuid(out Guid guid);
        void Render();
        void GetBoundingBox(out BoundingBox box);
        void Translate(float dx, float dy, float dz);
        void Highlight(bool highliht);
    }
}
