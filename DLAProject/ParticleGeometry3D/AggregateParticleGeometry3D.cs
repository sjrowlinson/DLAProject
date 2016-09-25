using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DLAProject.ParticleGeometry3D {
    /// <summary>
    /// Represents the geometry of a single diffusion limited aggregate particle.
    /// </summary>
    public class AggregateParticleGeometry3D : SphereGeometry3D {
        /// <summary>
        /// Initialises an instance of the AggregateParticleGeometry3D class.
        /// </summary>
        AggregateParticleGeometry3D() {
            Radius = 1;
            Seperators = 5;
        }
    }
}
