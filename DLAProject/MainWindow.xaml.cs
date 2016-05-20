using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Timers;
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
        // flag for pause state
        private bool isPaused;
        // handle to AggregateSystemManager used for updating simulation render
        private readonly AggregateSystemManager aggregate_manager;
        private uint current_particles;

        public MainWindow() {
            InitializeComponent();
            // initalise aggregate containers
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
            aggregate_manager = new AggregateSystemManager();
            isPaused = false;
            current_particles = 0;
            WorldModels.Children.Add(aggregate_manager.AggregateSystemModel());
        }

        /// <summary>
        /// Updates the aggregate using a timer with short interval, calling 
        /// AggregateUpdateOnTimeEvent periodically.
        /// </summary>
        /// <param name="_particle_slider_val">Number of particles to generate in aggregate.</param>
        private void AggregateUpdateListener(uint _particle_slider_val) {
            const double interval = 10.0;
            // initialise a Timer with a 5ms interval
            System.Timers.Timer timer = new System.Timers.Timer(interval);
            // repeatedly call AggregateUpdateOnTimedEvent every 'interval' ms
            if (dla_2d.Size() < _particle_slider_val) {
                timer.Elapsed += (sender, e) => AggregateUpdateOnTimedEvent(sender, e, _particle_slider_val);
                timer.AutoReset = true;
                timer.Enabled = true;
            }
            // stop timer and dispose all attached resources
            else {
                timer.Stop();
                timer.Dispose();
            }
        }

        /// <summary>
        /// Updates the aggregate based on current contents of DLA batch_queue - processes this
        /// batch_queue and adds its contents to the simulation view.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void AggregateUpdateOnTimedEvent(object source, ElapsedEventArgs e, uint _total_particles) {
            // lock around aggregate updating and batch queue processing to prevent 
            // non-dereferencable std::deque iterator run-time errors
            lock (locker) {
                // get and process the batch_queue from the DLA handle
                BlockingCollection<KeyValuePair<int, int>> blocking_queue = dla_2d.ProcessBatchQueue();
                // loop over blocking_queue adding contents to interface and dequeueing on each iteration
                while (blocking_queue.Count != 0) {
                    KeyValuePair<int, int> agg_kvp = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_kvp.Key, agg_kvp.Value, 0);
                    // TODO: compute particle_colour based on current_particle using formula for a cold-hot temp gradient
                    aggregate_manager.AddParticle(pos, ComputeColor(_total_particles), 1.0);
                    ++current_particles;
                    // dispatch GUI updates to UI thread
                    Dispatcher.Invoke(() => { aggregate_manager.Update(); });
                }
            }
        }

        /// <summary>
        /// Calculates the colour for a particle based on its generation number.
        /// </summary>
        /// <param name="_total_particles">Total number of particles to generate in aggregate.</param>
        /// <returns>Color object of generated particle.</returns>
        private Color ComputeColor(uint _total_particles) {
            Color colour = new Color();
            colour.ScA = 1;
            colour.ScR = (float)current_particles / _total_particles;
            colour.ScB = 1 - (float)current_particles / _total_particles;
            if (current_particles < _total_particles/2) {
                colour.ScG = (float)current_particles / _total_particles;
            }
            else {
                colour.ScG = 1 - (float)current_particles / _total_particles;
            }
            return colour;
        }
             
        /// <summary>
        /// Generates a Diffusion Limited Aggregate with properties initialised by
        /// current values of sliders and combo-boxes in the UI. Should be called
        /// in a separate thread.
        /// </summary>
        private void GenerateAggregate() {
            uint particle_slider_val = 0;
            // dispatch the particles_slider value access code to UI thread
            Dispatcher.Invoke(() => {
                particle_slider_val = (uint)particles_slider.Value;
            });
            // start asynchronous task calling AggregateUpdateListener to perform rendering
            Task.Factory.StartNew(() => AggregateUpdateListener(particle_slider_val));
            // generate the DLA using value of particle slider
            dla_2d.Generate(particle_slider_val);
        }

        /// <summary>
        /// Handler for generate_button click event. Calls GenerateAggregate() in a separate task factory.
        /// </summary>
        /// <param name="sender">Sender identification</param>
        /// <param name="e">Variable containing state information associated with event</param>
        private void GenerateButtonHandler(object sender, RoutedEventArgs e) {
            // clear any existing aggregate
            ClearButtonHandler(null, null);
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

        /// <summary>
        /// Handler for pause_button click event. Pauses simulation if running, resumes if paused.
        /// </summary>
        /// <param name="sender">Sender identification</param>
        /// <param name="e">Variable containing state information associated with event</param>
        private void PauseButtonHandler(object sender, RoutedEventArgs e) {
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

        /// <summary>
        /// Handler for clear_button click event. Clears the current aggregate data and simulation view.
        /// </summary>
        /// <param name="sender">Sender identification</param>
        /// <param name="e">Variable containing state information associated with event</param>
        private void ClearButtonHandler(object sender, RoutedEventArgs e) {
            dla_2d.Clear();
            dla_3d.Clear();
            // clear aggregate from user interface
            aggregate_manager.ClearAggregate();
            current_particles = 0;
        }

    }
}
