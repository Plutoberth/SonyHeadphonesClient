#
# You will need GLFW (http://www.glfw.org):
#   brew install glfw
#

#CXX = g++
#CXX = clang++

EXE = sonyheadphonesclient
SOURCES = main.mm
SOURCES += MacOSGUI.cpp MacOSBluetoothConnector.cpp BluetoothWrapper.cpp
SOURCES += TimedMessageQueue.cpp CascadiaCodeFont.cpp CommandSerializer.cpp
SOURCES += ByteMagic.cpp
SOURCES += ../imgui/examples/imgui_impl_glfw.cpp ../imgui/examples/imgui_impl_metal.mm
SOURCES += ../imgui/imgui.cpp ../imgui/imgui_widgets.cpp ../imgui/imgui_demo.cpp ../imgui/imgui_draw.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

LIBS = -framework IOBluetooth -framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore
LIBS += -L/usr/local/lib -lglfw

CXXFLAGS = -I../imgui/ -I../imgui/examples/ -I../ -I/usr/local/include
CXXFLAGS += -Wall -Wformat -std=c++17 -stdlib=libc++
CFLAGS = $(CXXFLAGS)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:../%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:../imgui/examples/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:../imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:../imgui/examples/%.mm
	$(CXX) $(CXXFLAGS) -ObjC++ -fobjc-weak -fobjc-arc -c -o $@ $<

%.o:%.mm
	$(CXX) $(CXXFLAGS) -ObjC++ -fobjc-weak -fobjc-arc -c -o $@ $<

%.o:../%.mm
	$(CXX) $(CXXFLAGS) -ObjC++ -fobjc-weak -fobjc-arc -c -o $@ $<

all: $(EXE)
	$(MAKE) clean_app
	$(MAKE) package_app
	$(MAKE) clean
	@echo Build complete

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)

clean_app:
	rm -rf "./$(EXE).app/"

package_app:
	mkdir -p "./$(EXE).app"/Contents/{MacOS,Resources}
	cp ./Info.plist "./$(EXE).app/Contents/"
	sed -e "s/APP_NAME/$(EXE)/g" -i "" "./$(EXE).app/Contents/Info.plist"
	cp ./$(EXE) "./$(EXE).app/Contents/MacOS/$(EXE)"