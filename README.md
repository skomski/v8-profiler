# v8-profiler

node.js bindings for the v8 profiler

## Installation

    npm install v8-profiler

## Usage

    var profiler = require('v8-profiler');

    var heapshot = profiler.heapshot([name]);  // takes a heapSnapshot

    profiler.startProfiling([name]);                  // start cpu profiling
    var cpuProfile = profiler.stopProfiling([name]);  // stop cpu profiling
