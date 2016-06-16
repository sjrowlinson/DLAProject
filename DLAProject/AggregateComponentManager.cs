using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Text;
using System.Threading.Tasks;

namespace DLAProject {
    public class AggregateComponentManager {
        private readonly List<AggregateComponent> agg_comp_list;
        private int id_counter;
        private int current_counter;

        public AggregateComponentManager() {
            agg_comp_list = new List<AggregateComponent>();
            id_counter = 0;
            current_counter = -1;
        }

        public Model3D CreateAggregateComponent(Color _color) {
            AggregateComponent agg_comp = new AggregateComponent(_color, ++id_counter);
            agg_comp_list.Add(agg_comp);
            return agg_comp.ComponentModel;
        }

        public void AddParticleToComponent(Point3D _pos, double _size) {
            agg_comp_list[++current_counter].SpawnParticle(_pos, _size);
        }

        public void Update() {
            foreach (var p in agg_comp_list) {
                p.Update();
            }
        }

        public void Clear() {
            foreach (var p in agg_comp_list) {
                p.Clear();
            }
        }
    }
}
