﻿using System;
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
        private readonly Stack<AggregateParticle> particle_stack;
        private readonly GeometryModel3D particle_model;

        private readonly Point3DCollection particle_positions;
        private readonly Int32Collection triangle_indices;
        private readonly PointCollection tex_coords;

        /// <summary>
        /// Initialises a new instance of the AggregateSystem class.
        /// </summary>
        public AggregateSystem() {
            particle_list = new List<AggregateParticle>();
            particle_stack = new Stack<AggregateParticle>();
            particle_model = new GeometryModel3D { Geometry = new MeshGeometry3D() };
            // initialise ellipse with specified Width and Height
            Ellipse e = new Ellipse {
                Width = 32.0,
                Height = 32.0
            };

            RadialGradientBrush radial_grad_brush = new RadialGradientBrush();
            // set new GradientStops of radial_grad_brush
            radial_grad_brush.GradientStops.Add(new GradientStop(Color.FromArgb(0xFF,Colors.White.R, Colors.White.B, Colors.White.G), 0.25));
           // radial_grad_brush.GradientStops.Add(new GradientStop(Color.FromArgb(0x00,Colors.White.R, Colors.White.B, Colors.White.G), 1.0));
            // fill ellipse interior using radial_grad_brush
            e.Fill = radial_grad_brush;
            e.Measure(new Size(32, 32));
            e.Arrange(new Rect(0, 0, 32, 32));
            // initialise render target bitmap
            RenderTargetBitmap render_target = new RenderTargetBitmap(32, 32, 96, 96, PixelFormats.Pbgra32);
            // render ellipse to render_target
            render_target.Render(e);
            // set render_target to unmodifiable
            render_target.Freeze();
            Brush brush = new ImageBrush(render_target);

            DiffuseMaterial material = new DiffuseMaterial(brush);
            particle_model.Material = material;

            particle_positions = new Point3DCollection();
            triangle_indices = new Int32Collection();
            tex_coords = new PointCollection();
        }

        public Model3D AggregateModel => particle_model;

        /// <summary>
        /// Updates the aggregate system - applying all simulation view updates.
        /// </summary>
        public void Update() {
            UpdateSimulationView();
        }
        
        /// <summary>
        /// Updates the simulation geometry, adds any new aggregate particles to view.
        /// </summary>
        private void UpdateSimulationView() {
            // positions collection to store particle positions
            Point3DCollection positions = new Point3DCollection();
            // indices collection to store TriangleIndices
            Int32Collection indices = new Int32Collection();
            // point collection to store texture co-ordinates
            PointCollection texcoords = new PointCollection();

            // TODO: uncomment following section and remove for loop when synchronisation of threads is working
            /*AggregateParticle p = particle_stack.Peek();
            int position_index = particle_stack.Count * 4;
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
            triangle_indices.Add(position_index + 2);*/

            for (int i = 0; i < particle_list.Count; ++i) {
                int position_index = i * 4;
                // get particle instance at current index of list
                AggregateParticle p = particle_list[i];
                // create points according to particle co-ords
                Point3D p1 = new Point3D(p.position.X, p.position.Y, p.position.Z);
                Point3D p2 = new Point3D(p.position.X, p.position.Y + p.size, p.position.Z);
                Point3D p3 = new Point3D(p.position.X + p.size, p.position.Y + p.size, p.position.Z);
                Point3D p4 = new Point3D(p.position.X + p.size, p.position.Y, p.position.Z);
                // add points to particle positions collection
                positions.Add(p1);
                positions.Add(p2);
                positions.Add(p3);
                positions.Add(p4);
                // create points for texture co-ords
                Point t1 = new Point(0.0, 0.0);
                Point t2 = new Point(0.0, 1.0);
                Point t3 = new Point(1.0, 1.0);
                Point t4 = new Point(1.0, 0.0);
                // add texture co-ords points to texcoords collection
                texcoords.Add(t1);
                texcoords.Add(t2);
                texcoords.Add(t3);
                texcoords.Add(t4);
                // add position indices to indices collection
                indices.Add(position_index);
                indices.Add(position_index + 2);
                indices.Add(position_index + 1);
                indices.Add(position_index);
                indices.Add(position_index + 3);
                indices.Add(position_index + 2);
            }

            // set particle_model Geometry model properties 
            //((MeshGeometry3D)particle_model.Geometry).Positions = particle_positions;
            //((MeshGeometry3D)particle_model.Geometry).TriangleIndices = triangle_indices;
            //((MeshGeometry3D)particle_model.Geometry).TextureCoordinates = tex_coords;
            ((MeshGeometry3D)particle_model.Geometry).Positions = positions;
            ((MeshGeometry3D)particle_model.Geometry).TriangleIndices = indices;
            ((MeshGeometry3D)particle_model.Geometry).TextureCoordinates = texcoords;
        }

        public void Clear() {
            particle_list.Clear();
            particle_stack.Clear();
            ((MeshGeometry3D)particle_model.Geometry).Positions.Clear();
            ((MeshGeometry3D)particle_model.Geometry).TriangleIndices.Clear();
            ((MeshGeometry3D)particle_model.Geometry).TextureCoordinates.Clear();
            // TODO: fix bug where last (?) particle added on previous aggregate is rendered at start of next generation
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
            particle_stack.Push(agg_particle);
        }

    }

}


