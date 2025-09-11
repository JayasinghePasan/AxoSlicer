using AxoSlicer_Ui.Interop;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Documents;

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

        public void EnablePickMode(Guid geomId, bool enable)
        {
            if (geomId == Guid.Empty)
            {
                geometries.ToList().ForEach( geom => geom.nativeGeometry.Highlight(false));
                return;
            }
            Geometry geom = geometries.FirstOrDefault(g => g.geometryId == geomId);
            if (geom == null || !geom.IsVisible)
                return;
            geom.nativeGeometry.Highlight(enable);
        }

        public void TranslateGeometry(Guid geomId, eViewDirection dir, float d)
        {
            Geometry geom = geometries.FirstOrDefault(g => g.geometryId == geomId);
            if (geom == null || !geom.IsVisible)
                return;
            //geom.Translate
        }
    }
}
