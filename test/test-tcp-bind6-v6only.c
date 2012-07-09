/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>


static int close_cb_called = 0;


static void close_cb(uv_handle_t* handle) {
  ASSERT(handle != NULL);
  close_cb_called++;
}


TEST_IMPL(tcp_bind6_v6only_error_addrinuse) {
  struct sockaddr_in6 addr6 = uv_ip6_addr("::", TEST_PORT);
  struct sockaddr_in addr4 = uv_ip4_addr("0.0.0.0", TEST_PORT);
  uv_tcp_t server1, server2;
  int r;

  r = uv_tcp_init(uv_default_loop(), &server1);
  ASSERT(r == 0);
  r = uv_tcp_v6only(&server1, 0);
  ASSERT(r == 0);
  r = uv_tcp_bind6(&server1, addr6);
  ASSERT(r == 0);

  r = uv_tcp_init(uv_default_loop(), &server2);
  ASSERT(r == 0);
  r = uv_tcp_bind(&server2, addr4);
  ASSERT(r == 0);

  r = uv_listen((uv_stream_t*)&server1, 128, NULL);
  ASSERT(r == 0);
  r = uv_listen((uv_stream_t*)&server2, 128, NULL);
  ASSERT(r == -1);

  ASSERT(uv_last_error(uv_default_loop()).code == UV_EADDRINUSE);

  uv_close((uv_handle_t*)&server1, close_cb);
  uv_close((uv_handle_t*)&server2, close_cb);

  uv_run(uv_default_loop());

  ASSERT(close_cb_called == 2);

  return 0;
}


TEST_IMPL(tcp_bind6_v6only_error_inval) {
  struct sockaddr_in6 addr6 = uv_ip6_addr("::", TEST_PORT);
  uv_tcp_t server1;
  int r;

  r = uv_tcp_init(uv_default_loop(), &server1);
  ASSERT(r == 0);
  r = uv_tcp_v6only(&server1, 0);
  ASSERT(r == 0);
  r = uv_tcp_bind6(&server1, addr6);
  ASSERT(r == 0);
  r = uv_tcp_v6only(&server1, 1);
  ASSERT(r == -1);

  uv_close((uv_handle_t*)&server1, close_cb);

  uv_run(uv_default_loop());

  ASSERT(close_cb_called == 1);

  return 0;
}

TEST_IMPL(tcp_bind6_v6only_ok) {
  struct sockaddr_in6 addr6 = uv_ip6_addr("::", TEST_PORT);
  struct sockaddr_in addr4 = uv_ip4_addr("0.0.0.0", TEST_PORT);
  uv_tcp_t server1, server2;
  int r;

  r = uv_tcp_init(uv_default_loop(), &server1);
  ASSERT(r == 0);
  r = uv_tcp_v6only(&server1, 1);
  ASSERT(r == 0);
  r = uv_tcp_bind6(&server1, addr6);
  ASSERT(r == 0);

  r = uv_tcp_init(uv_default_loop(), &server2);
  ASSERT(r == 0);
  r = uv_tcp_bind(&server2, addr4);
  ASSERT(r == 0);

  r = uv_listen((uv_stream_t*)&server1, 128, NULL);
  ASSERT(r == 0);
  r = uv_listen((uv_stream_t*)&server2, 128, NULL);
  ASSERT(r == 0);

  uv_close((uv_handle_t*)&server1, close_cb);
  uv_close((uv_handle_t*)&server2, close_cb);

  uv_run(uv_default_loop());

  ASSERT(close_cb_called == 2);

  return 0;
}
