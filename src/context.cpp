/*
 * JSDX Framework
 *
 * Copyright(c) 2012 Fred Chien <fred@mandice.com>
 *
 */

#include <v8.h>
#include <node.h>

#include "jsdx_toolkit.hpp"
#include "context.hpp"

namespace JSDXToolkit {
 
	using namespace node;
	using namespace v8;

	static struct gcontext g_context;
	static uv_prepare_t prepare_handle;
	static uv_check_t check_handle;

	Context::Context() {}

	void Context::Init()
	{
		GMainContext *gc = g_main_context_default();

		struct gcontext *ctx = &g_context;

		if (!g_thread_supported())
			g_thread_init(NULL);

		g_main_context_acquire(gc);
		ctx->gc = g_main_context_ref(gc);
		ctx->nfds = 0;
		ctx->fds = NULL;

		/* Prepare */
		uv_prepare_init(uv_default_loop(), &prepare_handle);
		uv_prepare_start(&prepare_handle, prepare_cb);

		/* Check */
		uv_check_init(uv_default_loop(), &check_handle);
		uv_check_start(&check_handle, check_cb);
	}

	void Context::Uninit()
	{
		struct gcontext *ctx = &g_context;

		uv_check_stop(&check_handle);
		uv_close((uv_handle_t *)&check_handle, NULL);

		uv_prepare_stop(&prepare_handle);
		uv_close((uv_handle_t *)&prepare_handle, NULL);

		g_main_context_unref(ctx->gc);
	}

	void Context::poll_cb(uv_poll_t *handle, int status, int events)
	{
		struct gcontext_pollfd *_pfd = (struct gcontext_pollfd *)handle->data;

		GPollFD *pfd = _pfd->pfd;

		pfd->revents |= pfd->events & ((events & UV_READABLE ? G_IO_IN : 0) | (events & UV_WRITABLE ? G_IO_OUT : 0));

		uv_poll_stop(handle);
		uv_close((uv_handle_t *)handle, NULL);
		delete _pfd;
	}

	void Context::prepare_cb(uv_prepare_t *handle, int status)
	{
		gint i;
		gint timeout;
		struct gcontext *ctx = &g_context;

		g_main_context_prepare(ctx->gc, &ctx->max_priority);

		/* Getting all sources from GLib main context */
		while((ctx->nfds = g_main_context_query(ctx->gc, ctx->max_priority, &timeout, ctx->fds, ctx->allocated_nfds)) > ctx->allocated_nfds) { 
			g_free(ctx->fds);
			g_free(ctx->poll_handles);

			ctx->allocated_nfds = ctx->nfds;

			ctx->fds = g_new(GPollFD, ctx->nfds);
			ctx->poll_handles = g_new(uv_poll_t, ctx->nfds);
		}

		/* Poll */
		for (i = 0; i < ctx->nfds; ++i) {
			GPollFD *pfd = ctx->fds + i;
			uv_poll_t *pt = ctx->poll_handles + i;

			struct gcontext_pollfd *data = new gcontext_pollfd;
			data->pfd = pfd;
			pt->data = data;

			pfd->revents = 0;

			uv_poll_init(uv_default_loop(), pt, pfd->fd);
			uv_poll_start(pt, UV_READABLE | UV_WRITABLE, poll_cb);
		}
	}

	void Context::check_cb(uv_check_t *handle, int status)
	{
		gint i;
		struct gcontext *ctx = &g_context;

		/* Release all polling events which aren't finished yet. */
		for (i = 0; i < ctx->nfds; ++i) {
			GPollFD *pfd = ctx->fds + i;
			uv_poll_t *pt = ctx->poll_handles + i;

			if (uv_is_active((uv_handle_t *)pt)) {
				uv_poll_stop(pt);
				uv_close((uv_handle_t *)pt, NULL);
				delete (struct gcontext_pollfd *)pt->data;
			}
		}

		g_main_context_check(ctx->gc, ctx->max_priority, ctx->fds, ctx->nfds);
		g_main_context_dispatch(ctx->gc);
	}
}
