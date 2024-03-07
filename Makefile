run:
	mkdir src/build && cd src/build && cmake .. && make -j
	./run_self.sh

.PHONY: clean clean_code clean_replay
clean_code:
	rm -rf src/build

clean_replay:
	rm -rf replay/*

clean: clean_code clean_replay
	echo "Cleanup code and replay files."

