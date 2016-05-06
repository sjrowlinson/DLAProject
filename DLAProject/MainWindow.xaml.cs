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
        private Dictionary<KeyValuePair<int, int>, uint> aggregate_map;
        // handles to ManagedDLAContainer objects
        private ManagedDLA2DContainer dla_2d;
        private ManagedDLA3DContainer dla_3d;

        public MainWindow() {
            InitializeComponent();

            // initalise aggregate containers
            aggregate_map = new Dictionary<KeyValuePair<int, int>, uint>();
            dla_2d = new ManagedDLA2DContainer();
            dla_3d = new ManagedDLA3DContainer();
        }

        private void GenerateAggregate() {
            // lock around aggregate generation
            lock (locker) {
                // generate the 2D aggregate of size given by particles_slider value
                Dispatcher.Invoke(new Action(() => { dla_2d.Generate((uint)particles_slider.Value); }));

                // TODO: add particles to "canvas" on GUI as they are generated
            }
        }

        private void GenerateButtonClick(object sender, RoutedEventArgs e) {
            // set the coefficient of stickiness of aggregate
            // to current value of stickiness_slider
            try {
                dla_2d.SetCoeffStick(stickiness_slider.Value);
            }
            catch (ArgumentException) {
                
            }
            // start asynchronous task calling GenerateAggregate method
            Task.Factory.StartNew(() => GenerateAggregate());
        }

    }
}
