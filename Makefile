CC=clang
OBJECT_FILES= stopwatch.o
CFLAGS=`pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`
EXECUTABLE= stopwatch

$(EXECUTABLE): $(OBJECT_FILES)
	$(CC) stopwatch.o $(LDFLAGS) -o $(EXECUTABLE)

stopwatch.o: stopwatch.c
	$(CC) -c stopwatch.c $(CFLAGS) 

clean:
	rm -r $(EXECUTABLE) $(OBJECT_FILES)
