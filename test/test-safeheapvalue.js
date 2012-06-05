// test out the getHeapValueSafe function.  make sure it completes and
// doesn't miss or get anything that is unexpected.

var profiler = require('v8-profiler');
var assert = require('assert');

// define some objects, arrays, etc.

var xx = [ 1, 2, 3, "asd" ];

var yy = { first: xx, second: 'haha', third: function() { xx = []; }};

function TestObject() { this.status = 200; this.foo = 6; this.bar = xx; this.zz = yy; };
TestObject.prototype.foo = 5;

var test = new TestObject();

// grab a snapshot

var heapshot = profiler.heapshot('test');

var nodes = heapshot.allNodes();

// go through the nodes and decide which should be safe and which shouldn't be safe
var skip=0;
var get=0;
var map = {};
for (var key in nodes) {
    var node = nodes[key];
    var skipit = 0;
    if (node.name.substr(0,1) == "(")
	skipit = 1;
    else if (node.type == "Hidden") skipit = 1;
    else if (node.type == "Code") skipit = 1;
    else if (node.type == "HeapNumber") skipit = 1;
    else if ((node.type == "Object")&&(node.name == "Object")) {
	if ((node.dominatorNode.type == 'Hidden')&&(node.dominatorNode.name == 'system / FunctionTemplateInfo')) skipit = 1;
	else if ((node.dominatorNode.type == 'Hidden')&&(node.dominatorNode.name == 'system / ObjectTemplateInfo')) skipit = 1;
    } else if ((node.type == "Array")&&(node.name == "")) skipit = 1;
    map[key] = skipit;
    skip += skipit;
    get += (1-skipit);
}

// now try and get the value for every node in the snapshot
for (var key in nodes) {
    var node = nodes[key];
    var skipped = 0;
    val = node.getHeapValueSafe();
    if (val == undefined) skipped = 1;
    if (key in map) {
	if (map[key] == skipped) delete map[key];
    } else {
	console.log(val + "was not in map");
    }
    skip -= skipped;
    get -= (1-skipped);
}

// now test that it behaved properly.  Best I can do is see that get
// and skip are small since there may be some undefined values in the
// heap.
var passed = false;
if (((get > 0)&&(get < 3))&&((skip < 0)&&(skip > -3))) {
    console.log("All ok");
    passed = true;
} else {
    // we got an unexpected result, so show what is left in the map
    console.log("After getting values we have get:"+get+" and skip:"+skip);
    for (key in map) {
	var node = nodes[key];
	console.log(">"+node.id+"\t"+node.size+"\t"+node.type+"\t"+(node.type === 'String' ? '' : node.name)+" skip:"+map[key]);
	console.log(node);
	val = node.getHeapValueSafe();
	console.log("value of "+key);
	console.log(val);
    }
}

process.on('exit', function() {
  assert.ok(passed);
});

