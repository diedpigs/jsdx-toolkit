#ifndef JSDX_TOOLKIT_GROUP_H_
#define JSDX_TOOLKIT_GROUP_H_

#include <v8.h>
#include <node.h>

namespace JSDXToolkit {

	class Group : public Container {
	public:
		static void Initialize(v8::Handle<v8::Object> target);

	protected:
		Group();

		static v8::Handle<v8::Value> New(const v8::Arguments& args);
	};

}

#endif
