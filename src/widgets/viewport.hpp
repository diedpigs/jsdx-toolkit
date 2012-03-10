#ifndef NODE_CLUTTER_WIDGETS_VIEWPORT_H_
#define NODE_CLUTTER_WIDGETS_VIEWPORT_H_

#include <clutter/clutter.h>
#include <v8.h>
#include "scrollable.hpp"

namespace clutter {

class Viewport : public Bin {
public:
	static v8::Persistent<v8::FunctionTemplate> constructor;
	static void Initialize(v8::Handle<v8::Object> target);

	Scrollable *scrollable;

protected:
	Viewport();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
};

}

#endif