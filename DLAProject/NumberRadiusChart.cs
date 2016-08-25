using System.ComponentModel;
using System.Windows.Media;
using LiveCharts;
using LiveCharts.Wpf;
using LiveCharts.Configurations;
using DLAClassLibrary;

namespace DLAProject {
    /// <summary>
    /// Measurement model used for charting the radius of an aggregate against its particle number.
    /// </summary>
    public class NumberRadiusMeasureModel {
        public uint ParticleNumber { get; set; }
        public double AggregateRadius { get; set; }
    }

    /// <summary>
    /// Represents a chart which plots the radius of a diffusion limited aggregate against 
    /// the number of particles in the system. Inherits from INotifyPropertyChanged in order
    /// to notify client when a chart property changed event has been fired.
    /// </summary>
    public class NumberRadiusChart : INotifyPropertyChanged {
        private string x_axis_title;
        private string y_axis_title;
        private uint x_axis_min;    // minimum value of x-axis
        private uint x_axis_max;    // maximum value of x-axis
        private double y_axis_min;
        private double y_axis_max;
        private uint x_axis_step;   // incremental step of x-axis
        private double y_axis_step;
        private int series_counter; // number of series plotted, zero-indexed

        /// <summary>
        /// Initialises a new instance of the NumberRadiusChart class.
        /// </summary>
        public NumberRadiusChart() {
            x_axis_title = "Number of Particles";
            y_axis_title = "Aggregate Radius";
            // create a mapper using NumberRadiusMeasureModel where
            // X co-ord is ParticleNumber and Y co-ord is Radius.
            CartesianMapper<NumberRadiusMeasureModel> mapper = Mappers.Xy<NumberRadiusMeasureModel>()
                 .X(model => model.ParticleNumber)
                 .Y(model => model.AggregateRadius);
            // save the mapper globally
            Charting.For<NumberRadiusMeasureModel>(mapper);
            SeriesCollection = new SeriesCollection();
            series_counter = -1;
            ResetAxisProperties();
        }

        public event PropertyChangedEventHandler PropertyChanged;
        /// <summary>
        /// Handle property changed event.
        /// </summary>
        /// <param name="propertyName">Name of property that was changed.</param>
        protected virtual void OnPropertyChanged(string propertyName = null) {
            if (PropertyChanged != null)
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public SeriesCollection SeriesCollection { get; set; }

        /// <summary>
        /// Title of chart x-axis, readonly.
        /// </summary>
        public string XAxisTitle {
            get { return x_axis_title; }
        }
        /// <summary>
        /// Title of chart y-axis, readonly.
        /// </summary>
        public string YAxisTitle {
            get { return y_axis_title; }
        }
        /// <summary>
        /// Incremental step of x-axis.
        /// </summary>
        public uint XAxisStep {
            get { return x_axis_step; }
            set { x_axis_step = value; OnPropertyChanged("XAxisStep"); }
        }

        /// <summary>
        /// Minimum value of x-axis.
        /// </summary>
        public uint XAxisMin {
            get { return x_axis_min; }
            set { x_axis_min = value; OnPropertyChanged("XAxisMin"); }
        }
        /// <summary>
        /// Maximum value of x-axis.
        /// </summary>
        public uint XAxisMax {
            get { return x_axis_max; }
            set { x_axis_max = value; OnPropertyChanged("XAxisMax"); }
        }
        public double YAxisStep {
            get { return y_axis_step; }
            set { y_axis_step = value; OnPropertyChanged("YAxisStep"); }
        }
        public double YAxisMin {
            get { return y_axis_min; }
            set { y_axis_min = value; OnPropertyChanged("YAxisMin"); }
        }
        public double YAxisMax {
            get { return y_axis_max; }
            set { y_axis_max = value; OnPropertyChanged("YAxisMax"); }
        }
        /// <summary>
        /// Resets the axes' minimum, maximum values and step-size.
        /// </summary>
        public void ResetAxisProperties() {
            XAxisMin = 0;
            XAxisMax = 2000;
            XAxisStep = 200;
            YAxisMin = 0;
            YAxisMax = 20;
            YAxisStep = 5;
        }
        public int SeriesCount() {
            return SeriesCollection.Count;
        }
        /// <summary>
        /// Adds a new data series to the chart, the new series is a LineSeries added to
        /// NRSeriesCollection with Title given by nparticles and coeff_stick params.
        /// </summary>
        /// <param name="nparticles">Number of particles in current aggregate generation.</param>
        /// <param name="coeff_stick">Coefficient of stickiness of aggregate structure.</param>
        public void AddDataSeries(uint nparticles, double coeff_stick, ManagedLatticeType lattice_type) {
            string title = nparticles + "/" + coeff_stick + "/";
            // set appropriate title based on aggregate lattice type
            switch (lattice_type) {
                case ManagedLatticeType.Square:
                    title += "S";
                    break;
                case ManagedLatticeType.Triangle:
                    title += "T";
                    break;
            }
            // reduce opacity of each previously plotted series
            foreach (LineSeries s in SeriesCollection) {
                s.Stroke.Opacity = 0.25;
            }
            // add new series to collection, with opacity at default 1.0 for focus
            SeriesCollection.Add(new LineSeries {
                Title = title,
                Values = new ChartValues<NumberRadiusMeasureModel>(),
                PointGeometrySize = 5,
                Fill = Brushes.Transparent,
            });
            ++series_counter;
        }
        /// <summary>
        /// Adds a data point to the chart with specified x,y values.
        /// </summary>
        /// <param name="_particles">Number of particles (x co-ordinate).</param>
        /// <param name="_radius">Radius of aggregate (y co-ordinate).</param>
        public void AddDataPoint(uint _particles, double _radius) {
            SeriesCollection[series_counter].Values.Add(new NumberRadiusMeasureModel {
                ParticleNumber = _particles,
                AggregateRadius = _radius
            });
            OnPropertyChanged("AxisMin");
        }
        /// <summary>
        /// Clears all data points from all series on the chart.
        /// </summary>
        public void ClearAllSeriesDataPoints() {
            SeriesCollection.Clear();
            series_counter = -1;
        }

    }
}
