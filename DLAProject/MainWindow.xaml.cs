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
        // pair holding most-recently-added pair to 2D aggregate
        private KeyValuePair<int, int> mra_cache_pair;
        // handle to AggregateSystemManager used for updating simulation render
        private readonly AggregateSystemManager aggregate_manager;

        public MainWindow() {
            InitializeComponent();
            // initalise aggregate containers
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
            mra_cache_pair = new KeyValuePair<int, int>();
            aggregate_manager = new AggregateSystemManager();
            isPaused = false;
            WorldModels.Children.Add(aggregate_manager.AggregateSystemModel());
        }

        /// <summary>
        /// Updates the aggregate using a timer with short interval, calling 
        /// AggregateUpdateOnTimeEvent periodically.
        /// </summary>
        /// <param name="_particle_slider_val">Number of particles to generate in aggregate.</param>
        private void AggregateUpdateListenerAlt(uint _particle_slider_val) {
            const double interval = 10.0;
            // initialise a Timer with a 5ms interval
            System.Timers.Timer timer = new System.Timers.Timer(interval);
            // repeatedly call AggregateUpdateOnTimedEvent every 'interval' ms
            if (dla_2d.Size() < _particle_slider_val) {
                timer.Elapsed += AggregateUpdateOnTimedEvent;
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
        private void AggregateUpdateOnTimedEvent(object source, ElapsedEventArgs e) {
            // lock around aggregate updating and batch queue processing to prevent 
            // non-dereferencable std::deque iterator run-time errors
            lock (locker) {
                // get and process the batch_queue from the DLA handle
                BlockingCollection<KeyValuePair<int, int>>  blocking_queue = dla_2d.ProcessBatchQueue();
                // loop over blocking_queue adding contents to interface and dequeueing on each iteration
                while (blocking_queue.Count != 0) {
                    KeyValuePair<int, int> agg_kvp = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_kvp.Key, agg_kvp.Value, 0);
                    aggregate_manager.AddParticle(pos, Colors.Red, 1.0);
                    // dispatch GUI updates to UI thread
                    Dispatcher.Invoke(() => { aggregate_manager.Update(); });
                }
            }
        }

        /// <summary>
        /// Checks for updates to the simulated DLA structure and performs
        /// rendering of any added particles. Should be called in a separate thread.
        /// </summary>
        /// <param name="_particle_slider_val">Number of particles to generate in aggregate.</param>
        private void AggregateUpdateListener(uint _particle_slider_val) {
            // continue execution until aggregate is completely generated
            while (dla_2d.Size() < _particle_slider_val) {
                // get the Most-Recently-Added aggregate particle
                KeyValuePair<int, int> agg_kvp = dla_2d.GetMRAParticle();
                if (agg_kvp.Equals(mra_cache_pair)) {
                    // no updates to aggregate
                }
                // aggregate has updated, add new particle to simulation view 
                else {
                    mra_cache_pair = agg_kvp;
                    Point3D position = new Point3D(agg_kvp.Key, agg_kvp.Value, 0);
                    aggregate_manager.AddParticle(position, Colors.Red, 1.0);
                    // dispatch particle rendering code to UI thread
                    Dispatcher.Invoke(() => { aggregate_manager.Update(); });
                }
            }             
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
            Task.Factory.StartNew(() => AggregateUpdateListenerAlt(particle_slider_val));
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
        }

    }
}
