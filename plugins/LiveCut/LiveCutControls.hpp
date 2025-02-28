/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 Adapted for DISTRHO Plugin Framework (DPF) by eventual-recluse.
 
 Livecut can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 Livecut is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Livecut; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 Boston, MA 02111-1307 USA
 */

/* Control information required by LiveCut.cpp and LiveCutUI.cpp */

#ifndef LIVECUT_CONTROLS_HPP
#define LIVECUT_CONTROLS_HPP

#include <array>

template <class T>
T limit (const T x, const T min, const T max)
{
    return (x < min ? min : (x > max ? max : x));
}

enum LiveCutControls{
    LVC_CUTPROC       = 0,
    LVC_SUBDIV        = 1,
    LVC_FADE          = 2,
    LVC_MINAMP        = 3,
    LVC_MAXAMP        = 4,
    LVC_MINPAN        = 5,
    LVC_MAXPAN        = 6,
    LVC_MINPITCH      = 7,
    LVC_MAXPITCH      = 8,
    LVC_DUTY          = 9,
    LVC_FILLDUTY      = 10,
    LVC_MAXPHRS       = 11,
    LVC_MINPHRS       = 12,
    LVC_MAXREP        = 13,
    LVC_MINREP        = 14,
    LVC_STUTTER       = 15,
    LVC_AREA          = 16,
    LVC_STRAIGHT      = 17,
    LVC_REGULAR       = 18,
    LVC_RITARD        = 19,
    LVC_SPEED         = 20,
    LVC_ACTIVITY      = 21,
    LVC_CRUSHER       = 22,
    LVC_MINBITS       = 23,
    LVC_MAXBITS       = 24,
    LVC_MINFREQ       = 25,
    LVC_MAXFREQ       = 26,
    LVC_COMB          = 27,
    LVC_TYPE          = 28,
    LVC_FEEDBACK      = 29,
    LVC_MINDELAY      = 30,
    LVC_MAXDELAY      = 31,
    LVC_SEED          = 32,
    LVC_CONTROL_NR    = 33
};

static const std::array<std::pair<float, float>, LVC_CONTROL_NR> controlLimits =
{{
    {0.0f, 2.0f},        // LVC_CUTPROC
    {0.0f, 6.0f},        // LVC_SUBDIV
    {0.0f, 100.0f},      // LVC_FADE
    {0.0f, 1.0f},        // LVC_MINAMP
    {0.0f, 1.0f},        // LVC_MAXAMP
    {-1.0f, 1.0f},       // LVC_MINPAN
    {-1.0f, 1.0f},       // LVC_MAXPAN
    {-2400.0f, 2400.0f}, // LVC_MINPITCH
    {-2400.0f, 2400.0f}, // LVC_MAXPITCH
    {0.0f, 1.0f},        // LVC_DUTY
    {0.0f, 1.0f},        // LVC_FILLDUTY
    {1.0f, 8.0f},        // LVC_MAXPHRS
    {1.0f, 8.0f},        // LVC_MINPHRS
    {0.0f, 4.0f},        // LVC_MAXREP
    {0.0f, 4.0f},        // LVC_MINREP
    {0.0f, 1.0f},        // LVC_STUTTER
    {0.0f, 1.0f},        // LVC_AREA
    {0.0f, 1.0f},        // LVC_STRAIGHT
    {0.0f, 1.0f},        // LVC_REGULAR
    {0.0f, 1.0f},        // LVC_RITARD
    {0.5f, 0.999f},      // LVC_SPEED
    {0.0f, 1.0f},        // LVC_ACTIVITY
    {0.0f, 1.0f},        // LVC_CRUSHER
    {1.0f, 32.0f},       // LVC_MINBITS
    {1.0f, 32.0f},       // LVC_MAXBITS
    {441.0f, 44100.0f},  // LVC_MINFREQ
    {441.0f, 44100.0f},  // LVC_MAXFREQ
    {0.0f, 1.0f},        // LVC_COMB
    {0.0f, 1.0f},        // LVC_TYPE
    {0.0f, 0.6f},     // LVC_FEEDBACK
    {0.05f, 10.0f},       // LVC_MINDELAY
    {0.05f, 10.0f},       // LVC_MAXDELAY
    {1.0f, 16.0f}        // LVC_SEED
}};

static const float LVC_DEFAULTS[LVC_CONTROL_NR] = {
    0.0f, // LVC_CUTPROC
    1.0f, // LVC_SUBDIV
    0.01f, // LVC_FADE
    1.0f, // LVC_MINAMP
    1.0f, // LVC_MAXAMP
    -0.2f, // LVC_MINPAN
    0.2f,  // LVC_MAXPAN
    0.0f, // LVC_MINPITCH
    0.0f, // LVC_MAXPITCH
    1.0f, // LVC_DUTY
    1.0f, // LVC_FILLDUTY
    4.0f, // LVC_MAXPHRS
    1.0f, // LVC_MINPHRS
    1.0f, // LVC_MAXREP
    0.0f, // LVC_MINREP
    0.8f, // LVC_STUTTER
    0.5f, // LVC_AREA
    0.3f, // LVC_STRAIGHT
    0.5f, // LVC_REGULAR
    0.5f, // LVC_RITARD
    0.9f, // LVC_SPEED
    0.5f, // LVC_ACTIVITY
    0.0f, // LVC_CRUSHER
    32.0f, // LVC_MINBITS
    32.0f, // LVC_MAXBITS
    44100.0f, // LVC_MINFREQ
    44100.0f, // LVC_MAXFREQ
    0.0f, // LVC_COMB
    0.0f, // LVC_TYPE
    0.5f, // LVC_FEEDBACK
    1.0f, // LVC_MINDELAY
    10.0f, //  LVC_MAXDELAY
    1.0f //  LVC_SEED
};

static const long LVC_SUBDIV_OPTIONS[7] = {
    6, 8, 12, 16, 18, 24, 32
};

#endif
