using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;

namespace DLAProject {

    public class AggregateSystem {

        private readonly List<AggregateParticle> particle_list;
        private readonly GeometryModel3D particle_model;
        
        public AggregateSystem() {
            particle_list = new List<AggregateParticle>();
            particle_model = new GeometryModel3D { Geometry = new MeshGeometry3D() };
        }

        public Model3D AggregateModel => particle_model;

        public void SpawnParticle(Point3D _position, Color _colour, double _size) {
            AggregateParticle agg_particle = new AggregateParticle {
                position = _position, colour = _colour, size = _size
            };
            particle_list.Add(agg_particle);
        }

    }

}


