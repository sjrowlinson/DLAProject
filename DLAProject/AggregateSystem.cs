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
           // Color colour = new Color();
            
            RadialGradientBrush b = new RadialGradientBrush();
            b.GradientStops.Add(new GradientStop(Color.FromArgb(0xFF,Colors.White.R, Colors.White.B, Colors.White.G), 0.25));
            b.GradientStops.Add(new GradientStop(Color.FromArgb(0x00,Colors.White.R, Colors.White.B, Colors.White.G), 1.0));
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

        public void Update() {
            UpdateSimulationView();
        }

        private void UpdateSimulationView() {
            Point3DCollection positions = new Point3DCollection();
            Int32Collection indices = new Int32Collection();
            PointCollection texcoords = new PointCollection();

            for (int i = 0; i < particle_list.Count; ++i) {
                int position_index = i * 4;
                int index = i * 6;
                AggregateParticle p = particle_list[i];

                Point3D p1 = new Point3D(p.position.X, p.position.Y, p.position.Z);
                Point3D p2 = new Point3D(p.position.X, p.position.Y + p.size, p.position.Z);
                Point3D p3 = new Point3D(p.position.X + p.size, p.position.Y + p.size, p.position.Z);
                Point3D p4 = new Point3D(p.position.X + p.size, p.position.Y, p.position.Z);

                positions.Add(p1);
                positions.Add(p2);
                positions.Add(p3);
                positions.Add(p4);

                Point t1 = new Point(0.0, 0.0);
                Point t2 = new Point(0.0, 1.0);
                Point t3 = new Point(1.0, 1.0);
                Point t4 = new Point(1.0, 0.0);

                texcoords.Add(t1);
                texcoords.Add(t2);
                texcoords.Add(t3);
                texcoords.Add(t4);

                indices.Add(position_index);
                indices.Add(position_index + 2);
                indices.Add(position_index + 1);
                indices.Add(position_index);
                indices.Add(position_index + 3);
                indices.Add(position_index + 2);
            }
            ((MeshGeometry3D)particle_model.Geometry).Positions = positions;
            ((MeshGeometry3D)particle_model.Geometry).TriangleIndices = indices;
            ((MeshGeometry3D)particle_model.Geometry).TextureCoordinates = texcoords;
        }

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


