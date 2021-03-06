/*
 * JSDX Framework
 *
 * Copyright(c) 2012 Fred Chien <fred@mandice.com>
 *
 */

#include <v8.h>
#include <node.h>
#include <clutter/clutter.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>

#include "../jsdx_toolkit.hpp"
#include "../actor.hpp"
#include "style.hpp"
#include "widget.hpp"
#include "adjustment.hpp"
#include "scrollable.hpp"
#include "button.hpp"
#include "entry.hpp"
#include "frame.hpp"
#include "label.hpp"
#include "dialog.hpp"
#include "progress_bar.hpp"
#include "slider.hpp"
#include "toggle.hpp"
#include "spinner.hpp"
#include "image.hpp"
#include "bin.hpp"
#include "box_layout.hpp"
#include "stack.hpp"
#include "grid.hpp"
#include "table.hpp"
#include "viewport.hpp"
#include "scrollview.hpp"
#include "kinetic_scrollview.hpp"

#include "stylable.hpp"

namespace JSDXToolkit {
 
	using namespace node;
	using namespace v8;

	Widget::Widget() : Actor() {
		hasClassName = FALSE;
	}

	void Widget::Initialize(Handle<Object> target)
	{
		HandleScope scope;

		Local<String> name = String::NewSymbol("Widget");

		/* Create a new class */
		Handle<ObjectTemplate> ObjectTpl = ObjectTemplate::New();
		ObjectTpl->SetInternalFieldCount(1);
		Local<Object> ClassObject = ObjectTpl->NewInstance();
		target->Set(name, ClassObject);

		/* Initializing Widget Class  */
		Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		/* Widgets */
		Style::Initialize(ClassObject);
		Bin::Initialize(ClassObject);
		BoxLayout::Initialize(ClassObject);
		Stack::Initialize(ClassObject);
		Grid::Initialize(ClassObject);
		Table::Initialize(ClassObject);
		Adjustment::Initialize(ClassObject);
		Scrollable::Initialize(ClassObject);
		//Stylable::Initialize(ClassObject);
		Button::Initialize(ClassObject);
		Entry::Initialize(ClassObject);
		Frame::Initialize(ClassObject);
		Label::Initialize(ClassObject);
		Dialog::Initialize(ClassObject);
		ProgressBar::Initialize(ClassObject);
		Slider::Initialize(ClassObject);
		Toggle::Initialize(ClassObject);
		Spinner::Initialize(ClassObject);
		Image::Initialize(ClassObject);
		Viewport::Initialize(ClassObject);
		ScrollView::Initialize(ClassObject);
		KineticScrollView::Initialize(ClassObject);

		/* Constants */
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "ORIENTATION_HORIZONTAL", JSDX_TOOLKIT_WIDGET_ORIENTATION_HORIZONTAL);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "ORIENTATION_VERTICAL", JSDX_TOOLKIT_WIDGET_ORIENTATION_VERTICAL);

		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "SCALE_MODE_NONE", MX_IMAGE_SCALE_NONE);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "SCALE_MODE_FIT", MX_IMAGE_SCALE_FIT);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "SCALE_MODE_CROP", MX_IMAGE_SCALE_CROP);

		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "KINETIC_SCROLL_VIEW_STATE_IDLE", MX_KINETIC_SCROLL_VIEW_STATE_IDLE);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "KINETIC_SCROLL_VIEW_STATE_PANNING", MX_KINETIC_SCROLL_VIEW_STATE_PANNING);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "KINETIC_SCROLL_VIEW_STATE_SCROLLING", MX_KINETIC_SCROLL_VIEW_STATE_SCROLLING);
		JSDX_TOOLKIT_DEFINE_CONSTANT(ClassObject, "KINETIC_SCROLL_VIEW_STATE_CLAMPING", MX_KINETIC_SCROLL_VIEW_STATE_CLAMPING);

		ClassObject->Set(name, tpl->GetFunction());
	}

	void Widget::PrototypeMethodsInit(Handle<FunctionTemplate> constructor_template)
	{
		HandleScope scope;

		Actor::PrototypeMethodsInit(constructor_template);

		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("disabled"), Widget::DisabledGetter, Widget::DisabledSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("className"), Widget::ClassNameGetter, Widget::ClassNameSetter);

		NODE_SET_PROTOTYPE_METHOD(constructor_template, "applyStyle", Widget::ApplyStyle);
	}

	/* ECMAScript constructor */
	Handle<Value> Widget::New(const Arguments& args)
	{
		HandleScope scope;

		if (!args.IsConstructCall()) {
			return ThrowException(Exception::TypeError(
				String::New("Use the new operator to create instances of this object."))
			);
		}

		// Creates a new instance object of this type and wraps it.
		Widget* obj = new Widget();
		obj->Wrap(args.This());

		return scope.Close(args.This());
	}

	Handle<Value> Widget::DisabledGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

		ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		return scope.Close(
			Boolean::New(mx_widget_get_disabled(MX_WIDGET(instance)))
		);
	}

	void Widget::DisabledSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsBoolean()) {
			ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

			mx_widget_set_disabled(MX_WIDGET(instance), value->ToBoolean()->Value());
		}
	}

	Handle<Value> Widget::ClassNameGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

		/* This widget has no class name */
		if (!ObjectWrap::Unwrap<Widget>(info.This())->hasClassName) {
			return scope.Close(
				String::New("")
			);
		}

		ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		return scope.Close(
			String::New(mx_stylable_get_style_class(MX_STYLABLE(instance)))
		);
	}

	void Widget::ClassNameSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsString()) {
			ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

			ObjectWrap::Unwrap<Widget>(info.This())->hasClassName = TRUE;
			mx_stylable_set_style_class(MX_STYLABLE(instance), *String::Utf8Value(value->ToString()));
			mx_stylable_set_style(MX_STYLABLE(instance), mx_style_get_default());
		}
	}

	/* Methods */
	Handle<Value> Widget::ApplyStyle(const Arguments& args)
	{
		HandleScope scope;

		if (args[0]->IsObject()) {
			ClutterActor *instance = ObjectWrap::Unwrap<Actor>(args.This())->_actor;
			MxStyle *style = ObjectWrap::Unwrap<Style>(args[0]->ToObject())->_style;

			mx_stylable_set_style(MX_STYLABLE(instance), MX_STYLE(style));
		}

		return args.This();
	}
}
