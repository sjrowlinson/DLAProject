using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace DLAProject {
    /// <summary>
    /// Structure containing meta-data of a particle in the aggregate view.
    /// </summary>
    public class AggregateParticle {
        public Point3D position;
        public double size;
        public Color colour;
    }
}
