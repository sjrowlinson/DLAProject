using System;
using System.ComponentModel;
using System.Windows.Media;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiveCharts;
using LiveCharts.Wpf;
using LiveCharts.Configurations;

namespace DLAProject {
    /// <summary>
    /// Measurement model used for charting the rate of aggregate build-up 
    /// against the time span of the simulation process.
    /// </summary>
    public class RateGenerationMeasureModel {
        public TimeSpan Time { get; set; }
        public uint Rate { get; set; }
    }
    /// <summary>
    /// Represents a chart which plots the rate of generation of a diffusion limited aggregate
    /// against the time span of the simulation. Inherits from INotifyPropertyChanged in order
    /// to notify client when a chart property changed event has been fired.
    /// </summary>
    public class GenerationRateChart : INotifyPropertyChanged {
        private long x_axis_min;    // min-value of x-axis
        private long x_axis_max;    // max-value of x-axis
        private long x_axis_step;   // incremental step of x-axis
        private int series_counter;
        private string x_axis_title;
        private string y_axis_title;
        /// <summary>
        /// Initialises a new instance of the GenerationRateChart class.
        /// </summary>
        public GenerationRateChart() {
            x_axis_title = "Time (s)";
            y_axis_title = "Rate of Generation (/s)";
            // create a mapper using RateGenerationMeasureModel where X co-ord is
            // time-span and Y co-ord is the rate of aggregate generation
            CartesianMapper<RateGenerationMeasureModel> mapper = Mappers.Xy<RateGenerationMeasureModel>()
                .X(model => model.Time.Seconds)
                .Y(model => model.Rate);
            // save mapper globally
            Charting.For<RateGenerationMeasureModel>(mapper);
            TimeSpanFormatter = value => new TimeSpan(value).ToString("mm:ss");
            SeriesCollection = new SeriesCollection();
            series_counter = -1;
            ResetXAxisProperties();
        }

        public Func<long, string> TimeSpanFormatter { get; set; }

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
        /// Title of chart y-axis readonly.
        /// </summary>
        public string YAxisTitle {
            get { return y_axis_title; }
        }
        /// <summary>
        /// Minimum value of x-axis.
        /// </summary>
        public long AxisMin {
            get { return x_axis_min; }
            set { x_axis_min = value; OnPropertyChanged("AxisMin"); }
        }
        /// <summary>
        /// Maximum value of x-axis.
        /// </summary>
        public long AxisMax {
            get { return x_axis_max; }
            set { x_axis_max = value; OnPropertyChanged("AxisMax"); }
        }
        /// <summary>
        /// Incremental step value of x-axis.
        /// </summary>
        public long AxisStep {
            get { return x_axis_step; }
            set { x_axis_step = value; OnPropertyChanged("AxisStep"); }
        }
        /// <summary>
        /// Resets the x-axis minimum, maximum and step-size values.
        /// </summary>
        public void ResetXAxisProperties() {
            AxisMin = TimeSpan.Zero.Ticks;
            AxisMax = TimeSpan.FromSeconds(60).Ticks;
            AxisStep = TimeSpan.FromSeconds(1).Ticks;
        }

        public int SeriesCount() {
            return SeriesCollection.Count;
        }
        /// <summary>
        /// Adds a new data series to the chart, the new series is a LineSeries added to SeriesCollection
        /// with Title given by the lattice type and stickiness coefficient of the aggregate passed as params.
        /// </summary>
        /// <param name="lattice_type">Type of lattice which aggregated is generated on.</param>
        /// <param name="coeff_stick">Coefficient of stickiness of aggregate structure.</param>
        public void AddDataSeries(string lattice_type, double coeff_stick) {
            SeriesCollection.Add(new LineSeries {
                Title = lattice_type + "/" + coeff_stick,
                Values = new ChartValues<RateGenerationMeasureModel>(),
                PointGeometrySize = 5,
                Fill = Brushes.Transparent
            });
            ++series_counter;
        }
        /// <summary>
        /// Adds a data point to the chart with specified x,y values.
        /// </summary>
        /// <param name="time">Time span representing time-point w.r.t. starting time of aggregate generation.</param>
        /// <param name="rate">Rate of generation of aggregate structure, in particles added per second.</param>
        public void AddDataPoint(TimeSpan time, uint rate) {
            SeriesCollection[series_counter].Values.Add(new RateGenerationMeasureModel {
                Time = time,
                Rate = rate
            });
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
