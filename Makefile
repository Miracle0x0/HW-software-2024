# 颜色定义
RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m # 没有颜色

EXEC_NAME=tmp/build/main

run: clean_code
	@echo "${GREEN}Copy source files to tmp directory.${NC}"
	cp -r src tmp
	@echo "${GREEN}Merge headers into target file: main.cpp.${NC}"
	python3 helper/merge_header.py
	@echo "${GREEN}Format code with clang-format.${NC}"
	clang-format -i tmp/main.cpp --style=file:.clang-format
	@echo "${GREEN}Build the project with CMake.${NC}"
	mkdir tmp/build && cd tmp/build && cmake .. && make -j

.PHONY: clean clean_code clean_replay
clean_code:
	rm -rf tmp

clean_replay:
	rm -rf replay/*

clean: clean_code clean_replay
	@echo "${GREEN}Cleanup code and replay files.${NC}"
	@echo "${GREEN}Cleanup debug logs.${NC}"
	@rm -f debug.log
	@echo "${GREEN}Cleanup txt files.${NC}"
	@rm -f *.txt
	@echo "${GREEN}Cleanup images.${NC}"
	@rm -rf image

test:
	@if [ -f ${EXEC_NAME} ]; then \
	    ./run_self.sh; \
	else \
		echo "${RED}Error: ${EXEC_NAME} does not exist.${NC}"; \
	fi