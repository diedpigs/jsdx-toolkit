#ifndef NODE_CLUTTER_WIDGETS_VIEWPORT_H_
#define NODE_CLUTTER_WIDGETS_VIEWPORT_H_

#include <clutter/clutter.h>
#include <v8.h>

namespace clutter {

class Viewport : public Bin {
public:
	static void Initialize(v8::Handle<v8::Object> target);

protected:
	Viewport();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
};

}

#endif
