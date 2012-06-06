var profiler = require('..');
var assert = require('assert');

var passed = false;

// TestObject

var testDate = "2012";

function TestObject() {
  this.number = 200;
  this.array  = [ 1, 2, 3, "asd"];
  this.object = { first: "good" };
  this.function = function() { var good = "good"; };
  this.date = new Date(testDate);
  this.testBuffer = new Buffer(100);
  this.string = "text";
};
TestObject.prototype.foo = 5;

var test = new TestObject();


var heapshot = profiler.heapshot('test');
var nodes = heapshot.allNodes();

for (var key in nodes) {
  var node = nodes[key];

  try {
    node.getHeapValueSafe() + '';
  } catch (err) {
    assert.equal(err.name, 'TypeError');
  }

  if (node.type === 'Object' && node.name === 'TestObject' && node.size > 40) {
    var val = node.getHeapValueSafe();
    if (val != undefined) {
      assert.deepEqual(val.number, 200);
      assert.deepEqual(val.array, [1,2,3,"asd"]);
      assert.deepEqual(val.object, {"first":"good"});
      assert.deepEqual(val.date, new Date(testDate));
      assert.deepEqual(val.string, "text");
      assert.deepEqual(val.function, "function () { var good = \"good\"; }");

      assert.ok(node.retainersCount);

      var children = heapshot.children(node);
      for (var childKey in children) {
        var childNode = children[childKey];
        console.log(childNode.name);
        console.log(childNode.to.getHeapValueSafe());

        switch(childNode.name) {
          case 'function':
            assert.equal(
                childNode.to.getHeapValueSafe(),
                "function () { var good = \"good\"; }");
            passedFunction = true;
            break;
          case 'string':
            assert.equal(childNode.to.getHeapValueSafe(), "text");
            passedString = true;
            break;
          case 'date':
            assert.deepEqual(childNode.to.getHeapValueSafe(), new Date(testDate));
            passedDate = true;
            break;
          case 'object':
            assert.deepEqual(childNode.to.getHeapValueSafe(), {"first":"good"});
            passedObject = true;
            break;
          case 'array':
            assert.deepEqual(childNode.to.getHeapValueSafe(), [1,2,3,"asd"]);
            passedArray = true;
            break;
          case '__proto__':
            assert.deepEqual(childNode.to.getHeapValueSafe(), { foo: 5 });
            passedProto = true;
            break;
          case 'testBuffer':
            assert.equal(childNode.to.getHeapValueSafe().length, 100);
            passedBuffer = true;
            break;
        }
      }

      assert(passedFunction);
      assert(passedString);
      assert(passedDate);
      assert(passedObject);
      assert(passedArray);
      assert(passedProto);
      assert(passedBuffer);
      passed = true;
    }
  }
}

process.on('exit', function() {
  assert.ok(passed);
});

