// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "aggreg.h"
#include "dbg_report.h"
#include "animation.h"
#include "colors.h"
#include "fractal.h"
#include "light.h"
#include "demo_func.h"
#include <sstream>
#include <string>
#include <string_view>
#include <sys/types.h>

// Main program aggregat (collection of) structs
// containing MovAnim(animation), Color Palette,
// and Light Source

 
// Post Construction (Initialization) Init, sync
void MainProgAggr::postInitSync() {
  // Synchronize flash color pallete with (just initialized) current light color
  colorPal.calc_flash_color_pallet(LightS::s_lightColor);
}

// One step per display loop - to be disapthed to subordinate classes/struct
void MainProgAggr::one_step_cfg_change() {
  movWind.one_step_cfg_change();
  colorPal.one_step_flash_reset();  
  lightS.one_step_light_resume();
}

// current transformation data (array) and Colors
std::string MainProgAggr::prepareSnapshotData(void) {
  std::stringstream ss;

  // Takes snpashot data from base classes
  ss << colorPal.log_rnd_color_pallet() << '\n';
  ss << movWind.log_trans_config() << '\n';
  return ss.str();
}
  
void MainProgAggr::resetConfig(bool keyAction) {
  // Stop both types of animation
  movWind.stopAnimation(); // close/open animation
  movWind.stop_wind();
  movWind.stopAtZero = false;
  // if done by keyboard
  if (keyAction) { 
    // Set to one of pre-calculated configuration
    movWind.reset_pre_cfg(); // algo_data = default
    // Reset (current) color to nullify dimm efect 
    colorPal.reset_cur_color_pallet();
    // Reset light position
    lightS.reset_light();
    }
  // Refresh flash color pallete
  colorPal.calc_flash_color_pallet(LightS::s_lightColor);
  colorPal.reset_flash_algo();
  // resume time flow (temporary flash ligth effect) - if stopped
  movWind.resumeTimeFlow();
}
  
// (Re)Draw some possible artefacts on top of fractal structure
void MainProgAggr::draw_artefacts(sf::RenderWindow & win, AutoScale & rescale) {
  // Draw either rescaling or Lights structure
  if (rescale.ifRescaleActive()) {
    float scale = rescale.getShrinkCumulativeFactor();
    logtxt.rescale_draw(win, scale);
  } else {
    // draw Lights structure (if active)
    lightS.light_draw(win);  
  }

  // Draw Help if requested
  logtxt.help_draw(win);
  
  // Draw Speed if requested
  auto speed = movWind.get_speedScale();
  logtxt.speed_draw(win, speed);
  
  // Draw Snapshot saved confirmation
  logtxt.saved_draw(win);
  
  // Draw Snapshot info if requested
  logtxt.snapshot_draw(win);
}

// General key decodation
// can be dispatched to subordinate classes/structs
void MainProgAggr::key_decodation(const sf::Keyboard::Key key,
                                  Element& prim_element) {
  if (key == sf::Keyboard::Space) {
    // Stop both types of animation
    if (m_demoActive) { movWind.stopAnimation(); }
    else { movWind.stopFreezeAnimation(); }// Freeze time or permanent Stop
    movWind.stop_wind();
    // print current speed scale for # of frames
    logtxt.startSpeedDraw();
  }
  else if (key == sf::Keyboard::R) {
    resetConfig(true); // key action
  }
  else if (key == sf::Keyboard::F1) {
    // Help text will be appearing for some time
    logtxt.startHelpDraw();
  } 
  else if (key == sf::Keyboard::F2) {
    // Store fractal snapshot/configuration: transformations and colors
    logtxt.log_snapshot( prepareSnapshotData(), prim_element);
    // Display confirmation
    logtxt.startSavedDraw();
  } 
  else if (key == sf::Keyboard::F3) {
    // Retrieve fractal snapshot/configuration from file
    logtxt.load_next_snapshot( prim_element, movWind.algo_data, ColorPal::s_col_palet);
    // Refresh also flash color pallete
    colorPal.calc_flash_color_pallet(LightS::s_lightColor);
    // In case of change of leaf contruction reset flash
    colorPal.reset_flash_algo();
    movWind.resumeTimeFlow();
    // Allow display snapshot description (or time)
    logtxt.startSnapshotDraw();
  } 
  else if (key == sf::Keyboard::PageUp) {
    // Increase size thus speed
    movWind.speedIncrement();
    // draw speed scale for next xx frames
    logtxt.startSpeedDraw();
    movWind.resumeTimeFlow();
  } 
  else if (key == sf::Keyboard::PageDown) {
    // Decrease size thus speed
    movWind.speedDecrement();
    // draw speed scale for next xx frames
    logtxt.startSpeedDraw();
    movWind.resumeTimeFlow();
  } 
  else {
    if (key == sf::Keyboard::P) {
      // In case of change of leaf contruction reset flash
      colorPal.reset_flash_algo();
      movWind.resumeTimeFlow();
    }
    bool keyFound = false;
    // possible Colors Palletes related control
    keyFound |= colorPal.key_decodation(key);
    
    // possible animation move control
    keyFound |= movWind.MovAnim::key_decodation(key);
    
    // possible wind (wobbling) animation control
    keyFound |= movWind.MovWind::key_decodation(key);
    
    // possible Light control
    auto l_lightRet = lightS.key_decodation(key);
    if (l_lightRet.colorChanged) {
      // Refresh additionally flash color pallete
      colorPal.calc_flash_color_pallet(LightS::s_lightColor);
    }
    keyFound |= l_lightRet.lightMoved;

    // Key decodation successfull by some base functionality/class
    if (keyFound) movWind.resumeTimeFlow();
  }
  
}

// Single demo step
void MainProgAggr::demoGenerator(Element & primEl, AutoScale & autoScale) {
  // Counter for this Demo changes - it can be reset by demoRand
  static long int ownDemoCnt { cFrac::DemoInitCnt };
  // total Counter of demos
  static long int allDemoCnt { cFrac::DemoInitCnt };
  static long int lastActionDistance { 0 };

  if (!m_demoActive) return;
  
  // Count demo frames
  Dbg::demo_frames(allDemoCnt);
  if (allDemoCnt < 0) {
    Dbg::report_warning("Demo Counter Overflow ? : ", allDemoCnt);
    allDemoCnt = cFrac::DemoInitCnt;
  }
  assert(lastActionDistance >= 0 and "Demo algo counter failure");

  if (allDemoCnt <= cFrac::DemoInitCnt) {
    // start demo with displaying help text (on top of fractal)
    logtxt.startHelpDraw();
    // Initialize other subordinate generators
    (void)lightS.demoGenerator(cFrac::DemoInitCnt, 0);
    // Further initialization like seed generation
    srand(time(NULL));
    allDemoCnt = cFrac::DemoInitCnt +1;
    }
  else {
    ++lastActionDistance;
    ++allDemoCnt;
  }

  // Do NOT perform action one after another
  if (lastActionDistance < 25) return;
  
  ++ownDemoCnt;
  bool actionDone = false;

  // Reset every about 500 frames
  if (demoRand(500, ownDemoCnt) or movWind.stopAtZero) {
    // all reset
    primEl.initPrimary();
    autoScale.resetAutoScale();
    // Next Leaf construction
    movWind.rotate_pre_cfg();
    resetConfig(false);
    // Reset (some) subordinates generators
    lightS.demoGenerator(allDemoCnt, true);
    actionDone |= true;
  }
  
  if (lightS.demoGenerator(allDemoCnt, false)) {
    // Refresh flash color pallete if some color reconfiguration being made
    colorPal.calc_flash_color_pallet(LightS::s_lightColor);
    actionDone = true;
  }
  
  actionDone |= colorPal.demoGenerator(allDemoCnt, false);
  actionDone |= movWind.demoGenerator(allDemoCnt, false);

  if (actionDone) lastActionDistance = 0;
}


