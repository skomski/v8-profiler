# v8-profiler [![Build Status](https://secure.travis-ci.org/Skomski/v8-profiler.png?branch=unstable)](http://travis-ci.org/Skomski/v8-profiler)

node.js bindings for the v8 profiler

## Installation

    npm install https://github.com/Skomski/v8-profiler/tarball/v3.7.1

## Usage

    var profiler = require('v8-profiler');

    var heapshot = profiler.heapshot([name]);  // takes a heapSnapshot

    profiler.startProfiling([name]);                  // start cpu profiling
    var cpuProfile = profiler.stopProfiling([name]);  // stop cpu profiling
