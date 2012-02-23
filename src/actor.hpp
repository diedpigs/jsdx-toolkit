#ifndef NODE_CLUTTER_ACTOR_H_
#define NODE_CLUTTER_ACTOR_H_

#include <clutter/clutter.h>
#include <v8.h>
#include <node.h>

namespace clutter {

class Actor : public node::ObjectWrap {
public:
	static void Initialize(v8::Handle<v8::Object> target);
	static void PrototypeMethodsInit(v8::Handle<v8::FunctionTemplate> constructor_template);

	ClutterActor *_actor;

protected:
	Actor();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
	static v8::Handle<v8::Value> Show(const v8::Arguments& args);
	static v8::Handle<v8::Value> ShowAll(const v8::Arguments& args);
	static v8::Handle<v8::Value> Hide(const v8::Arguments& args);

	static v8::Handle<v8::Value> SetOpacity(const v8::Arguments& args);
	static v8::Handle<v8::Value> Resize(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetWidth(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetWidth(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetHeight(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetHeight(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetPosition(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetDepth(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetDepth(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetX(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetX(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetY(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetY(const v8::Arguments& args);

	static v8::Handle<v8::Value> Scale(const v8::Arguments& args);
};

}

#endif
