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
            // create an AggregateComponent with given color and id
            agg_comp_list.Add(new AggregateComponent(_color, ++id_counter));
            return agg_comp_list[agg_comp_list.Count - 1].ComponentModel;
        }

        public void AddParticleToComponent(Point3D _pos, double _size) {
            // spawn the particle corresponding to next index of agg_comp_list
            agg_comp_list[++current_counter].SpawnParticle(_pos, _size);
        }

        public void Update() {
            // update all components
            foreach (var p in agg_comp_list) {
                p.Update();
            }
        }

        public void Clear() {
            id_counter = 0;
            current_counter = -1;
            // clear all components
            foreach (var p in agg_comp_list) {
                p.Clear();
            }
            agg_comp_list.Clear();
        }
    }
}
