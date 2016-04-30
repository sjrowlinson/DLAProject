using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DLAProject {

    public enum LatticeType {
        SQUARE,
        TRIANGLE,
    }

    public enum AttractorType {
        POINT,
        LINE,
        PLANE,
    }

    public abstract class DLAContainer {

        protected LatticeType latticeType;
        protected AttractorType attractorType;
        protected double stickyCoeff;
        static protected Random rng = new Random();

        public DLAContainer(double _stickyCoeff = 1.0) {
            SetStickyCoefficient(_stickyCoeff);
        }

        public DLAContainer(LatticeType _latticeType, AttractorType _attractorType, double _stickyCoeff = 1.0) {
            SetStickyCoefficient(_stickyCoeff);
            latticeType = _latticeType;
            attractorType = _attractorType;
        }

        public DLAContainer(DLAContainer _other) {
            latticeType = _other.latticeType;
            attractorType = _other.attractorType;
            stickyCoeff = _other.stickyCoeff;
        }

        public abstract uint Size();

        public abstract void Clear();

        public abstract double EstimateFractalDimension();

        public abstract void generate(uint _n);

        public void SetStickyCoefficient(double _stickyCoeff) {
            if (_stickyCoeff <= 0.0 || _stickyCoeff > 1.0)
                throw new ArgumentException("_stickyCoeff must be in (0,1]");
            stickyCoeff = _stickyCoeff;
        }

        /**
         * <summary>Updates the position of a particle on a 2d lattice via unbiased random walk motion.</summary>
         * 
         * <param name="_x">Position in x co-ord</param>
         * <param name="_y">Position in y co-ord</param>
         * <param name="_movement_choice">Probability determining direction of movement</param>
         */
        protected void update_particle_position(ref int _x, ref int _y, double _movement_choice) {
            switch (latticeType) {
                case LatticeType.SQUARE:
                    if (_movement_choice < 0.25) {
                        ++_x;
                    }
                    else if (_movement_choice >= 0.25 && _movement_choice < 0.5) {
                        --_x;
                    }
                    else if (_movement_choice >= 0.5 && _movement_choice < 0.75) {
                        ++_y;
                    }
                    else if (_movement_choice >= 0.75 && _movement_choice < 1.0) {
                        --_y;
                    }
                    break;
                case LatticeType.TRIANGLE:
                    if (_movement_choice < 1.0 / 6.0) {
                        ++_x;
                    }
                    else if (_movement_choice >= 1.0 / 6.0 && _movement_choice < 2.0 / 6.0) {
                        --_x;
                    }
                    else if (_movement_choice >= 2.0 / 6.0 && _movement_choice < 3.0 / 6.0) {
                        ++_x;
                        ++_y;
                    }
                    else if (_movement_choice >= 3.0 / 6.0 && _movement_choice < 4.0 / 6.0) {
                        ++_x;
                        --_y;
                    }
                    else if (_movement_choice >= 4.0 / 6.0 && _movement_choice < 5.0 / 6.0) {
                        --_x;
                        ++_y;
                    }
                    else if (_movement_choice >= 5.0 / 6.0 && _movement_choice < 1.0) {
                        --_x;
                        --_y;
                    }
                    break;
                    // TODO: add extra cases for differemt LatticeType constants
            }
        }

        /**
         * <summary>Updates the position of a particle on a 3d lattice via unbiased random walk motion.</summary>
         * 
         * <param name="_x">Position in x co-ord</param>
         * <param name="_y">Position in y co-ord</param>
         * <param name="_z">Position in z co-ord</param>
         * <param name="_movement_choice">Probability determining direction of movement</param>
         */
        protected void update_particle_position(ref int _x, ref int _y, ref int _z, double _movement_choice) {
            switch (latticeType) {
                case LatticeType.SQUARE:
                    if (_movement_choice < 1.0 / 6.0) {
                        ++_x;
                    }
                    else if (_movement_choice >= 1.0 / 6.0 && _movement_choice < 2.0 / 6.0) {
                        --_x;
                    }
                    else if (_movement_choice >= 2.0 / 6.0 && _movement_choice < 3.0 / 6.0) {
                        ++_y;
                    }
                    else if (_movement_choice >= 3.0 / 6.0 && _movement_choice < 4.0 / 6.0) {
                        --_y;
                    }
                    else if (_movement_choice >= 4.0 / 6.0 && _movement_choice < 5.0 / 6.0) {
                        ++_z;
                    }
                    else if (_movement_choice >= 5.0 / 6.0 && _movement_choice < 1.0) {
                        --_z;
                    }
                    break;
                case LatticeType.TRIANGLE:
                    if (_movement_choice < 1.0 / 8.0) {
                        ++_x;
                    }
                    else if (_movement_choice >= 1.0 / 8.0 && _movement_choice < 2.0 / 8.0) {
                        --_x;
                    }
                    else if (_movement_choice >= 2.0 / 8.0 && _movement_choice < 3.0 / 8.0) {
                        ++_x;
                        ++_y;
                    }
                    else if (_movement_choice >= 3.0 / 8.0 && _movement_choice < 4.0 / 8.0) {
                        ++_x;
                        --_y;
                    }
                    else if (_movement_choice >= 4.0 / 8.0 && _movement_choice < 5.0 / 8.0) {
                        --_x;
                        ++_y;
                    }
                    else if (_movement_choice >= 5.0 / 8.0 && _movement_choice < 6.0 / 8.0) {
                        --_x;
                        --_y;
                    }
                    else if (_movement_choice >= 6.0 / 8.0 && _movement_choice < 7.0 / 8.0) {
                        ++_z;
                    }
                    else if (_movement_choice >= 7.0 / 8.0 && _movement_choice < 1.0) {
                        --_z;
                    }
                    break;
                    // TODO: add extra cases for differemt LatticeType constants
            }
        }

        /**
         * <summary>Check for collision of a particle with a 2d lattice boundary and reflect if true.</summary>
         */
        protected bool lattice_boundary_collision(ref int _x, ref int _y, int _prev_x, int _prev_y, int _spawn_diameter) {
            switch(attractorType) {
                case AttractorType.POINT:
                    if (Math.Abs(_x) > (_spawn_diameter/2 + 2) || Math.Abs(_y) > (_spawn_diameter/2 + 2)) {
                        _x = _prev_x;
                        _y = _prev_y;
                        return true;
                    }
                    break;
            }
            // TODO:: add extra cases for different attractor types
            return false;
        }

        /**
         * <summary>Check for collision of a particle with a 3d lattice boundary and reflect if true.</summary>
         * 
         * <param name="_x"></param>
         * <param name="_y"></param>
         * <param name="_z"></param>
         * <param name="_prev_x"></param>
         * <param name="_prev_y"></param>
         * <param name="_prev_z"></param>
         * <param name="_spawn_diameter"></param>
         */
        protected bool lattice_boundary_collision(ref int _x, ref int _y, ref int _z, int _prev_x, int _prev_y, int _prev_z, int _spawn_diameter) {
            switch (attractorType) {
                case AttractorType.POINT:
                    if (Math.Abs(_x) > (_spawn_diameter / 2 + 2) || Math.Abs(_z) > (_spawn_diameter / 2 + 2) || Math.Abs(_z) > (_spawn_diameter / 2 + 2)) {
                        _x = _prev_x;
                        _y = _prev_y;
                        _z = _prev_z;
                        return true;
                    }
                    break;
            }
            // TODO: add extra cases for different attractor types
            return false;
        }

        protected abstract bool aggregate_collision(ref int _x, ref int _y, int _prev_x, int _prev_y, double _sticky_pr, uint _count);
        protected abstract bool aggregate_collision(ref int _x, ref int _y, ref int _z, int _prev_x, int _prev_y, int _prev_z, double _sticky_pr, uint _count);

    }
}
