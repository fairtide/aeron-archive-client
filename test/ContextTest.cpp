/*
 * Copyright 2018 Fairtide Pte. Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <ChannelUri.h>
#include <Context.h>

using namespace aeron::archive;

TEST(ContextTest, shouldSetControlRequestChannel) {
    Configuration cfg;
    Context ctx(cfg);

    EXPECT_FALSE(ctx.controlRequestChannel().empty());

    ChannelUri uri = ChannelUri::parse(ctx.controlRequestChannel());
    EXPECT_TRUE(uri.get("term-length"));
    EXPECT_TRUE(uri.get("mtu"));
    EXPECT_TRUE(uri.get("sparse"));
}
