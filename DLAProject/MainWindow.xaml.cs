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
    /// Enum representing dimensions of lattice structure.
    /// </summary>
    public enum LatticeDimension {
        _2D,
        _3D
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        // lock object for multi-threading tasks
        private static readonly object locker = new object();
        // TODO: implement locks around aggregate generation methods for pause functionality
        // whilst using Monitor.Enter(pause_obj) and Monitor.Exit(pause_obj) in pause handler
        private static readonly object pause_obj = new object();
        // handles to ManagedDLAContainer objects
        private readonly ManagedDLA2DContainer dla_2d;
        private readonly ManagedDLA3DContainer dla_3d;
        // flags for paused and finished states
        private bool isPaused;
        private bool hasFinished;
        // handle to AggregateSystemManager used for updating simulation render
        private readonly AggregateSystemManager aggregate_manager;
        private TrackView trackview;
        private uint current_particles;
        private List<Color> colour_list;
        private LatticeDimension lattice_dimension;

        public MainWindow() {
            InitializeComponent();
            // initalise aggregate containers
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
            aggregate_manager = new AggregateSystemManager();
            isPaused = false;
            hasFinished = true;
            current_particles = 0;
            colour_list = new List<Color>();
            lattice_dimension = LatticeDimension._2D;
            WorldModels.Children.Add(aggregate_manager.AggregateSystemModel());
        }

        /// <summary>
        /// Method called on loading MainWindow. Initialises trackview ready for 
        /// transforming simulation viewport.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLoaded(object sender, EventArgs e) {
            // create trackview instance for rotating simulation view
            trackview = new TrackView();
            // attach main window to trackview mouse handlers
            trackview.Attach(this);
            // assign Viewport3D world to trackview viewport slave
            trackview.Viewport = World;
            trackview.Enabled = true;
        }

        /// <summary>
        /// Updates the aggregate using a timer with short interval, calling 
        /// AggregateUpdateOnTimeEvent periodically.
        /// </summary>
        /// <param name="_particle_slider_val">Number of particles to generate in aggregate.</param>
        private void AggregateUpdateListener(uint _particle_slider_val) {
            // interval of timer for refreshing aggregate in ms
            const double interval = 10.0;
            // initialise a Timer with a 10ms interval
            System.Timers.Timer timer = new System.Timers.Timer(interval);
            // repeatedly call AggregateUpdateOnTimedEvent every 'interval' ms
            switch (lattice_dimension) {
                case LatticeDimension._2D:
                    if (dla_2d.Size() < _particle_slider_val) {
                        timer.Elapsed += Aggregate2DUpdateOnTimedEvent;
                        timer.AutoReset = true;
                        timer.Enabled = true;
                    }
                    // stop timer and dispose all attached resources
                    else {
                        timer.Stop();
                        timer.Dispose();
                    }
                    break;
                case LatticeDimension._3D:
                    if (dla_3d.Size() < _particle_slider_val) {
                        timer.Elapsed += Aggregate3DUpdateOnTimedEvent;
                        timer.AutoReset = true;
                        timer.Enabled = true;
                    }
                    // stop timer and dispose all attached resources
                    else {
                        timer.Stop();
                        timer.Dispose();
                    }
                    break;
            }
            
        }

        /// <summary>
        /// Updates a 2D aggregate based on current contents of dla_2d batch_queue - processes this
        /// batch_queue and adds its contents to the simulation view.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void Aggregate2DUpdateOnTimedEvent(object source, ElapsedEventArgs e) {
            // lock around aggregate updating and batch queue processing to prevent 
            // non-dereferencable std::deque iterator run-time errors
            lock (locker) {
                // get and process the batch_queue from the DLA handle
                BlockingCollection<KeyValuePair<int, int>> blocking_queue = dla_2d.ProcessBatchQueue();
                // loop over blocking_queue adding contents to interface and dequeueing on each iteration
                while (blocking_queue.Count != 0) {
                    KeyValuePair<int, int> agg_kvp = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_kvp.Key, agg_kvp.Value, 0);
                    aggregate_manager.AddParticle(pos, colour_list[(int)current_particles], 1.0);
                    // dispatch GUI updates to UI thread
                    Dispatcher.Invoke(() => {
                        aggregate_manager.Update();
                        DynamicParticleLabel.Content = "Particles: " + current_particles;
                    });
                    ++current_particles;
                }
            }
        }
        
        /// <summary>
        /// Updates a 3D aggregate based on current contents of dla_3d batch_queue - processes this
        /// batch_queue and adds its contents to the simulation view.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void Aggregate3DUpdateOnTimedEvent(object source, ElapsedEventArgs e) {
            // lock around aggregate updating and batch queue processing to prevent 
            // non-dereferencable std::deque iterator run-time errors
            lock (locker) {
                // get and process the batch_queue from the DLA handle
                BlockingCollection<Tuple<int, int, int>> blocking_queue = dla_3d.ProcessBatchQueue();
                // loop over blocking_queue adding contents to interface and dequeueing on each iteration
                while (blocking_queue.Count != 0) {
                    Tuple<int, int, int> agg_tuple = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_tuple.Item1, agg_tuple.Item2, agg_tuple.Item3);
                    aggregate_manager.AddParticle(pos, colour_list[(int)current_particles], 1.0);
                    ++current_particles;
                    // dispatch GUI updates to UI thread
                    Dispatcher.Invoke(() => {
                        aggregate_manager.Update();
                        DynamicParticleLabel.Content = "Particles: " + current_particles;
                    });
                }
            }
        }

        /// <summary>
        /// Fills the colour_list field with colour instances for 
        /// each particle to be generated in an aggregate. This 
        /// method uses a progressive temperature gradient from
        /// cold to hot for each subsequent particle in the aggregate. 
        /// </summary>
        /// <param name="_total_particles">Total number of particles to be generated.</param>
        private void ComputeColorList(uint _total_particles) {
            for (uint i = 0; i <= _total_particles; ++i) {
                Color colour = new Color();
                colour.ScA = 1;
                colour.ScR = (float)i / _total_particles;
                colour.ScB = 1 - (float)i / _total_particles;
                if (i < _total_particles/2) {
                    colour.ScG = (float)i / _total_particles;
                }
                else {
                    colour.ScG = 1 - (float)i / _total_particles;
                }
                colour_list.Add(colour);
            }
        }

        /// <summary>
        /// Generates a Diffusion Limited Aggregate with properties initialised by
        /// current values of sliders and combo-boxes in the UI. Should be called
        /// in a separate thread.
        /// </summary>
        private void GenerateAggregate() {
            hasFinished = false;
            uint particle_slider_val = 0;
            // dispatch the particles_slider value access code to UI thread
            Dispatcher.Invoke(() => {
                particle_slider_val = (uint)particles_slider.Value;
            });
            // start asynchronous task calling AggregateUpdateListener to perform rendering
            Task.Factory.StartNew(() => AggregateUpdateListener(particle_slider_val));
            // generate the DLA using value of particle slider
            switch (lattice_dimension) {
                case LatticeDimension._2D:
                    dla_2d.Generate(particle_slider_val);
                    break;
                case LatticeDimension._3D:
                    dla_3d.Generate(particle_slider_val);
                    break;
            }
            hasFinished = true;
        }

        /// <summary>
        /// Sets up all the properties of the aggregate necessary for generation, including 
        /// lattice type, attractor type and the dimensions of the aggregate.
        /// </summary>
        private void SetUpAggregateProperties() {
            // get the selected dimension type
            ComboBoxItem selected_dimension = (ComboBoxItem)(dimension_ComboBox.SelectedValue);
            string dimension_str = (string)(selected_dimension.Content);
            // set corresponding lattice_dimension constant
            switch (dimension_str) {
                case "2D":
                    lattice_dimension = LatticeDimension._2D;
                    break;
                case "3D":
                    lattice_dimension = LatticeDimension._3D;
                    break;
            }
            // reset simulation view
            ResetViewButtonHandler(null, null);
            // switch on current lattice dimension constant
            switch (lattice_dimension) {
                case LatticeDimension._2D:
                    dla_2d.SetCoeffStick(stickiness_slider.Value);
                    // set the lattice type to current selected item
                    // of latticeType_ComboBox ui element
                    ComboBoxItem selected_2DlatticeType = (ComboBoxItem)(latticeType_ComboBox.SelectedValue);
                    string lattice_type2D_str = (string)(selected_2DlatticeType.Content);
                    ManagedLatticeType lattice_type2D = (ManagedLatticeType)Enum.Parse(typeof(ManagedLatticeType), lattice_type2D_str);
                    dla_2d.SetLatticeType(lattice_type2D);
                    // set the attractor type to current selected item
                    // of attractorType_ComboBox ui element
                    ComboBoxItem selected_2DAttractorType = (ComboBoxItem)(attractorType_ComboBox.SelectedValue);
                    string attractor_type2D_str = (string)(selected_2DAttractorType.Content);
                    ManagedAttractorType attractor_type2D = (ManagedAttractorType)Enum.Parse(typeof(ManagedAttractorType), attractor_type2D_str);
                    dla_2d.SetAttractorType(attractor_type2D);
                    break;
                case LatticeDimension._3D:
                    dla_3d.SetCoeffStick(stickiness_slider.Value);
                    // set the lattice type to current selected item
                    // of latticeType_ComboBox ui element
                    ComboBoxItem selected_3DlatticeType = (ComboBoxItem)(latticeType_ComboBox.SelectedValue);
                    string lattice_type3D_str = (string)(selected_3DlatticeType.Content);
                    ManagedLatticeType lattice_type3D = (ManagedLatticeType)Enum.Parse(typeof(ManagedLatticeType), lattice_type3D_str);
                    dla_3d.SetLatticeType(lattice_type3D);
                    // set the attractor type to current selected item
                    // of attractorType_ComboBox ui element
                    ComboBoxItem selected_3DAttractorType = (ComboBoxItem)(attractorType_ComboBox.SelectedValue);
                    string attractor_type3D_str = (string)(selected_3DAttractorType.Content);
                    ManagedAttractorType attractor_type3D = (ManagedAttractorType)Enum.Parse(typeof(ManagedAttractorType), attractor_type3D_str);
                    dla_3d.SetAttractorType(attractor_type3D);
                    break;
            }
        }

        /// <summary>
        /// Handler for generate_button click event. Calls GenerateAggregate() in a separate task factory.
        /// </summary>
        /// <param name="sender">Sender identification</param>
        /// <param name="e">Variable containing state information associated with event</param>
        private void GenerateButtonHandler(object sender, RoutedEventArgs e) {
            // clear any existing aggregate
            if (current_particles > 0)
                ClearButtonHandler(null, null);
            // (re)-initialise aggregate properties
            SetUpAggregateProperties();
            // pre-compute colour_list for each particle in aggregate
            ComputeColorList((uint)particles_slider.Value);
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
            // if an aggregate exists, clear it
            if (current_particles > 0) {
                // switch on dimension of lattice
                switch (lattice_dimension) {
                    case LatticeDimension._2D:
                        // if generation process not finished, raise an abort signal
                        if (!hasFinished)
                            dla_2d.RaiseAbortSignal();
                        dla_2d.Clear();
                        break;
                    case LatticeDimension._3D:
                        // if generation process not finished, raise an abort signal
                        if (!hasFinished)
                            dla_3d.RaiseAbortSignal();
                        dla_3d.Clear();
                        break;
                }
            }
            // clear aggregate from user interface
            aggregate_manager.ClearAggregate();
            current_particles = 0;
            DynamicParticleLabel.Content = "Particles: " + current_particles;
            colour_list.Clear();
        }

        /// <summary>
        /// Handler for reset_view_button click event. Resets the viewport to initial state.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ResetViewButtonHandler(object sender, RoutedEventArgs e) {
            // reset rotational view
            trackview.ResetView();
            // reset orthographic_camera properties
            switch (lattice_dimension) {
                case LatticeDimension._2D:
                    orthograghic_camera.Position = new Point3D(0, 0, 32);
                    orthograghic_camera.LookDirection = new Vector3D(0, 0, -32);
                    orthograghic_camera.Width = 256.0;
                    break;
                case LatticeDimension._3D:
                    orthograghic_camera.Position = new Point3D(16, 16, 16);
                    orthograghic_camera.LookDirection = new Vector3D(-8, -8, -8);
                    orthograghic_camera.Width = 128.0;
                    break;
            }
        }

    }
}
