using AxoSlicer_Ui.Interop;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace AxoSlicer_Ui.Utilities
{
    internal class Geometry : INotifyPropertyChanged
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

        bool _isVisible = true;
        public bool IsVisible
        {
            get => _isVisible;
            set
            {
                if (_isVisible != value)
                {
                    _isVisible = value;
                    OnPropertyChanged();
                }
            }
        }

        public void EnableTranslate( bool enable)
        {
            //nativeGeometry.Highight(enable); 
        }

        public event PropertyChangedEventHandler PropertyChanged;
        void OnPropertyChanged([CallerMemberName] string? name = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
