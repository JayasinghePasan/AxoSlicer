using AxoSlicer_Ui.Interop;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace AxoSlicer_Ui.Utilities
{
    internal class GeometryManager
    {
        ObservableCollection<Geometry> geometries;
        iGeometryManager nativeGeometryManager = null;

        public GeometryManager(iGeometryManager nativeManager)
        {
            this.geometries = new ObservableCollection<Geometry>();
            this.nativeGeometryManager = nativeManager;
        }

        public ObservableCollection<Geometry> Geometries => geometries;

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

        public void RemoveGeometry(Guid geometryId)
        {
            var geometry = geometries.FirstOrDefault(g => g.geometryId == geometryId);
            if (geometry != null)
            {
                nativeGeometryManager.RemoveGeometry(geometryId);
                geometries.Remove(geometry);
            }
        }

        public void ToggleVisibility(Guid geometryId)
        {
            var geometry = geometries.FirstOrDefault(g => g.geometryId == geometryId);
            if (geometry != null)
            {
                geometry.IsVisible = !geometry.IsVisible;
                nativeGeometryManager.SetVisibility(geometryId, geometry.IsVisible);
            }
        }

        public void ResetManager()
        {
            geometries.Clear();
            nativeGeometryManager = null;
        }
    }
}
