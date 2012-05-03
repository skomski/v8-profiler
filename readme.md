# v8-profiler

node.js bindings for the v8 profiler

## Installation

    npm install v8-profiler

## Usage

    var profiler = require('v8-profiler');

## API

    var snapshot = profiler.takeSnapshot([name])      // takes a heap snapshot

    profiler.startProfiling([name])                   // begin cpu profiling
    var cpuProfile = profiler.stopProfiling([name])   // finish cpu profiling
