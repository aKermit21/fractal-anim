build/aggreg.o: src/aggreg.cpp src/aggreg.h src/autoscale.h src/dbg_report.h \
 src/fractal.h src/transform.h src/colors.h src/light.h src/logtxt.h \
 src/opt_lyra.h \
 src/text_draw.h src/windy.h \
 src/animation.h src/demo_func.h
build/animation.o: src/animation.cpp src/animation.h src/fractal.h \
 src/transform.h src/dbg_report.h src/demo_func.h
build/autoscale.o: src/autoscale.cpp src/autoscale.h src/dbg_report.h \
 src/fractal.h src/transform.h
build/colors.o: src/colors.cpp src/colors.h src/dbg_report.h src/fractal.h \
 src/light.h src/demo_func.h
build/dbg_report.o: src/dbg_report.cpp src/dbg_report.h src/fractal.h \
 src/garbage_coll.h
build/demo_func.o: src/demo_func.cpp src/demo_func.h
build/draw.o: src/draw.cpp src/fractal.h src/dbg_report.h src/light.h \
 src/transform.h src/colors.h src/animation.h
build/light.o: src/light.cpp src/light.h src/dbg_report.h src/fractal.h \
 src/demo_func.h
build/logtxt.o: src/logtxt.cpp src/logtxt.h src/fractal.h src/opt_lyra.h \
 src/text_draw.h \
 src/dbg_report.h src/transform.h src/windy.h src/animation.h
build/main.o: src/main.cpp src/dbg_report.h src/fractal.h src/aggreg.h \
 src/autoscale.h src/transform.h src/colors.h src/light.h src/logtxt.h \
 src/opt_lyra.h \
 src/text_draw.h src/windy.h \
 src/animation.h src/garbage_coll.h
build/opt_lyra.o: src/opt_lyra.cpp src/opt_lyra.h \
 src/dbg_report.h \
 src/fractal.h
build/recurrence.o: src/recurrence.cpp src/animation.h src/fractal.h \
 src/transform.h src/dbg_report.h src/autoscale.h src/garbage_coll.h
build/text_draw.o: src/text_draw.cpp src/text_draw.h src/dbg_report.h \
 src/fractal.h src/transform.h
build/transform.o: src/transform.cpp src/transform.h src/dbg_report.h \
 src/fractal.h src/animation.h
build/vec2angle.o: src/vec2angle.cpp src/dbg_report.h src/fractal.h src/light.h
build/vec2rotate.o: src/vec2rotate.cpp src/dbg_report.h src/fractal.h \
 src/transform.h
build/windy.o: src/windy.cpp src/windy.h src/fractal.h src/animation.h \
 src/transform.h src/dbg_report.h
