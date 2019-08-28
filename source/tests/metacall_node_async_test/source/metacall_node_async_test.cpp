/*
 *	MetaCall Library by Parra Studios
 *	A library for providing a foreign function interface calls.
 *
 *	Copyright (C) 2016 - 2019 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 *
 */

#include <gmock/gmock.h>

#include <metacall/metacall.h>
#include <metacall/metacall_value.h>
#include <metacall/metacall_loaders.h>

class metacall_node_async_test : public testing::Test
{
public:
};

TEST_F(metacall_node_async_test, DefaultConstructor)
{
	metacall_print_info();

	ASSERT_EQ((int) 0, (int) metacall_initialize());

	/* NodeJS */
	#if defined(OPTION_BUILD_LOADERS_NODE)
	{
		const char buffer[] =
			"const util = require('util');\n"
			"function f(x) {\n"
			"\treturn new Promise(r => console.log(`Promise executed: ${util.inspect(r)} -> ${x}`) || r(x));\n"
			"}\n"
			"function g(x) {\n"
			"\treturn new Promise((_, r) => console.log(`Promise executed: ${util.inspect(r)} -> ${x}`) || r(x));\n"
			"}\n"
			"module.exports = { f, g };\n";

		EXPECT_EQ((int) 0, (int) metacall_load_from_memory("node", buffer, sizeof(buffer), NULL));

		void * args[] =
		{
			metacall_value_create_double(10.0)
		};

		struct async_context
		{
			int value;
		} ctx = {
			234
		};

		/* Test resolve */
		void * future = metacall_async("f", args);

		EXPECT_NE((void *) NULL, (void *) future);

		EXPECT_EQ((enum metacall_value_id) metacall_value_id(future), (enum metacall_value_id) METACALL_FUTURE);

		void * ret = metacall_await(future, [](void * result, void * data) -> void * {
			EXPECT_NE((void *) NULL, (void *) result);

			EXPECT_EQ((enum metacall_value_id) metacall_value_id(result), (enum metacall_value_id) METACALL_DOUBLE);

			EXPECT_EQ((double) 10.0, (double) metacall_value_to_double(result));

			EXPECT_NE((void *) NULL, (void *) data);

			struct async_context * ctx = static_cast<struct async_context *>(data);

			EXPECT_EQ((int) 234, (int) ctx->value);

			printf("Resolve C Callback\n");

			return metacall_value_create_double(15.0);
		}, [](void *, void *) -> void * {
			int this_should_never_be_executed = 0;

			EXPECT_EQ((int) 1, (int) this_should_never_be_executed);

			printf("Reject C Callback\n");

			return NULL;
		}, static_cast<void *>(&ctx));

		metacall_value_destroy(future);

		EXPECT_NE((void *) NULL, (void *) ret);

		EXPECT_EQ((enum metacall_value_id) metacall_value_id(ret), (enum metacall_value_id) METACALL_FUTURE);

		metacall_value_destroy(ret);

		/* Test reject */
		future = metacall_async("g", args);

		EXPECT_NE((void *) NULL, (void *) future);

		EXPECT_EQ((enum metacall_value_id) metacall_value_id(future), (enum metacall_value_id) METACALL_FUTURE);

		ret = metacall_await(future, [](void *, void *) -> void * {
			int this_should_never_be_executed = 0;

			EXPECT_EQ((int) 1, (int) this_should_never_be_executed);

			printf("Resolve C Callback\n");

			return NULL;
		}, [](void * result, void * data) -> void * {
			EXPECT_NE((void *) NULL, (void *) result);

			EXPECT_EQ((enum metacall_value_id) metacall_value_id(result), (enum metacall_value_id) METACALL_DOUBLE);

			EXPECT_EQ((double) 10.0, (double) metacall_value_to_double(result));

			EXPECT_NE((void *) NULL, (void *) data);

			struct async_context * ctx = static_cast<struct async_context *>(data);

			EXPECT_EQ((int) 234, (int) ctx->value);

			printf("Reject C Callback\n");

			return metacall_value_create_double(15.0);
		}, static_cast<void *>(&ctx));

		metacall_value_destroy(future);

		EXPECT_NE((void *) NULL, (void *) ret);

		EXPECT_EQ((enum metacall_value_id) metacall_value_id(ret), (enum metacall_value_id) METACALL_FUTURE);

		metacall_value_destroy(ret);

		metacall_value_destroy(args[0]);
	}
	#endif /* OPTION_BUILD_LOADERS_NODE */

	EXPECT_EQ((int) 0, (int) metacall_destroy());
}
