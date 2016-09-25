using System;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Diagnostics;

namespace DLAProject.ParticleGeometry3D {
    /// <summary>
    /// Represents a spherical mesh geometry, inheriting from the abstract class `RoundGeometry3D`.
    /// </summary>
    public class SphereGeometry3D : RoundGeometry3D {
        /// <summary>
        /// Initialises an instance of the SphereGeometry3D class.
        /// </summary>
        public SphereGeometry3D() {}
        /// <summary>
        /// Computes a spherical mesh based on the Radius and Seperators properties of the
        /// symmetrical geometry.
        /// </summary>
        protected override void CalculateGeometry() {
            double segment_radius = Math.PI / 2.0 / (seperators + 1);
            int n_seperators = 4 * seperators + 4; 
            points = new Point3DCollection();
            triangle_indices = new Int32Collection();
            for (int i = -seperators; i <= seperators; ++i) {
                // compute azimuthal points
                double r_i = radius * Math.Cos(segment_radius * i);
                double y_i = radius * Math.Sin(segment_radius * i);
                // compute polar points
                for (int j = 0; j <= (n_seperators -1); ++j) {
                    double z_j = - r_i * Math.Sin(segment_radius * j);
                    double x_j = r_i * Math.Cos(segment_radius * j);
                    points.Add(new Point3D(x_j, y_i, z_j));
                }
            }
            points.Add(new Point3D(0.0, radius, 0.0));
            points.Add(new Point3D(0.0, -radius, 0.0));
            for (int i = 0; i < 2*seperators; ++i) {
                for (int j = 0; j < n_seperators; ++j) {
                    triangle_indices.Add(i * n_seperators + j);
                    triangle_indices.Add(i * n_seperators + j + n_seperators);
                    triangle_indices.Add(i * n_seperators + (j + 1) % n_seperators + n_seperators);
                    triangle_indices.Add(i * n_seperators + (j + 1) % n_seperators + n_seperators);
                    triangle_indices.Add(i * n_seperators + (j + 1) % n_seperators);
                    triangle_indices.Add(i * n_seperators + j);
                }
            }
            for (int i = 0; i < n_seperators; ++i) {
                triangle_indices.Add(2 * seperators * n_seperators + i);
                triangle_indices.Add(2 * seperators + (i + 1) % n_seperators);
                triangle_indices.Add(n_seperators * (2 * seperators + 1));
            }
            for (int i = 0; i < n_seperators; ++i) {
                triangle_indices.Add(i);
                triangle_indices.Add((i + 1) % n_seperators);
                triangle_indices.Add(n_seperators * (2 * seperators + 1) + 1);
            }
        }
    }
}
