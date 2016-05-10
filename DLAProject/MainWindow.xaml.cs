using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using DLAClassLibrary;

namespace DLAProject {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        // lock object for multi-threading tasks
        private static readonly object locker = new object();
        // handles to ManagedDLAContainer objects
        private readonly ManagedDLA2DContainer dla_2d;
        private readonly ManagedDLA3DContainer dla_3d;
        private bool isPaused;
        private KeyValuePair<int, int> mra_cache_pair;

        private readonly AggregateSystemManager aggregate_manager;

        public MainWindow() {
            InitializeComponent();
            // initalise aggregate containers
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
            mra_cache_pair = new KeyValuePair<int, int>();
            aggregate_manager = new AggregateSystemManager();
            isPaused = false;
        }

        private void AggregateUpdateListener(uint _particle_slider_val) {
            lock (locker) {
                while (dla_2d.Size() < _particle_slider_val-1) {
                    // get the Most-Recently-Added aggregate particle
                    KeyValuePair<int, int> agg_kvp = dla_2d.GetMRAParticle();
                    if (agg_kvp.Equals(mra_cache_pair)) {
                        // no updates to aggregate
                    }
                    // aggregate has updated, add new particle to simulation view
                    else {
                        Point3D position = new Point3D(agg_kvp.Key, agg_kvp.Value, 0);
                        // dispatch particle addition code to UI thread
                        Dispatcher.Invoke(() => { aggregate_manager.AddParticle(position, Colors.Red, 0.5); });
                    }
                }
            }
        }

        private void GenerateAggregate() {
            // lock around aggregate generation
            lock (locker) {
                // TODO: lock the particle_slider to prevent changes whilst simulating
                uint particle_slider_val = 0;
                // dispatch the particles_slider value access code to UI thread
                Dispatcher.Invoke(() => {
                    particle_slider_val = (uint)particles_slider.Value;
                });
                Task.Factory.StartNew(() => AggregateUpdateListener(particle_slider_val));
                // generate the DLA using value of particle slider
                dla_2d.Generate(particle_slider_val);
                
                // TODO: add particles to "canvas" on GUI as they are generated - will require
                // using Dispatcher.Invoke to update the GUI
            }
        }

        private void GenerateButtonClick(object sender, RoutedEventArgs e) {
            // set the coefficient of stickiness of aggregate
            // to current value of stickiness_slider
            dla_2d.SetCoeffStick(stickiness_slider.Value);

            // set the lattice type to current selected item
            // of latticeType_ComboBox ui element
            ComboBoxItem selected_latticeType = (ComboBoxItem)(latticeType_ComboBox.SelectedValue);
            string lattice_type_str = (string)(selected_latticeType.Content);
            ManagedLatticeType lattice_type = (ManagedLatticeType)Enum.Parse(typeof(ManagedLatticeType), lattice_type_str);
            dla_2d.SetLatticeType(lattice_type);

            // set the attractor type to current selected item
            // of attractorType_ComboBox ui element
            ComboBoxItem selected_AttractorType = (ComboBoxItem)(attractorType_ComboBox.SelectedValue);
            string attractor_type_str = (string)(selected_AttractorType.Content);
            ManagedAttractorType attractor_type = (ManagedAttractorType)Enum.Parse(typeof(ManagedAttractorType), attractor_type_str);
            dla_2d.SetAttractorType(attractor_type);

            // start asynchronous task calling GenerateAggregate method
            Task.Factory.StartNew(() => GenerateAggregate());
        }

        private void PauseButtonClick(object sender, RoutedEventArgs e) {
            // TODO: implement pause functionality
            if (!isPaused) {
                pause_button.Content = "Resume";
                isPaused = true;
            }
            else {
                pause_button.Content = "Pause";
                isPaused = false;
            }
        }

        private void ClearButtonClick(object sender, RoutedEventArgs e) {
            dla_2d.Clear();
            dla_3d.Clear();
            // TODO: clear aggregate from GUI
        }

    }
}
