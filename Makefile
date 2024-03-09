run: clean_code
	mkdir src/build && cd src/build && cmake .. && make -j

.PHONY: clean clean_code clean_replay
clean_code:
	rm -rf src/build

clean_replay:
	rm -rf replay/*

clean: clean_code clean_replay
	@echo "Cleanup code and replay files."
	@echo "Cleanup debug logs."
	rm -f debug.log
	@echo "Cleanup partition txt."
	rm -f partition.txt
	@echo "Cleanup partition image."
	rm -rf helper/*.png

test:
	@if [ -f src/build/main ]; then \
		./run_self.sh; \
	else \
		echo "Error: src/build/main does not exist."; \
	fi
