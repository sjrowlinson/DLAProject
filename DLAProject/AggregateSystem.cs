using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;

namespace DLAProject {

    /// <summary>
    /// Represents the view / graphics associated with a single diffusion
    /// limited aggregate system.
    /// </summary>
    public class AggregateSystem {
        
        private readonly List<AggregateParticle> particle_list;
        private readonly GeometryModel3D particle_model;
        
        /// <summary>
        /// Initialises a new instance of the AggregateSystem class.
        /// </summary>
        public AggregateSystem() {
            particle_list = new List<AggregateParticle>();
            particle_model = new GeometryModel3D { Geometry = new MeshGeometry3D() };

            Ellipse e = new Ellipse {
                Width = 32.0,
                Height = 32.0
            };

            RadialGradientBrush b = new RadialGradientBrush();
            b.GradientStops.Add(new GradientStop());
            e.Fill = b;
            e.Measure(new Size(32, 32));
            e.Arrange(new Rect(0, 0, 32, 32));

            RenderTargetBitmap render_target = new RenderTargetBitmap(32, 32, 96, 96, PixelFormats.Pbgra32);
            render_target.Render(e);
            render_target.Freeze();
            Brush brush = new ImageBrush(render_target);


            DiffuseMaterial material = new DiffuseMaterial(brush);

            particle_model.Material = material;

        }

        public Model3D AggregateModel => particle_model;

        /// <summary>
        /// Spawns a new AggregateParticle with specified properties.
        /// </summary>
        /// <param name="_position">Position in 3D space of particle.</param>
        /// <param name="_colour">Colour of particle.</param>
        /// <param name="_size">Size of particle.</param>
        public void SpawnParticle(Point3D _position, Color _colour, double _size) {
            AggregateParticle agg_particle = new AggregateParticle {
                position = _position, colour = _colour, size = _size
            };
            // add agg_particle to particle_list container
            particle_list.Add(agg_particle);
        }

    }

}


