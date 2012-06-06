#include "graph_node.h"
#include "graph_edge.h"
#include "node_version.h"
#include <iostream>

#include <string.h>

using namespace v8;

namespace nodex {

Persistent<ObjectTemplate> GraphNode::node_template_;

void GraphNode::Initialize() {
  node_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  node_template_->SetInternalFieldCount(1);
  node_template_->SetAccessor(String::NewSymbol("type"), GraphNode::GetType);
  node_template_->SetAccessor(String::NewSymbol("name"), GraphNode::GetName);
  node_template_->SetAccessor(String::NewSymbol("id"), GraphNode::GetId);
  node_template_->SetAccessor(String::NewSymbol("ptr"), GraphNode::GetPtr);
  node_template_->SetAccessor(
      String::NewSymbol("childrenCount"), GraphNode::GetChildrenCount);
  node_template_->SetAccessor(
      String::NewSymbol("retainersCount"), GraphNode::GetRetainersCount);
  node_template_->SetAccessor(
      String::NewSymbol("size"), GraphNode::GetSize);
  node_template_->SetAccessor(
      String::NewSymbol("dominatorNode"), GraphNode::GetDominator);
  node_template_->Set(
      String::NewSymbol("getChild"),
      FunctionTemplate::New(GraphNode::GetChild));
  node_template_->Set(
      String::NewSymbol("retainedSize"),
      FunctionTemplate::New(GraphNode::GetRetainedSize));
  node_template_->Set(
      String::NewSymbol("getRetainer"),
      FunctionTemplate::New(GraphNode::GetRetainer));
  node_template_->Set(
      String::NewSymbol("getHeapValue"),
      FunctionTemplate::New(GraphNode::GetHeapValue));
  node_template_->Set(
      String::NewSymbol("getHeapValueSafe"),
      FunctionTemplate::New(GraphNode::GetHeapValueSafe));
}

Handle<Value> GraphNode::GetType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t type = static_cast<int32_t>(static_cast<HeapGraphNode*>(ptr)->GetType());
  Local<String> t;
  switch(type) {
    case HeapGraphNode::kArray :
        t = String::New("Array");
        break;
    case HeapGraphNode::kString :
        t = String::New("String");
        break;
    case HeapGraphNode::kObject :
        t = String::New("Object");
        break;
    case HeapGraphNode::kCode :
        t = String::New("Code");
        break;
    case HeapGraphNode::kClosure :
        t = String::New("Closure");
        break;
    case HeapGraphNode::kRegExp :
        t = String::New("RegExp");
        break;
    case HeapGraphNode::kHeapNumber :
        t = String::New("HeapNumber");
        break;
    case HeapGraphNode::kNative :
        t = String::New("Native");
        break;
    default:
        t = String::New("Hidden");
  }
  return scope.Close(t);
}

Handle<Value> GraphNode::GetName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<HeapGraphNode*>(ptr)->GetName();
  return scope.Close(title);
}


Handle<Value> GraphNode::GetId(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint64_t id = static_cast<HeapGraphNode*>(ptr)->GetId();
  return scope.Close(Integer::NewFromUnsigned(id));
}

Handle<Value> GraphNode::GetPtr(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint64_t id = reinterpret_cast<uint64_t>(ptr);
  return scope.Close(Integer::NewFromUnsigned(id));
}

Handle<Value> GraphNode::GetChildrenCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetChildrenCount();
  return scope.Close(Integer::New(count));
}

Handle<Value> GraphNode::GetRetainersCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetRetainersCount();
  return scope.Close(Integer::New(count));
}

Handle<Value> GraphNode::GetSize(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetSelfSize();
  return scope.Close(Integer::New(count));
}

Handle<Value> GraphNode::GetChild(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphNode*>(ptr)->GetChild(index);
  return scope.Close(GraphEdge::New(edge));
}

Handle<Value> GraphNode::GetRetainedSize(const Arguments& args) {
  HandleScope scope;

  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);

#if NODE_VERSION_AT_LEAST(0, 7, 0)
  int32_t size = static_cast<HeapGraphNode*>(ptr)->GetRetainedSize();
#else
  bool exact = false;
  if (args.Length() > 0) {
    exact = args[0]->BooleanValue();
  }
  int32_t size = static_cast<HeapGraphNode*>(ptr)->GetRetainedSize(exact);
#endif

  return scope.Close(Integer::New(size));
}

Handle<Value> GraphNode::GetHeapValueSafe(const Arguments& args) {
  HandleScope scope;
  Handle<Object> self = args.This();
  HeapGraphNode* node = static_cast<HeapGraphNode*>(self->GetPointerFromInternalField(0));

  Handle<String> title = node->GetName();
  v8::String::AsciiValue str(title);
  char* name = *str;

  if (name[0] == '(') {
    // we assume this is not safe if name begins with a '(' as those
    // objects seem to be system objects and emperically cause a seg
    // fault.
    return scope.Close(v8::Undefined());
  }

  int type = node->GetType();

  switch(type) {
    case HeapGraphNode::kString:
      // special case as some strings seem to cause a seg fault, so return the name
      return scope.Close(title);
      break;

    case HeapGraphNode::kArray:
      // type array with an empty name seems to segfault
      if (strlen(name) == 0) return scope.Close(v8::Undefined());

    case HeapGraphNode::kClosure:
    case HeapGraphNode::kObject:
    case HeapGraphNode::kRegExp:
    case HeapGraphNode::kHeapNumber:
      return scope.Close(node->GetHeapValue());
      break;

    case HeapGraphNode::kNative:
    case HeapGraphNode::kCode:
    default:
      return scope.Close(v8::Undefined());
  }
}


Handle<Value> GraphNode::GetHeapValue(const Arguments& args) {
  HandleScope scope;
  Handle<Object> self = args.This();
  HeapGraphNode* node = static_cast<HeapGraphNode*>(self->GetPointerFromInternalField(0));
  return scope.Close(node->GetHeapValue());
}

Handle<Value> GraphNode::GetRetainer(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphNode*>(ptr)->GetRetainer(index);
  return scope.Close(GraphEdge::New(edge));
}

Handle<Value> GraphNode::GetDominator(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr)->GetDominatorNode();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> GraphNode::New(const HeapGraphNode* node) {
  HandleScope scope;
  
  if (node_template_.IsEmpty()) {
    GraphNode::Initialize();
  }
  
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = node_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}
}
