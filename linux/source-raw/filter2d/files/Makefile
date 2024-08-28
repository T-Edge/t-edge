APP = filter2d

# Add any other object files to this list below
APP_OBJS = cmdlineparser.o  filter2d.o  kernel_hw.o  kernel_sw.o

CXXFLAGS += $(shell pkg-config --cflags opencv4) -Wno-narrowing
LDFLAGS += $(shell pkg-config --libs opencv4)

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CXX) -o $@ $(APP_OBJS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(APP) *.o
