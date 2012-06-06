test:
	node test/test-heapshot.js
	node test/test-cpu-profiler.js
	node test/test-heapshot-GetSafeHeapValue.js

.PHONY: test
