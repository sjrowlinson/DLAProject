using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Text;
using System.Threading.Tasks;

namespace DLAProject {

    /// <summary>
    /// Manager for the AggregateSystem - provides methods for modifying aggregate simulation view.
    /// </summary>
    public class AggregateSystemManager {

        private readonly AggregateSystem agg_sys;

        /// <summary>
        /// Initialises a new instance of the AggregateSystemManager class.
        /// </summary>
        public AggregateSystemManager() {
            agg_sys = new AggregateSystem();
        }

        /// <summary>
        /// Adds a particle to the aggregate simulation view with specified properties.
        /// </summary>
        /// <param name="position">Position of particle in 3D space</param>
        /// <param name="colour">Colour of particle in terms of alpha, RGB channels</param>
        /// <param name="size">Size of particle</param>
        public void AddParticle(Point3D position, Color colour, double size) {
            agg_sys.SpawnParticle(position, colour, size);    
        }

        /// <summary>
        /// Gets the Model3D object associated with the aggregate system.
        /// </summary>
        /// <returns>The Model3D instance of the associated aggregate system.</returns>
        public Model3D AggregateSystemModel() {
            return agg_sys.AggregateModel;
        }

    }

}
