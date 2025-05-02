// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "fractal.h"
#include "dbg_report.h"
#include <iostream>
#include <memory>
#include <vector>

// Collects link pointers ownership - Garbage Collector
// Dbg: Collects and reports debug info from every other class
struct MemAndDebug : Dbg
{
  MemAndDebug(){
    ptrs_cnt = 0;
  }
  // explicit destructor always virtual: learncpp 25.4
  virtual ~MemAndDebug(){
#ifndef NDEBUG
    std::cerr << "Garbage Collector (Memory management) auto clean-up Done." << std::endl;
#endif
  }

  
  // Add ownership pointer to the collection
  static void collect_u_ptr(std::unique_ptr<std::array<Element, cFrac::NrOfElements>> ptr) {
    all_ptrs.push_back(std::move(ptr));
    ++ptrs_cnt;

    if (Dbg::cReportWarning) {
      if (ptrs_cnt >= Dbg::cCreatePointersThreshold) {
        Dbg::report_mltpl_warning( Dbg::mltplPointers, ptrs_cnt);
      }
    }

    // Check some very Big Number
    if (Dbg::cReportError and ptrs_cnt > 50'000'000) {
      Dbg::report_error("Too large # of u ptrs collected: ", ptrs_cnt);
      // Fatal Error
      release_all();
      throw "Too large number of elements created.";
    }
  }

    // Explicit release all allocated data from the collection
  static void release_all(){
    // Release all (manually) in reverse order
    for (auto rit { MemAndDebug::all_ptrs.rbegin() }; rit != MemAndDebug::all_ptrs.rend(); ++rit)  {
      rit->reset();  // release allocated data
    }
    all_ptrs.clear();  // clear vector itself
    std::cerr << "Garbage collector manually clean-up\n";
  }

  private:
  static unsigned long ptrs_cnt; 
  // Collection of pointers ownership
  static std::vector<std::unique_ptr<std::array<Element, cFrac::NrOfElements>>> all_ptrs;
};

// extern GarbColl gc;

