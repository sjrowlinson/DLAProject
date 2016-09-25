using System;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace DLAProject.ParticleGeometry3D {
    /// <summary>
    /// Represents an abstract geometrical mesh with symmetrical properties.
    /// </summary>
    public abstract class RoundGeometry3D {
        protected int seperators = 10;
        protected int radius = 20;
        protected Point3DCollection points;
        protected Int32Collection triangle_indices;
        /// <summary>
        /// Gets or sets the radius of the symmetrical mesh.
        /// </summary>
        public virtual int Radius {
            get { return radius; }
            set { radius = value; CalculateGeometry(); }
        }
        /// <summary>
        /// Gets or sets the number of seperators of the symmetrical mesh.
        /// </summary>
        public virtual int Seperators {
            get { return seperators; }
            set { seperators = value; CalculateGeometry(); }
        }
        /// <summary>
        /// Gets the Point3DCollection representing the surface points of the symmetrical mesh.
        /// </summary>
        public Point3DCollection Points {
            get { return points; }
        }
        /// <summary>
        /// Gets the Int32Collection representing the triangle indices of the symmetrical mesh.
        /// </summary>
        public Int32Collection TriangleIndices {
            get { return triangle_indices; }
        }
        /// <summary>
        /// Compute the geometry of the symmetrical mesh.
        /// </summary>
        protected abstract void CalculateGeometry();
    }
}
