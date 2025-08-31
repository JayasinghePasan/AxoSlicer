using AxoSlicer_Ui.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AxoSlicer_Ui.Utilities
{
    internal class Geometry
    {
        public string filepath {  get;  }
        public iGeometry nativeGeometry { get; private set; }
        public Guid geometryId { get;  }

        public Geometry(string filepath, iGeometry nativeGeometry)
        {
            this.filepath = filepath;
            this.nativeGeometry = nativeGeometry;
            nativeGeometry.getGuid(out Guid tempIdHolder);
            this.geometryId = tempIdHolder;
        }
    }
}
