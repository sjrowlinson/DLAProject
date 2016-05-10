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

    }

}
