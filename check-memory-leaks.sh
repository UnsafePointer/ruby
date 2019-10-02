#!/usr/bin/env bash

valgrind -q --leak-check=full --show-leak-kinds=all ./build/ruby
