/*
 * JSDX Framework
 *
 * Copyright(c) 2012 Fred Chien <fred@mandice.com>
 *
 */

#include <v8.h>
#include <node.h>

#if USE_X11
#include <clutter/x11/clutter-x11.h>
#include "backend/x11.hpp"
#endif

#include <clutter/clutter.h>
#if ENABLE_MX
	#include <mx/mx.h>
#endif

#include "jsdx_toolkit.hpp"
#include "actor.hpp"
#ifndef ENABLE_MX
#include "container.hpp"
#endif
#include "stage.hpp"
#include "window.hpp"

#if ENABLE_MX
#include "widgets/style.hpp"
#endif

namespace JSDXToolkit {
 
	using namespace node;
	using namespace v8;

	Persistent<FunctionTemplate> JSDXWindow::constructor;

	JSDXWindow::JSDXWindow() : Stage()
	{
		/* Create a new stage */
		parent = NULL;
		x = -1;
		y = -1;
		width = -1;
		height = -1;
		windowType = JSDX_WINDOW_TYPE_NORMAL;
		_actor = clutter_stage_new();
		clutter_stage_set_title(CLUTTER_STAGE(_actor), "Default");

#if ENABLE_MX
		_window = mx_window_new_with_clutter_stage(CLUTTER_STAGE(_actor));

		/* No toolbar by default */
		mx_window_set_has_toolbar(MX_WINDOW(_window), FALSE);
#endif

#if USE_X11
		grabWindow = -1;
		hasDecorator = TRUE;
#endif
	}

	void JSDXWindow::Initialize(Handle<Object> target)
	{
		HandleScope scope;

		Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
		Local<String> name = String::NewSymbol("Window");

		constructor = Persistent<FunctionTemplate>::New(tpl);
		constructor->InstanceTemplate()->SetInternalFieldCount(1);
		constructor->SetClassName(name);

		/* Methods */
		JSDXWindow::PrototypeMethodsInit(constructor);

		target->Set(name, constructor->GetFunction());
	}

	void JSDXWindow::PrototypeMethodsInit(Handle<FunctionTemplate> constructor_template)
	{
		HandleScope scope;

		Stage::PrototypeMethodsInit(constructor_template);

		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("x"), JSDXWindow::XGetter, JSDXWindow::XSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("y"), JSDXWindow::YGetter, JSDXWindow::YSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("width"), JSDXWindow::WidthGetter, JSDXWindow::WidthSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("height"), JSDXWindow::HeightGetter, JSDXWindow::HeightSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("hasToolbar"), JSDXWindow::HasToolbarGetter, JSDXWindow::HasToolbarSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("rotation"), JSDXWindow::RotationGetter, JSDXWindow::RotationSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("hasDecorator"), JSDXWindow::HasDecoratorGetter, JSDXWindow::HasDecoratorSetter);
		constructor_template->InstanceTemplate()->SetAccessor(String::NewSymbol("windowType"), JSDXWindow::WindowTypeGetter, JSDXWindow::WindowTypeSetter);

		NODE_SET_PROTOTYPE_METHOD(constructor_template, "_setParentWindow", JSDXWindow::SetParentWindow);
		NODE_SET_PROTOTYPE_METHOD(constructor_template, "_setChild", JSDXWindow::SetChild);
		NODE_SET_PROTOTYPE_METHOD(constructor_template, "focus", JSDXWindow::Focus);
		NODE_SET_PROTOTYPE_METHOD(constructor_template, "show", JSDXWindow::Show);
		NODE_SET_PROTOTYPE_METHOD(constructor_template, "showAll", JSDXWindow::ShowAll);
		NODE_SET_PROTOTYPE_METHOD(constructor_template, "hide", JSDXWindow::Hide);
	}

	Local<Object> JSDXWindow::New(void)
	{
		Local<Object> ObjectInstance = JSDXWindow::constructor->GetFunction()->NewInstance();
		JSDXWindow* obj = new JSDXWindow();
		obj->Wrap(ObjectInstance);

		return ObjectInstance;
	}

	/* ECMAScript constructor */
	Handle<Value> JSDXWindow::New(const Arguments& args)
	{
		HandleScope scope;

		if (!args.IsConstructCall()) {
			return ThrowException(Exception::TypeError(
				String::New("Use the new operator to create instances of this object."))
			);
		}

		// Creates a new instance object of this type and wraps it.
		JSDXWindow* obj = new JSDXWindow();
		obj->Wrap(args.This());

		return scope.Close(args.This());
	}

	/* Accessors */
	Handle<Value> JSDXWindow::WidthGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());

#if ENABLE_MX
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
			gint width, height;
			mx_window_get_window_size(MX_WINDOW(window->_window), &width, &height);

			return scope.Close(Number::New(width));
		} else {
			return scope.Close(Number::New(window->width));
		}
#else
		return Actor::WidthGetter(name, info);
#endif
	}

	void JSDXWindow::WidthSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsNumber()) {
			JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

#if ENABLE_MX
			gint width, height;

			window->width = value->ToInteger()->Value();

			if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
				mx_window_get_window_size(MX_WINDOW(window->_window), &width, &height);

				if (window->width < 0)
					window->width = width;

				mx_window_set_window_size(MX_WINDOW(window->_window), window->width, window->height);
			}
#else
			Actor::WidthSetter(name, value, info);
#endif
		}
	}

	Handle<Value> JSDXWindow::HeightGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());

#if ENABLE_MX
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
			gint width, height;
			mx_window_get_window_size(MX_WINDOW(window->_window), &width, &height);

			return scope.Close(Number::New(height));
		} else {
			return scope.Close(Number::New(window->height));
		}
#else
		return Actor::HeightGetter(name, info);
#endif
	}

	void JSDXWindow::HeightSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsNumber()) {
			JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

#if ENABLE_MX
			gint width, height;

			window->height = value->ToInteger()->Value();

			if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
				mx_window_get_window_size(MX_WINDOW(window->_window), &width, &height);

				if (window->height < 0)
					window->height = height;

				mx_window_set_window_size(MX_WINDOW(window->_window), window->width, window->height);
			}
#else
			Actor::HeightSetter(name, value, info);
#endif
		}
	}

	Handle<Value> JSDXWindow::HasToolbarGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		MxWindow *instance = ObjectWrap::Unwrap<JSDXWindow>(info.This())->_window;

		return scope.Close(
			Boolean::New(mx_window_get_has_toolbar(MX_WINDOW(instance)))
		);
#else
		return scope.Close(Boolean::New(False));
#endif
	}

	void JSDXWindow::HasToolbarSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		if (value->IsBoolean()) {
			MxWindow *instance = ObjectWrap::Unwrap<JSDXWindow>(info.This())->_window;

			mx_window_set_has_toolbar(MX_WINDOW(instance), value->ToBoolean()->Value());
		}
#endif
	}

	Handle<Value> JSDXWindow::HasDecoratorGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

#if USE_X11
		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());

		return scope.Close(Boolean::New(window->hasDecorator));
#else
		return scope.Close(Boolean::New(True));
#endif

	}

	void JSDXWindow::HasDecoratorSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

#if USE_X11
		if (value->IsBoolean()) {
			JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

			window->hasDecorator = value->ToBoolean()->Value();

			/* If window was realized, take off its decorator now */
			if (clutter_actor_get_flags(actor) & CLUTTER_ACTOR_REALIZED) {
				Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
				Display *disp = clutter_x11_get_default_display();

				X11::setWindowDecorator(disp, w, window->hasDecorator);
			}
		}
#endif

	}

	Handle<Value> JSDXWindow::WindowTypeGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());

		return scope.Close(Integer::New(window->windowType));
	}

	void JSDXWindow::WindowTypeSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());

		if (value->IsNumber()) {

			window->windowType = (JSDXWindowType)value->ToInteger()->Value();

#if USE_X11
			/* If window was realized, change window type now */
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;
			if (clutter_actor_get_flags(actor) & CLUTTER_ACTOR_REALIZED) {
				Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
				Display *disp = clutter_x11_get_default_display();

				X11::setWindowType(disp, w, (X11::X11WindowType)window->windowType);
			}
#endif
		}
	}

	Handle<Value> JSDXWindow::RotationGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		MxWindow *instance = ObjectWrap::Unwrap<JSDXWindow>(info.This())->_window;

		switch(mx_window_get_window_rotation(MX_WINDOW(instance))) {
		case MX_WINDOW_ROTATION_0:
			return scope.Close(Integer::New(0));
			break;

		case MX_WINDOW_ROTATION_90:
			return scope.Close(Integer::New(90));
			break;

		case MX_WINDOW_ROTATION_180:
			return scope.Close(Integer::New(180));
			break;

		case MX_WINDOW_ROTATION_270:
			return scope.Close(Integer::New(270));
		}
#else
		return scope.Close(Integer::New(0));
#endif
	}

	void JSDXWindow::RotationSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		if (value->IsNumber()) {
			MxWindow *instance = ObjectWrap::Unwrap<JSDXWindow>(info.This())->_window;

			switch(value->ToInteger()->Value()) {
			case 0:
				mx_window_set_window_rotation(MX_WINDOW(instance), MX_WINDOW_ROTATION_0);
				break;

			case 90:
				mx_window_set_window_rotation(MX_WINDOW(instance), MX_WINDOW_ROTATION_90);
				break;

			case 180:
				mx_window_set_window_rotation(MX_WINDOW(instance), MX_WINDOW_ROTATION_180);
				break;

			case 270:
				mx_window_set_window_rotation(MX_WINDOW(instance), MX_WINDOW_ROTATION_270);
				break;
			}
		}
#endif
	}

	Handle<Value> JSDXWindow::XGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
		MxWindow *instance = window->_window;
		mx_window_get_window_position(instance, &(window->x), &(window->y));

		return scope.Close(
			Number::New(window->x)
		);
#else
		ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		return scope.Close(
			Number::New(clutter_actor_get_x(CLUTTER_ACTOR(instance)))
		);
#endif
	}

	Handle<Value> JSDXWindow::YGetter(Local<String> name, const AccessorInfo& info)
	{
		HandleScope scope;

#if ENABLE_MX
		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
		MxWindow *instance = window->_window;
		mx_window_get_window_position(instance, &(window->x), &(window->y));

		return scope.Close(
			Number::New(window->y)
		);
#else
		ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

		return scope.Close(
			Number::New(clutter_actor_get_y(CLUTTER_ACTOR(instance)))
		);
#endif
	}

	void JSDXWindow::XSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsNumber()) {
#if ENABLE_MX
			JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;
			MxWindow *instance = window->_window;
			gint x, y;

			window->x = value->ToInteger()->Value();

			if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
				mx_window_get_window_position(instance, &x, &y);

				if (window->y < 0)
					window->y = y;

				mx_window_set_window_position(instance, window->x, window->y);
			}
#else
			ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

			clutter_actor_set_x(CLUTTER_ACTOR(instance), value->NumberValue());
#endif
		}
	}

	void JSDXWindow::YSetter(Local<String> name, Local<Value> value, const AccessorInfo& info)
	{
		HandleScope scope;

		if (value->IsNumber()) {
#if ENABLE_MX
			JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(info.This());
			ClutterActor *actor = ObjectWrap::Unwrap<Actor>(info.This())->_actor;
			MxWindow *instance = window->_window;
			gint x, y;

			window->y = value->ToInteger()->Value();

			if (CLUTTER_ACTOR_IS_REALIZED(actor)) {
				mx_window_get_window_position(instance, &x, &y);

				if (window->x < 0)
					window->x = x;

				mx_window_set_window_position(instance, window->x, window->y);
			}
#else
			ClutterActor *instance = ObjectWrap::Unwrap<Actor>(info.This())->_actor;

			clutter_actor_set_y(CLUTTER_ACTOR(instance), value->NumberValue());
#endif
		}
	}

	/* Methods */
	Handle<Value> JSDXWindow::SetChild(const Arguments &args)
	{
		HandleScope scope;

		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(args[0]->ToObject())->_actor;

#if ENABLE_MX
		MxWindow *instance = ObjectWrap::Unwrap<JSDXWindow>(args.This())->_window;

		mx_window_set_child(MX_WINDOW(instance), CLUTTER_ACTOR(actor));
#else
		Container::Add(args);
#endif

		return args.This();
	}

	Handle<Value> JSDXWindow::SetParentWindow(const Arguments &args)
	{
		HandleScope scope;

#if USE_X11
/*
		ClutterActor *parent = ObjectWrap::Unwrap<Actor>(args[0]->ToObject())->_actor;
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(args.This())->_actor;
		Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
		Display *disp = clutter_x11_get_default_display();
*/
		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(args.This());
		window->parent = ObjectWrap::Unwrap<JSDXWindow>(args[0]->ToObject());

#endif

		return args.This();
	}

	Handle<Value> JSDXWindow::Focus(const Arguments &args)
	{
		HandleScope scope;

#if USE_X11
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(args.This())->_actor;
		Display *disp = clutter_x11_get_default_display();
		Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
/*
		ClutterActor *parent = ObjectWrap::Unwrap<Actor>(args[0]->ToObject())->_actor;
		Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
*/

		X11::setActive(disp, clutter_x11_get_root_window(), w, TRUE);

#endif

		return args.This();
	}

	Handle<Value> JSDXWindow::Show(const Arguments &args)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(args.This());
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(args.This())->_actor;

		clutter_actor_realize(actor);

#if USE_X11
		/* Set Window properties */
		Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
		Display *disp = clutter_x11_get_default_display();
		Window root = clutter_x11_get_root_window();

		X11::setWindowType(disp, w, (X11::X11WindowType)window->windowType);
		X11::reparentWindow(disp, w, clutter_x11_get_root_window());

		if (window->windowType == JSDX_WINDOW_TYPE_MENU || window->windowType == JSDX_WINDOW_TYPE_POPUP_MENU) {

			if (window->grabWindow == -1) {
				int i;
				XGetInputFocus(disp, &(window->grabWindow), &i);

				/*
				 * It will be failed if someone window grabs pointer already.
				 * So there is no need to worry about doing the same thing twice
				 */
				XGrabPointer(disp, window->grabWindow, TRUE, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

				/* Add a filter to get event that click to hide menu */
				clutter_x11_add_filter(JSDXWindow::MenuEventHandler, (gpointer)window);
			}
		}

		if (window->windowType != JSDX_WINDOW_TYPE_NORMAL)
			X11::windowConfigure(disp, clutter_x11_get_root_window(), w, (X11::X11WindowType)window->windowType);
#endif

#if ENABLE_MX
		/* Set position if it was changed before realized */
		MxWindow *instance = window->_window;
		gint x, y;

		mx_window_get_window_position(instance, &x, &y);
		if (window->x < 0)
			window->x = x;

		if (window->y < 0)
			window->y = y;

		gint width, height;

		mx_window_get_window_size(instance, &width, &height);
		if (window->width < 0)
			window->width = width;

		if (window->height < 0)
			window->height = height;
#else
		/* TODO: Using pure X11 solution to get window position */
#endif

#if ENABLE_MX
		mx_window_show(mx_window_get_for_stage(CLUTTER_STAGE(actor)));
#else
		clutter_actor_show(actor);
#endif

#if USE_X11
		if (window->windowType != JSDX_WINDOW_TYPE_MENU && window->windowType != JSDX_WINDOW_TYPE_POPUP_MENU) {
			X11::setWindowDecorator(disp, w, window->hasDecorator);
		}
#endif

#if ENABLE_MX
		mx_window_set_window_size(instance, window->width, window->height);
		mx_window_set_window_position(instance, window->x, window->y);
#else
		/* TODO: Using pure X11 solution to set window position */
#endif

		return args.This();
	}

	Handle<Value> JSDXWindow::ShowAll(const Arguments &args)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(args.This());
		ClutterActor *actor = ObjectWrap::Unwrap<Actor>(args.This())->_actor;

		clutter_actor_realize(actor);

#if USE_X11
		/* Set Window properties */
		Window w = clutter_x11_get_stage_window(CLUTTER_STAGE(actor));
		Display *disp = clutter_x11_get_default_display();
		Window root = clutter_x11_get_root_window();

		X11::setWindowType(disp, w, (X11::X11WindowType)window->windowType);
		X11::reparentWindow(disp, w, clutter_x11_get_root_window());

		if (window->windowType == JSDX_WINDOW_TYPE_MENU || window->windowType == JSDX_WINDOW_TYPE_POPUP_MENU) {

			if (window->grabWindow == -1) {
				int i;
				XGetInputFocus(disp, &(window->grabWindow), &i);

				/*
				 * It will be failed if someone window grabs pointer already.
				 * So there is no need to worry about doing the same thing twice
				 */
				XGrabPointer(disp, window->grabWindow, TRUE, ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

				/* Add a filter to get event that click to hide menu */
				clutter_x11_add_filter(JSDXWindow::MenuEventHandler, (gpointer)window);
			}
		}

		X11::windowConfigure(disp, clutter_x11_get_root_window(), w, (X11::X11WindowType)window->windowType);
#endif

#if ENABLE_MX
		/* Set position if it was changed before realized */
		MxWindow *instance = window->_window;
		gint x, y;

		mx_window_get_window_position(instance, &x, &y);
		if (window->x < 0)
			window->x = x;

		if (window->y < 0)
			window->y = y;

		gint width, height;

		mx_window_get_window_size(instance, &width, &height);
		if (window->width < 0)
			window->width = width;

		if (window->height < 0)
			window->height = height;
#else
		/* TODO: Using pure X11 solution to get window position */
#endif

		clutter_actor_show_all(actor);

#if USE_X11
		if (window->windowType != JSDX_WINDOW_TYPE_MENU && window->windowType != JSDX_WINDOW_TYPE_POPUP_MENU) {
			X11::setWindowDecorator(disp, w, window->hasDecorator);
		}
#endif

#if ENABLE_MX
		mx_window_set_window_position(instance, window->x, window->y);
		mx_window_set_window_size(instance, window->width, window->height);
#else
		/* TODO: Using pure X11 solution to set window position */
#endif

		return args.This();
	}

	Handle<Value> JSDXWindow::Hide(const Arguments &args)
	{
		HandleScope scope;

		JSDXWindow *window = ObjectWrap::Unwrap<JSDXWindow>(args.This());

		/* It grabs screen if window type is popup menu or menu */
		if (window->grabWindow != -1) {
			XUngrabPointer(clutter_x11_get_default_display(), CurrentTime);

			window->grabWindow = -1;

			clutter_x11_remove_filter(JSDXWindow::MenuEventHandler, window);
		}

		clutter_actor_hide(window->_actor);

		return args.This();
	}

	/* Event Handler */
#if USE_X11
	ClutterX11FilterReturn JSDXWindow::MenuEventHandler(XEvent *xev, ClutterEvent *cev, gpointer data)
	{
		JSDXWindow *window = (JSDXWindow *)data;

		if (xev->type == ButtonRelease && window->grabWindow == xev->xbutton.window) {

			XUngrabPointer(clutter_x11_get_default_display(), CurrentTime);

			window->grabWindow = -1;
			clutter_actor_hide(window->_actor);

			clutter_x11_remove_filter(JSDXWindow::MenuEventHandler, data);

			return CLUTTER_X11_FILTER_REMOVE;
		}

		return CLUTTER_X11_FILTER_CONTINUE;
	}
#endif
}
