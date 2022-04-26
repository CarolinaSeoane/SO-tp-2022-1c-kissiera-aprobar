#!/bin/bash

gnome-terminal  --tab --command="bash -c 'cd memoria; ./memoria.out; $SHELL'" --tab --command="bash -c 'cd cpu; ./cpu.out; $SHELL'" --tab --command="bash -c 'cd kernel; ./kernel.out; $SHELL'" 