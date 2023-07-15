FILE=A16
CFLAGS = -std=c++17 -O0 -Iheaders
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: $(FILE).cpp
	g++ $(CFLAGS) -o $(FILE) $(FILE).cpp $(LDFLAGS)

.PHONY: test clean

test: $(FILE)
	./$(FILE)

clean:
	rm -f $(FILE)
