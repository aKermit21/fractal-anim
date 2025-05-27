// Copyright (c) 2025 Robert Gajewski
// (MIT License)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <cstdlib>
#include <optional>
#include <string>
#include <iostream>
#include "dbg_report.h"
#include "garbage_coll.h"
#include "fractal.h"

// Garbage collector static variables
unsigned long MemAndDebug::ptrs_cnt; 
// Collection of pointers ownership
std::vector<std::unique_ptr<std::array<Element, cFrac::NrOfElements>>> 
    MemAndDebug::all_ptrs;

// Static Counters
int Dbg::error_cnt {0}; 
long int Dbg::warning_cnt {0}; 
long int Dbg::info_cnt {0}; 
long int Dbg::elements {0};
long int Dbg::m_demoCnt {0};

std::chrono::time_point<Dbg::Clock> Dbg::time_beg;
Dbg::VecMinMax Dbg::minmax;


void Dbg::report_info(std::string_view s, std::optional<long> i) {
  if(cReportInfo) {
    std::cerr << "  Info: "<< s;
    if (i) {
      std::cerr << *i;
    }
    std::cerr << '\n';
  }
  ++info_cnt; 
}

void Dbg::report_trace(std::string_view s, std::optional<long> i) {
  if(cReportTrace) {
    std::cerr << "  Trace: "<< s;
    if (i) {
      std::cerr << *i;
    }
    std::cerr << '\n';
    
  }
  ++info_cnt; 
}

void Dbg::report_warning(std::string_view s, std::optional<long> i) {
  if(cReportWarning) {
    std::cerr << " Warning: "<< s;
    if (i) {
      std::cerr << *i;
    }
    std::cerr << '\n';
  }
  ++warning_cnt; 
}

void Dbg::report_error(std::string s, long int i) {
  if(cReportError) {
    std::cerr << "! ERROR: "<< s << i << '\n'; 
  }
  ++error_cnt; 
  if (error_cnt >= exitAfterErrors) {
    throw " Exiting program because of too much ERRORs";
  }
}

void Dbg::report_mltpl_warning(Dbg::MultipleWarning mwtype, long int counter) {
  if(cReportWarning) {
    static long mwCreate { cCreateWarningThreshold };
    static long mwDraw { cDrawWarningThreshold };
    static long mwPointer { cCreatePointersThreshold };

    if ((mltplElementsCreate == mwtype) and (counter >= mwCreate)) {
      std::cerr << " Warning: Too much Elements created: " << counter << '\n';
      mwCreate *= 2; // double threshold for next time
      ++warning_cnt; 
    }
    
    if ((mltplElementsDraw == mwtype) and (counter >= mwDraw)) {
      std::cerr << " Warning: Too much Elements Draw per cycle: " << counter << '\n';
      mwDraw *= 2; // double threshold for next time
      ++warning_cnt; 
    }
    
    if ((mltplPointers == mwtype) and (counter >= mwPointer)) {
      std::cerr << " Warning: Too much Smart Pointers collected: " << counter << '\n';
      mwPointer *= 2; // double threshold for next time
      ++warning_cnt; 
    }
  } else {
    static bool already_warned { false };
    // increment counter by one if no Report Warning option is chosen
    if (!already_warned) {
      ++warning_cnt;
    }
  }
  
}
  
void Dbg::count_elements(int i) {
  if ((i < 0) && (cReportError)) {
    std::cerr << "! ERROR: Deleted element?" << i << '\n'; 
    ++error_cnt; 
  } else {
    elements += i;
  }
}

void Dbg::demo_frames(long int cnt) {
  if (cnt > m_demoCnt) m_demoCnt = cnt;
}

// Collect min max drawing edge from frame minmax to entire program run one
void Dbg::find_minmax(const VecMinMax vec) {
  if (minmax.minX > vec.minX) { minmax.minX = vec.minX;}
  if (minmax.maxX < vec.maxX) { minmax.maxX = vec.maxX;}
  if (minmax.minY > vec.minY) { minmax.minY = vec.minY;}
  if (minmax.maxY < vec.maxY) { minmax.maxY = vec.maxY;}
}


// Timing, Counting elements - per frame
void before_recurrence() {
  
}

void after_recurrence() {
  
}


void Dbg::report_summary(void) noexcept {
  std::cout << "Total # of Elements: "<< elements << std::endl; 
  std::cout << "Total # of Infos: "<< info_cnt << '\n'; 
  if (m_demoCnt > 0) {
    std::cout << "Demo frames: "<< m_demoCnt << '\n'; 
  }
  std::cout << "Min/Max /   "<< minmax.minY  << "   \\ \n"; 
  std::cout << "Min/Max |"<< minmax.minX << "  " << minmax.maxX << "| \n"; 
  std::cout << "Min/Max \\   "<< minmax.maxY  << "  / \n"; 
  std::cout << "Total # of Warnings: "<< warning_cnt << '\n'; 
  std::cout << "Total # of ERRORS: "<< error_cnt << '\n'; 
  }
