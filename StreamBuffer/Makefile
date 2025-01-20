.PHONY: clean

all: testingStreamBuffer

testingStreamBuffer: streamBuffer.c testingStreamBuffer.c
	$(CC) $(^) -o $(@)

clean: testingStreamBuffer
	$(RM) $(^)
