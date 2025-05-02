// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "dbg_report.h"
#include "fractal.h"
#include "aggreg.h"
#include "autoscale.h"
#include "opt_lyra.h"
#include "transform.h"
#include "garbage_coll.h"
#include "logtxt.h"
#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

bool recurance_elements_creation(Element * prim_ptr, short level);

bool recurance_elements_redraw(Element * const prim_ptr, const short level, 
                sf::RenderWindow & win, const MovAnim & algo_anim, AutoScale & autoScale);


int main(int argc, const char** argv)
{
  try {

    // analyze command CLI options
    OptParams options { optParse(argc, argv ) };
    assert(options.parseResult >= OptParams::ok and "parse conclusion missing?"); 
    // Early return if problems with parsing or help/version only called
    if (options.parseResult == OptParams::help ) { return 0; }
    else if (options.parseResult == OptParams::error ) { return 2; } // cmd parsing error
    else { /* continue */ }
  
    // Collecting errors, warning, info (trace); also Garbage collector: memory management
    MemAndDebug dbg;
    // Auto (re)scalling
    AutoScale autoScale(!options.optAutoScaleOff);

    // Top aggregation starts: Drawing, animation, colors, logs, key decode
    MainProgAggr fractMain(options);
    fractMain.postInitSync();

    std::string windowName {cFrac::ProgramName};
    if (options.optDemo) windowName = cFrac::DemoProgramName;

    sf::RenderWindow window(sf::VideoMode(cFrac::WindowXsize, cFrac::WindowYsize), 
                            windowName);

    // Initialization...(wait) text
    fractMain.logtxt.welcome_draw(window, options.optSpeed);
    window.display();
  
    // First fractal element (order 0)
    Element prim_element;
    prim_element.initPrimary();

    // Using recurrence algo create all other elements starting from primary one
    (void)recurance_elements_creation(&prim_element, 1);  // parent node, next level (order)

    while (window.isOpen()) {

      sf::Event event;
      while (window.pollEvent(event)) {
        switch (event.type) {
          // Window button close
          case sf::Event::Closed:
            window.close();
            break;
          case sf::Event::KeyPressed:
          // Close on X and Escape
            if ((event.key.code == sf::Keyboard::X) or
                (event.key.code == sf::Keyboard::Escape)) {
              window.close();
            } else {
              if (event.key.code == sf::Keyboard::R) {
                // Reset
                prim_element.initPrimary();
                autoScale.resetAutoScale();
              } // intentionaly lack of else, reset handling continued below
              // Keys decodation dispatcher
              fractMain.key_decodation(event.key.code);
            }
            break;
          default: 
            break;
        }
      }

      window.clear();

      autoScale.cycleStart();

      // Reconfigurate elements according to current algo and Draw in recurrence
      (void)recurance_elements_redraw(&prim_element, 0, window, 
                                      fractMain.movWind, autoScale); // 0 - start level

      autoScale.cycleResume(prim_element);

      // Light source and/or possible text info
      fractMain.draw_artefacts(window, autoScale);

      window.display();
    
      // possible signle step change of algo due to animation or flash (light effect)
      if (!autoScale.ifRescaleActive()) {
        fractMain.one_step_cfg_change();
        // also possible demo generation step
        fractMain.demoGenerator(prim_element, autoScale);
      }
    }

  }
  catch (const char * exception) {
    std::cerr << "Exception (Fatal Error): " << exception << std::endl;
  }
#ifdef NDEBUG
  // For release mode catch all exceptions
  catch(const std::exception & exception) {
    std::cerr << "Error - Standard (std) Exception: " << exception.what()  << std::endl;
  }
  catch(...) {
    std::cerr << "Error - Unknown Exception" << std::endl;
  }
  // std::cout << " NDEBUG !! (release mode)" << std::endl;
#endif

  // gc.release_all(); //Manually (not really needed) dynamic data cleanup
  return 0;
}

