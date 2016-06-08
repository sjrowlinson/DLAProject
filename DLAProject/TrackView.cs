using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Media3D;

namespace DLAProject {

    /// <summary>
    /// Tracking class for moving the view of an aggregate around.
    /// </summary>
    public class TrackView {

        // state of trackview
        private bool isEnabled;
        // is rotation event occurring
        private bool isRotating;
        // initial point of drag
        private Point drag_point;
        private Quaternion rotation;
        // change to rotation because of drag
        private Quaternion rotation_delta;
        // is scaling event occurring
        private bool isScaling;
        private double scale;
        // change to scale because of drag
        private double scale_delta;
        private Viewport3D viewport;

        /// <summary>
        /// Initialises a new instance of the TrackView class.
        /// </summary>
        public TrackView() {
            InitialiseOrReset();
            UpdateViewport(rotation, scale);
        }

        /// <summary>
        /// Gets or sets the isEnabled flag associated with the TrackView instance.
        /// </summary>
        public bool Enabled
        {
            get { return isEnabled && (viewport != null); }
            set { isEnabled = value; }
        }

        /// <summary>
        /// Gets or sets the Viewport3D associated with the TrackView instance.
        /// </summary>
        public Viewport3D Viewport
        {
            get { return viewport; }
            set { viewport = value; }
        }

        /// <summary>
        /// Resets the viewport to initial state.
        /// </summary>
        public void ResetView() {
            InitialiseOrReset();
            UpdateViewport(rotation, scale);
        }

        /// <summary>
        /// Initialises, or resets, properties to default values.
        /// </summary>
        private void InitialiseOrReset() {
            rotation = new Quaternion(0.0, 0.0, 0.0, 1.0);
            rotation_delta = Quaternion.Identity;
            scale = 1.0;
            scale_delta = 1.0;
        }

        /// <summary>
        /// Updates the viewport of the TrackView using a given Quaternion argument for rotation.
        /// </summary>
        /// <param name="_qtn">Quaternion instance defining the rotation properties.</param>
        private void UpdateViewport(Quaternion _qtn, double _scale) {
            // check for null viewport
            if (viewport != null) {
                // get the ModelVisual3D instance of the viewport
                ModelVisual3D mv3d = viewport.Children[0] as ModelVisual3D;
                // get the Transform3DGroup associated with mv3d
                Transform3DGroup t3dg = mv3d.Transform as Transform3DGroup;
                // get the RotateTransform3D associated with t3dg
                RotateTransform3D rotate_transform = t3dg.Children[0] as RotateTransform3D;
                rotate_transform.Rotation = new AxisAngleRotation3D(_qtn.Axis, _qtn.Angle);
                ScaleTransform3D scale_transform = t3dg.Children[1] as ScaleTransform3D;
                scale_transform.ScaleX = _scale;
                scale_transform.ScaleY = _scale;
                scale_transform.ScaleZ = _scale;
            }
        }

        /// <summary>
        /// Attach a FrameworkElement to mouse event handlers for this trackview.
        /// </summary>
        /// <param name="_element"></param>
        public void Attach(FrameworkElement _element) {
            _element.MouseMove += MouseMoveHandler;
            _element.MouseRightButtonDown += MouseDownHandler;
            _element.MouseRightButtonUp += MouseUpHandler;
            _element.MouseWheel += MouseWheelHandler;
        }

        /// <summary>
        /// Detach a FrameworkElement from mouse event handlers for this trackview.
        /// </summary>
        /// <param name="_element"></param>
        public void Detach(FrameworkElement _element) {
            _element.MouseMove -= MouseMoveHandler;
            _element.MouseRightButtonDown -= MouseDownHandler;
            _element.MouseRightButtonUp -= MouseUpHandler;
            _element.MouseWheel -= MouseWheelHandler;
        }

        /// <summary>
        /// Handle right-click down event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MouseDownHandler(object sender, MouseButtonEventArgs e) {
            if (!Enabled)
                return;
            e.Handled = true;
            // get position of initial drag point relative to sender element
            var elem = (UIElement)sender;
            drag_point = e.MouseDevice.GetPosition(elem);
            isScaling = (e.MiddleButton == MouseButtonState.Pressed);
            isRotating = true;
            elem.CaptureMouse();
        }

        /// <summary>
        /// Handle right-click released event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MouseUpHandler(object sender, MouseButtonEventArgs e) {
            if (!Enabled)
                return;
            e.Handled = true;
            // put initial-delta product into initial such that the next move occurs
            // from correct position
            if (isRotating)
                rotation *= rotation_delta;
            else {

            }
            var elem = (UIElement)sender;
            elem.ReleaseMouseCapture();
        }

        /// <summary>
        /// Handle mouse-movement event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MouseMoveHandler(object sender, MouseEventArgs e) {
            if (!Enabled)
                return;
            e.Handled = true;

            var elem = (UIElement)sender;
            if (elem.IsMouseCaptured) {
                // get displacement between initial drag point and position of mouse relative to sender
                Vector delta = drag_point - e.MouseDevice.GetPosition(elem);
                delta /= 2;
                // assign local Quaternion variable to rotation field
                Quaternion qtn = rotation;
                if (isRotating) {
                    // map 2D mouse delta to 3D mouse delta with vector
                    // into the display as Z component
                    Vector3D mouse_vec = new Vector3D(delta.X, -delta.Y, 0.0);
                    // compute rotation axis from cross product of mouse_vec with k vector
                    Vector3D rot_axis = Vector3D.CrossProduct(mouse_vec, new Vector3D(0.0, 0.0, 1.0));
                    double axis_length = rot_axis.Length;
                    const double epsilon = 0.00001;
                    // if length of axis is less than some small factor, set rotation_delta to
                    // a Quaternion with k vector axis of rotation and 0.0 angle 
                    if (axis_length < epsilon || isScaling) {
                        rotation_delta = new Quaternion(new Vector3D(0.0, 0.0, 1.0), 0.0);
                    }
                    // else set rotation_delta to a Quaternion with computed rotation axis
                    // and axis length from above
                    else {
                        rotation_delta = new Quaternion(rot_axis, axis_length);
                    }
                    // multiply qtn by rotation_delta and assign result to qtn
                    qtn *= rotation_delta;
                }
                else {
                    //TODO: figure out code for !isRotating mouse movement handling
                }
                // update the viewport
                UpdateViewport(qtn, scale*scale_delta);
            }
        }

        /// <summary>
        /// Handles mouse wheel event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MouseWheelHandler(object sender, MouseWheelEventArgs e) {
            e.Handled = true;
            // set scale_delta to Delta of mouse wheel over 1000 for smaller changes
            scale_delta += e.Delta / (double)1000;
            // update the viewport to new zoom level
            UpdateViewport(rotation, scale * scale_delta);
        }

    }
}
