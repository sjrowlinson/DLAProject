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
        private GeometryModel3D component_model;
        private Point3DCollection particle_positions;
        private Int32Collection triangle_indices;
        private PointCollection tex_coords;
        private AggregateParticle p;
        private readonly int id;
        private bool is_done = false;

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
            rb.Freeze();
            ellipse.Fill = rb;
            ellipse.Measure(new Size(32.0, 32.0));
            ellipse.Arrange(new Rect(0.0, 0.0, 32.0, 32.0));
            //RenderTargetBitmap bm = new RenderTargetBitmap(32, 32, 96.0, 96.0, PixelFormats.Pbgra32);
            //bm.Render(ellipse);
            //bm.Freeze();
            //ImageBrush ib = new ImageBrush(bm);
            ellipse.CacheMode = new BitmapCache();
            BitmapCacheBrush ib = new BitmapCacheBrush(ellipse);
            DiffuseMaterial dm = new DiffuseMaterial(ib);
            component_model.Material = dm;
            particle_positions = new Point3DCollection();
            triangle_indices = new Int32Collection();
            tex_coords = new PointCollection();
            ((MeshGeometry3D)component_model.Geometry).Positions = particle_positions;
            ((MeshGeometry3D)component_model.Geometry).TriangleIndices = triangle_indices;
            ((MeshGeometry3D)component_model.Geometry).TextureCoordinates = tex_coords;
        }

        public Model3D ComponentModel => component_model;

        public void Update() {
            if (p == null)
                return;
            if (!is_done) {
                int position_index = id * 4;
                // add points to particle positions collection
                particle_positions.Add(new Point3D(p.position.X, p.position.Y, p.position.Z));
                particle_positions.Add(new Point3D(p.position.X, p.position.Y + p.size, p.position.Z));
                particle_positions.Add(new Point3D(p.position.X + p.size, p.position.Y + p.size, p.position.Z));
                particle_positions.Add(new Point3D(p.position.X + p.size, p.position.Y, p.position.Z));
                // add texture co-ords points to texcoords collection
                tex_coords.Add(new Point(0.0, 0.0));
                tex_coords.Add(new Point(0.0, 1.0));
                tex_coords.Add(new Point(1.0, 1.0));
                tex_coords.Add(new Point(1.0, 0.0));
                // add position indices to indices collection
                triangle_indices.Add(position_index);
                triangle_indices.Add(position_index + 2);
                triangle_indices.Add(position_index + 1);
                triangle_indices.Add(position_index);
                triangle_indices.Add(position_index + 3);
                triangle_indices.Add(position_index + 2);
                //is_done = true;
            }
        }

        public void Update3D() {
            if (p == null)
                return;
            // number of points to render for sphere
            const int n_points = 20;
            // initial azimuthal angle
            double phi_0 = 0.0;
            // initial polar angle
            double theta_0 = 0.0;
            // size of azimuthal angle increment
            double d_phi = Math.PI / n_points;
            // size of polar angle increment
            double d_theta = Math.PI / n_points;
            // length of side along y-axis, where p.size()/2 is the sphere radius
            double y0 = p.size/2 * Math.Cos(phi_0);
            // length of size parallel to x-z plane
            double r0 = p.size/2 * Math.Sin(phi_0);
            // azimuthal loop
            for (int i = 0; i < n_points; ++i) {
                double phi_1 = phi_0 + d_phi;
                double y1 = Math.Cos(phi_1);
                double r1 = Math.Sin(phi_1);
                theta_0 = 0.0;
                // create points with theta_0 polar angle
                Point3D p_00 = new Point3D(p.position.X + r0 * Math.Cos(theta_0),
                    p.position.Y + y0,
                    p.position.Z + r0 * Math.Sin(theta_0));
                Point3D p_10 = new Point3D(p.position.X + r1 * Math.Cos(theta_0),
                    p.position.Y + y1,
                    p.position.Z + r1 * Math.Sin(theta_0));
                // polar loop
                for (int j = 0; j < n_points; ++j) {
                    double theta_1 = theta_0 + d_theta;
                    // create points with theta_1 polar angle
                    Point3D p_01 = new Point3D(p.position.X + r0 * Math.Cos(theta_1),
                        p.position.Y + y0,
                        p.position.Z + r0 * Math.Sin(theta_1));
                    Point3D p_11 = new Point3D(p.position.X + r1 * Math.Cos(theta_1),
                        p.position.Y + y1,
                        p.position.Z + r1 * Math.Sin(theta_1));
                    // create triangles from points
                    CreateTriangle(p_00, p_11, p_10);
                    CreateTriangle(p_00, p_01, p_11);
                    // move to next theta
                    theta_0 = theta_1;
                    p_00 = p_01;
                    p_10 = p_11;
                }
                // move to next phi
                phi_0 = phi_1;
                y0 = y1;
                r0 = r1;
            }
        }

        public void CreateTriangle(Point3D _p1, Point3D _p2, Point3D _p3) {

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
