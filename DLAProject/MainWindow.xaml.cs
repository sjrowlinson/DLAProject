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
using LiveCharts;
using LiveCharts.Wpf;

namespace DLAProject {
    /// <summary>
    /// Enum representing dimensions of lattice structure.
    /// </summary>
    public enum LatticeDimension {
        _2D,
        _3D
    }
    public enum ChartType {
        NUMBERRADIUS,
        RATEGENERATION
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        #region Fields
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
        private bool isContinuous;
        private bool saveCurrentChartSeries;
        // handle to AggregateSystemManager used for updating simulation render
        private readonly AggregateSystemManager aggregate_manager;
        private TrackView trackview;
        private uint current_particles;
        private uint prev_max_chart_x_value;
        private List<Color> colour_list;
        private LatticeDimension lattice_dimension;
        private bool lattice_dimension_combo_handle = true;
        private LatticeDimension current_executing_dimension;
        private ManagedLatticeType lattice_type;
        private bool lattice_type_combo_handle = true;
        private ManagedAttractorType attractor_type;
        private bool attractor_type_combo_handle = true;
        private ChartType chart_type;
        private bool chart_type_combo_handle = true;
        //private readonly AggregateComponentManager comp_manager;
        private NumberRadiusChart nrchart;
        #endregion

        public MainWindow() {
            InitializeComponent();
            // initalise aggregate containers
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
            aggregate_manager = new AggregateSystemManager();
            // set initial flag values
            isPaused = false;
            hasFinished = true;
            isContinuous = false;
            saveCurrentChartSeries = true;
            current_particles = 0;
            prev_max_chart_x_value = 0;
            colour_list = new List<Color>();
            lattice_dimension = LatticeDimension._2D;
            current_executing_dimension = lattice_dimension;
            lattice_type = ManagedLatticeType.Square;
            attractor_type = ManagedAttractorType.Point;
            chart_type = ChartType.NUMBERRADIUS;
            WorldModels.Children.Add(aggregate_manager.AggregateSystemModel());
            nrchart = new NumberRadiusChart();
            NRChart.DataContext = nrchart;
            //comp_manager = new AggregateComponentManager();
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
        /// Handler for continuous_checkbox click event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnContinuousCheckboxClicked(object sender, RoutedEventArgs e) {
            if (!isContinuous) {
                isContinuous = true;
                particles_slider.IsEnabled = false;
                particle_val.IsEnabled = false;
            }
            else {
                isContinuous = false;
                particles_slider.IsEnabled = true;
                particle_val.IsEnabled = true;
            }
            if (!hasFinished) {
                switch (current_executing_dimension) {
                    case LatticeDimension._2D:
                        dla_2d.ChangeContinuousFlag(false);
                        break;
                    case LatticeDimension._3D:
                        dla_3d.ChangeContinuousFlag(false);
                        break;
                }
            }
        }

        #region ComboBoxHandlers

        /// <summary>
        /// Sets the value of the lattice_dimension field corresponding to 
        /// selected item in dimension_ComboBox.
        /// </summary>
        private void HandleLatticeDimensionComboBox() {
            // get the selected dimension type
            ComboBoxItem selected_dimension = (ComboBoxItem)(dimension_ComboBox.SelectedValue);
            string dimension_str = (string)(selected_dimension.Content);
            if (dimension_str == null) lattice_dimension = LatticeDimension._2D;
            else {
                ComboBoxItem planeItem = (ComboBoxItem)attractorType_ComboBox.FindName("PlaneItem");
                // set corresponding lattice_dimension
                switch (dimension_str) {
                    case "2D":
                        lattice_dimension = LatticeDimension._2D;
                        planeItem.IsEnabled = false;
                        break;
                    case "3D":
                        lattice_dimension = LatticeDimension._3D;
                        planeItem.IsEnabled = true;
                        break;
                }
            }
        }

        /// <summary>
        /// Handles dimension_ComboBox drop down closed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLatticeDimensionDropDownClosed(object sender, EventArgs e) {
            if (lattice_dimension_combo_handle)
                HandleLatticeDimensionComboBox();
            lattice_dimension_combo_handle = true;
        }

        /// <summary>
        /// Handles dimension_ComboBox selected item changed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLatticeDimensionSelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cb = sender as ComboBox;
            lattice_dimension_combo_handle = !cb.IsDropDownOpen;
            HandleLatticeDimensionComboBox();
        }

        /// <summary>
        /// Sets the value of the lattice_type field corresponding to
        /// selected item in latticeType_ComboBox.
        /// </summary>
        private void HandleLatticeTypeComboBox() {
            // get selected lattice type
            ComboBoxItem selected_lattice_type = (ComboBoxItem)(latticeType_ComboBox.SelectedValue);
            string lattice_type_str = (string)(selected_lattice_type.Content);
            // set corresponding lattice_type 
            if (lattice_type_str == null) lattice_type = ManagedLatticeType.Square;
            else lattice_type = (ManagedLatticeType)Enum.Parse(typeof(ManagedLatticeType), lattice_type_str);
        }

        /// <summary>
        /// Handles latticeType_ComboBox drop down closed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLatticeTypeDropDownClosed(object sender, EventArgs e) {
            if (lattice_type_combo_handle)
                HandleLatticeTypeComboBox();
            lattice_type_combo_handle = true;
        }

        /// <summary>
        /// Handles latticeType_ComboBox selected item changed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLatticeTypeSelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cb = sender as ComboBox;
            lattice_type_combo_handle = !cb.IsDropDownOpen;
            HandleLatticeTypeComboBox();
        }

        /// <summary>
        /// Sets the value of the attractor_type field corresponding to
        /// selected item in attractorType_ComboBox.
        /// </summary>
        private void HandleAttractorTypeComboBox() {
            // get selected attractor type
            ComboBoxItem selected_attractor_type = (ComboBoxItem)(attractorType_ComboBox.SelectedValue);
            string attractor_type_str = (string)(selected_attractor_type.Content);
            // set corresponding attractor_type
            if (attractor_type_str == null) attractor_type = ManagedAttractorType.Point;
            else attractor_type = (ManagedAttractorType)Enum.Parse(typeof(ManagedAttractorType), attractor_type_str);
        }

        /// <summary>
        /// Handles attractorType_ComboBox drop down closed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnAttractorTypeDropDownClosed(object sender, EventArgs e) {
            if (attractor_type_combo_handle)
                HandleAttractorTypeComboBox();
            attractor_type_combo_handle = true;
        }

        /// <summary>
        /// Handles attractorType_ComboBox selected item changed event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnAttractorTypeSelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cb = sender as ComboBox;
            attractor_type_combo_handle = !cb.IsDropDownOpen;
            HandleAttractorTypeComboBox();
        }

        private void HandleChartSelectorComboBox() {
            ComboBoxItem selected_chart = (ComboBoxItem)(chartSelectorComboBox.SelectedValue);
            string selected_chart_str = (string)(selected_chart.Content);
            if (selected_chart_str == null) chart_type = ChartType.NUMBERRADIUS;
            else {
                switch (selected_chart_str) {
                    case "Number-Radius":
                        chart_type = ChartType.NUMBERRADIUS;
                        break;
                    case "Generation Rate":
                        chart_type = ChartType.RATEGENERATION;
                        break;
                }
            }
        }

        private void OnChartSelectorDropDownClosed(object sender, EventArgs e) {
            if (chart_type_combo_handle)
                HandleChartSelectorComboBox();
            chart_type_combo_handle = true;
        }

        private void OnChartSelectorSelectionChanged(object sender, SelectionChangedEventArgs e) {
            ComboBox cb = sender as ComboBox;
            chart_type_combo_handle = !cb.IsDropDownOpen;
            HandleChartSelectorComboBox();
        }

        #endregion

        #region AggregatePropertySetting

        /// <summary>
        /// Fills the colour_list field with colour instances for 
        /// each particle to be generated in an aggregate. This 
        /// method uses a progressive temperature gradient from
        /// cold to hot for each subsequent particle in the aggregate. 
        /// </summary>
        /// <param name="_total_particles">Total number of particles to be generated.</param>
        private void ComputeColorList(uint _total_particles) {
            for (uint i = 1; i <= _total_particles; ++i) {
                Color colour = new Color();
                colour.ScA = 1;
                colour.ScR = (float)i / _total_particles;
                colour.ScB = 1 - (float)i / _total_particles;
                if (i < _total_particles / 2) {
                    colour.ScG = 2*(float)i / _total_particles;
                }
                else {
                    colour.ScG = 2*(1 - (float)i / _total_particles);
                }
                colour_list.Add(colour);
            }
        }

        /// <summary>
        /// Sets up all the properties of the aggregate necessary for generation, including 
        /// lattice type, attractor type and the dimensions of the aggregate.
        /// </summary>
        private void SetUpAggregateProperties(uint _nparticles, double _agg_sticky_coeff) {
            // reset simulation view
            ResetViewButtonHandler(null, null);
            // pre-compute the color list for all particles
            if (!isContinuous) ComputeColorList(_nparticles);
            else ComputeColorList(50000);   // TODO: change this, don't like "magic" number
            if(saveCurrentChartSeries) nrchart.AddDataSeries(_nparticles, _agg_sticky_coeff);
            NRChart.LegendLocation = LegendLocation.Right;
            nrchart.AddDataPoint(0, 0.0);
            // set properties of aggregate corresponding to dimension
            switch (current_executing_dimension) {
                case LatticeDimension._2D:
                    dla_2d.SetCoeffStick(_agg_sticky_coeff);
                    dla_2d.SetLatticeType(lattice_type);
                    dla_2d.SetAttractorType(attractor_type);
                    dla_2d.ChangeContinuousFlag(isContinuous);
                    break;
                case LatticeDimension._3D:
                    dla_3d.SetCoeffStick(_agg_sticky_coeff);
                    dla_3d.SetLatticeType(lattice_type);
                    dla_3d.SetAttractorType(attractor_type);
                    dla_3d.ChangeContinuousFlag(isContinuous);
                    break;
            }
        }

        #endregion

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
            switch (current_executing_dimension) {
                case LatticeDimension._2D:
                    if (dla_2d.Size() < _particle_slider_val || isContinuous) {
                        timer.Elapsed += (source, e) => Aggregate2DUpdateOnTimedEvent(source, e, _particle_slider_val);
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
                    if (dla_3d.Size() < _particle_slider_val || isContinuous) {
                        timer.Elapsed += (source, e) => Aggregate3DUpdateOnTimedEvent(source, e, _particle_slider_val);
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

        private void Aggregate2DUpdateOnTimedEventTest(object source, ElapsedEventArgs e) {
            lock(locker) {
                BlockingCollection<KeyValuePair<int, int>> blocking_queue = dla_2d.ProcessBatchQueue();
                while (blocking_queue.Count != 0) {
                    KeyValuePair<int, int> agg_kvp = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_kvp.Key, agg_kvp.Value, 0.0);
                    //comp_manager.AddParticleToComponent(pos, 1.0);
                    Dispatcher.Invoke(() => {
                        //comp_manager.Update();
                        DynamicParticleLabel.Content = "Particles: " + current_particles;
                        FracDimLabel.Content = "Est. Fractal Dimension: " + Math.Round(dla_2d.EstimateFractalDimension(), 3);
                        AggMissesLabel.Content = "Aggregate Misses: " + dla_2d.GetAggregateMisses();
                    });
                    ++current_particles;
                }
            }
        }

        private void Aggregate3DUpdateOnTimedEventTest(object source, ElapsedEventArgs e) {
            lock(locker) {
                BlockingCollection<Tuple<int, int, int>> blocking_queue = dla_3d.ProcessBatchQueue();
                while (blocking_queue.Count != 0) {
                    Tuple<int, int, int> agg_tuple = blocking_queue.Take();
                    Point3D pos = new Point3D(agg_tuple.Item1, agg_tuple.Item2, agg_tuple.Item3);
                    //comp_manager.AddParticleToComponent(pos, 1.0);
                    Dispatcher.Invoke(() => {
                        //comp_manager.Update();
                        DynamicParticleLabel.Content = "Particles: " + current_particles;
                        FracDimLabel.Content = "Est. Fractal Dimension: " + Math.Round(dla_3d.EstimateFractalDimension(), 3);
                        AggMissesLabel.Content = "Aggregate Misses: " + dla_3d.GetAggregateMisses();
                    });
                    ++current_particles;
                }
            }
        }

        /// <summary>
        /// Updates a 2D aggregate based on current contents of dla_2d batch_queue - processes this
        /// batch_queue and adds its contents to the simulation view.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void Aggregate2DUpdateOnTimedEvent(object source, ElapsedEventArgs e, uint total_particles) {
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
                    ++current_particles;
                    // dispatch GUI updates to UI thread
                    Dispatcher.Invoke(() => {
                        aggregate_manager.Update();
                        DynamicParticleLabel.Content = "Particles: " + current_particles;
                        FracDimLabel.Content = "Est. Fractal Dimension: " + Math.Round(dla_2d.EstimateFractalDimension(), 3);
                        AggMissesLabel.Content = "Aggregate Misses: " + dla_2d.GetAggregateMisses();
                        if (current_particles % 100 == 0)
                            nrchart.AddDataPoint(current_particles, Math.Sqrt(dla_2d.GetAggregateRadiusSquared()));
                        if (current_particles % 2000 == 0 && current_particles >= prev_max_chart_x_value && current_particles != total_particles) {
                            nrchart.AxisStep += 100;
                            nrchart.AxisMax += 2000;
                        }
                    });
                }
            }
        }
        
        /// <summary>
        /// Updates a 3D aggregate based on current contents of dla_3d batch_queue - processes this
        /// batch_queue and adds its contents to the simulation view.
        /// </summary>
        /// <param name="source"></param>
        /// <param name="e"></param>
        private void Aggregate3DUpdateOnTimedEvent(object source, ElapsedEventArgs e, uint total_particles) {
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
                        FracDimLabel.Content = "Est. Fractal Dimension: " + Math.Round(dla_3d.EstimateFractalDimension(), 3);
                        AggMissesLabel.Content = "Aggregate Misses: " + dla_3d.GetAggregateMisses();
                        if (current_particles % 100 == 0)
                            nrchart.AddDataPoint(current_particles, Math.Sqrt(dla_3d.GetAggregateRadiusSquared()));
                        if (current_particles % 2000 == 0 && current_particles > prev_max_chart_x_value && current_particles != total_particles) {
                            nrchart.AxisStep += 100;
                            nrchart.AxisMax += 2000;
                        }
                    });
                }
            }
        }

        /// <summary>
        /// Generates a Diffusion Limited Aggregate with properties initialised by
        /// current values of sliders and combo-boxes in the UI. Should be called
        /// in a separate thread.
        /// </summary>
        private void GenerateAggregate(uint _nparticles) {
            hasFinished = false;
            // start asynchronous task calling AggregateUpdateListener to perform rendering
            Task.Factory.StartNew(() => AggregateUpdateListener(_nparticles));
            // generate the DLA using value of particle slider
            switch (current_executing_dimension) {
                case LatticeDimension._2D:
                    dla_2d.Generate(_nparticles);
                    break;
                case LatticeDimension._3D:
                    dla_3d.Generate(_nparticles);
                    break;
            }
            hasFinished = true;
            prev_max_chart_x_value = nrchart.AxisMax;
            saveCurrentChartSeries = false;
            Dispatcher.Invoke(() => { compare_button.IsEnabled = true; });
        }
        
        #region ButtonHandlers

        /// <summary>
        /// Handler for generate_button click event. Calls GenerateAggregate() in a separate task factory.
        /// </summary>
        /// <param name="sender">Sender identification</param>
        /// <param name="e">Variable containing state information associated with event</param>
        private void GenerateButtonHandler(object sender, RoutedEventArgs e) {
            // clear any existing aggregate
            if (current_particles > 0)
                ClearButtonHandler(null, null);
            current_executing_dimension = lattice_dimension;
            uint nparticles = (uint)particles_slider.Value;
            double agg_sticky_coeff = stickiness_slider.Value;
            // (re)-initialise aggregate properties
            SetUpAggregateProperties(nparticles, agg_sticky_coeff);
            //for (int i = 0; i < (int)particles_slider.Value; ++i) {
            //    WorldModels.Children.Add(comp_manager.CreateAggregateComponent(colour_list[i]));
            //}
            // start asynchronous task calling GenerateAggregate method
            Task.Factory.StartNew(() => GenerateAggregate(isContinuous ? 0 : nparticles));
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
                switch (current_executing_dimension) {
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
            if (!saveCurrentChartSeries) {
                nrchart.ResetXAxisProperties();
                nrchart.ClearAllSeriesDataPoints();
                saveCurrentChartSeries = true;
                prev_max_chart_x_value = 0;
            }
            //WorldModels.Children.Clear();
            //WorldModels.Children.Add(new AmbientLight(Colors.White));
            //comp_manager.Clear();
            current_particles = 0;
            DynamicParticleLabel.Content = "Particles: " + current_particles;
            FracDimLabel.Content = "Est. Fractal Dimension: " + 0.0;
            AggMissesLabel.Content = "Aggregate Misses: " + 0;
            colour_list.Clear();
            compare_button.IsEnabled = false;
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
            switch (current_executing_dimension) {
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

        private void OnCompareButtonClick(object sender, RoutedEventArgs e) {
            saveCurrentChartSeries = true;
            compare_button.IsEnabled = false;
        }

        #endregion

    }
}
