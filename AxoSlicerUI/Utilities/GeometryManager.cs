using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using AxoSlicer_Ui.Interop;

namespace AxoSlicer_Ui.Utilities
{
    internal class GeometryManager
    {
        List<Geometry> geometries;
        iGeometryManager nativeGeometryManager = null;

        public GeometryManager()
        {
            geometries = new List<Geometry>();
            int hr = NativeMethods.createGeometryManager(out nativeGeometryManager);
            if (hr != 0 || nativeGeometryManager == null)
                throw new InvalidComObjectException("Geometry manager loading failed.");
        }

        public void AddGeometry(string filePath)
        {
            byte[] data = File.ReadAllBytes(filePath);
            IntPtr unmanagedData = Marshal.AllocHGlobal(data.Length);

            try
            {
                Marshal.Copy(data, 0, unmanagedData, data.Length);
                int hr = NativeMethods.createGeometry(unmanagedData, data.Length, out iGeometry nativeGrometry);

                if (hr != 0 || nativeGrometry == null)
                    throw new InvalidDataException($"Failed to load {filePath}.");

                // Add to front end
                Geometry geometry = new(filePath, nativeGrometry);
                geometries.Add(geometry);

                // Add to back end
                nativeGeometryManager.AddGeometry(nativeGrometry);
            }
            finally
            {
                Marshal.FreeHGlobal(unmanagedData);
            }
        }

        public void ResetManager()
        {
            geometries.Clear();
            nativeGeometryManager = null;
        }
    }
}
