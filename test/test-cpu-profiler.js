var profiler = require('..');
var assert = require('assert');

var testFinished = false;

profiler.startProfiling('test');
function fibonacci(n) {
  if (n <= 1) return n;
  return fibonacci(n - 2) + fibonacci(n - 1);
}
for (var i = 0; i < 20; i++) {
  fibonacci(i);
}
var cpuProfile = profiler.stopProfiling('test');

assert.equal(cpuProfile.uid, 1);
assert.equal(cpuProfile.title, 'test');

function walk(node) {
  for (var key in node.children) {
    var value = node.children[key];
    if (node.functionName === 'fibonacci') {
      assert.ok(node.url.indexOf('test-cpu-profiler.js') > 1);
      assert.equal(node.lineNumber, 7);
      testFinished = true;
    }
    walk(value);
  }
}

walk(cpuProfile.getTopDownRoot());
walk(cpuProfile.getBottomUpRoot());

process.on('exit', function() {
  assert.ok(testFinished);
});
