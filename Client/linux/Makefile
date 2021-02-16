#
# You will need GLFW (http://www.glfw.org):
#   brew install glfw
#

#CXX = g++
#CXX = clang++

EXE = sonyheadphonesclient
SOURCES = main.cpp
SOURCES += ../GUI_Impls/LinuxGUI.cpp ../GUI_Impls/CrossPlatformGUI.cpp
SOURCES += LinuxBluetoothConnector.cpp BluetoothWrapper.cpp DBusHelper.cpp
SOURCES += TimedMessageQueue.cpp CascadiaCodeFont.cpp CommandSerializer.cpp
SOURCES += ByteMagic.cpp
SOURCES += ../imgui/examples/imgui_impl_glfw.cpp ../imgui/examples/imgui_impl_opengl3.cpp
SOURCES += ../imgui/imgui.cpp ../imgui/imgui_widgets.cpp ../imgui/imgui_demo.cpp ../imgui/imgui_draw.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

LIBS += -L/usr/local/lib -lglfw -lpthread 

CXXFLAGS = -I../imgui/ -I../GUI_Impls/ -I../imgui/examples/ -I../ -I/usr/local/include
CXXFLAGS += -Wall -Wformat -std=c++17
CXXFLAGS += -DIMGUI_IMPL_OPENGL_LOADER_GLEW
CXXFLAGS += `pkg-config --cflags glfw3`
CXXFLAGS += `pkg-config --cflags dbus-1`

CFLAGS = $(CXXFLAGS)
LIBS += -lGLEW -lbluetooth -ldbus-1
LIBS += -lGL `pkg-config --static --libs glfw3`

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:../%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
%.o:../imgui/examples/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:../imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:../GUI_Impls/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:../%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

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
	rm -rf "./$(EXE).build/"

package_app:
	mkdir -p "./$(EXE).build"
	cp ./$(EXE) "./$(EXE).build/$(EXE)"

