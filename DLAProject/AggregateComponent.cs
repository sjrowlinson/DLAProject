using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;

namespace DLAProject {

    public class AggregateComponent {
        private readonly GeometryModel3D component_model;
        private readonly Point3DCollection particle_positions;
        private readonly Int32Collection triangle_indices;
        private readonly PointCollection tex_coords;
        private AggregateParticle p;
        private readonly int id;

        public AggregateComponent(Color _color, int _id) {
            p = null;
            id = _id;
            component_model = new GeometryModel3D { Geometry = new MeshGeometry3D() };
            Ellipse ellipse = new Ellipse {
                Width = 32.0,
                Height = 32.0
            };
            RadialGradientBrush rb = new RadialGradientBrush();
            rb.GradientStops.Add(new GradientStop(_color, 0.0));
            rb.GradientStops.Add(new GradientStop(Colors.Black, 1.0));
            ellipse.Fill = rb;
            ellipse.Measure(new Size(32.0, 32.0));
            ellipse.Arrange(new Rect(0.0, 0.0, 32.0, 32.0));
            RenderTargetBitmap bm = new RenderTargetBitmap(32, 32, 96.0, 96.0, PixelFormats.Pbgra32);
            bm.Render(ellipse);
            bm.Freeze();
            ImageBrush ib = new ImageBrush(bm);
            DiffuseMaterial dm = new DiffuseMaterial(ib);
            component_model.Material = dm;
            particle_positions = new Point3DCollection();
            triangle_indices = new Int32Collection();
            tex_coords = new PointCollection();
        }

        public Model3D ComponentModel => component_model;

        public void Update() {
            if (p == null)
                return;
            int position_index = id * 4;
            // create points according to particle co-ords
            Point3D p1 = new Point3D(p.position.X, p.position.Y, p.position.Z);
            Point3D p2 = new Point3D(p.position.X, p.position.Y + p.size, p.position.Z);
            Point3D p3 = new Point3D(p.position.X + p.size, p.position.Y + p.size, p.position.Z);
            Point3D p4 = new Point3D(p.position.X + p.size, p.position.Y, p.position.Z);
            // add points to particle positions collection
            particle_positions.Add(p1);
            particle_positions.Add(p2);
            particle_positions.Add(p3);
            particle_positions.Add(p4);
            // create points for texture co-ords
            Point t1 = new Point(0.0, 0.0);
            Point t2 = new Point(0.0, 1.0);
            Point t3 = new Point(1.0, 1.0);
            Point t4 = new Point(1.0, 0.0);
            // add texture co-ords points to texcoords collection
            tex_coords.Add(t1);
            tex_coords.Add(t2);
            tex_coords.Add(t3);
            tex_coords.Add(t4);
            // add position indices to indices collection
            triangle_indices.Add(position_index);
            triangle_indices.Add(position_index + 2);
            triangle_indices.Add(position_index + 1);
            triangle_indices.Add(position_index);
            triangle_indices.Add(position_index + 3);
            triangle_indices.Add(position_index + 2);
            // set particle_model Geometry model properties 
            ((MeshGeometry3D)component_model.Geometry).Positions = particle_positions;
            ((MeshGeometry3D)component_model.Geometry).TriangleIndices = triangle_indices;
            ((MeshGeometry3D)component_model.Geometry).TextureCoordinates = tex_coords;
        }

        public void Clear() {
            ((MeshGeometry3D)component_model.Geometry).Positions.Clear();
            ((MeshGeometry3D)component_model.Geometry).TriangleIndices.Clear();
            ((MeshGeometry3D)component_model.Geometry).TextureCoordinates.Clear();
        }

        public void SpawnParticle(Point3D _pos, double _size) {
            p = new AggregateParticle {
                position = _pos,
                size = _size
            };
        }

    }

}
