var profiler = require('..');
var assert = require('assert');

var testFinished = false;

function TestObject() { this.status = [200]; };

var test = new TestObject();

var heapshot = profiler.heapshot('test');

assert.equal(heapshot.uid, 1);
assert.equal(heapshot.title, 'test');
assert.equal(heapshot.type, 'Full');
assert.ok(heapshot.nodesCount > 10000);

var nodes = heapshot.allNodes();
var foundTestNode = false;

for (var key in nodes) {
  var node = nodes[key];

  assert(node.type);

  if (node.type === 'Object' && node.name === 'TestObject' && node.size > 40) {
    var val = node.getHeapValueSafe();
    if (val != undefined) {
      assert.deepEqual(val, { status: [200] });
      assert.ok(node.retainersCount);
      foundTestNode = true;
    }
  }
}

assert.ok(foundTestNode);

heapshot.serialize({
  onData: function(chunk, size) {
    assert.ok(chunk.length);
    assert.ok(size);
  },
  onEnd: function() {
    testFinished = true;
  }
});

process.on('exit', function() {
  assert.ok(testFinished);
});
