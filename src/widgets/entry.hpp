#ifndef JSDX_TOOLKIT_WIDGET_ENTRY_H_
#define JSDX_TOOLKIT_WIDGET_ENTRY_H_

#include <clutter/clutter.h>
#include <mx/mx.h>
#include <v8.h>
#include "widget.hpp"

namespace JSDXToolkit {

class Entry : public Widget {
public:
	static void Initialize(v8::Handle<v8::Object> target);
	static void PrototypeMethodsInit(v8::Handle<v8::FunctionTemplate> constructor_template);

protected:
	Entry();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);

	static v8::Handle<v8::Value> TextGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
	static void TextSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

	static v8::Handle<v8::Value> PasswordCharGetter(v8::Local<v8::String> name, const v8::AccessorInfo& info);
	static void PasswordCharSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

}

#endif
