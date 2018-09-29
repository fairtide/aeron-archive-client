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
#include <util/Exceptions.h>

#include <unordered_map>

using namespace aeron::archive;

namespace {

void assertParseWithParams(const std::string& uriStr, std::unordered_map<std::string, std::string>&& params) {
    ChannelUri uri = ChannelUri::parse(uriStr);

    for (auto p : params) {
        auto v = uri.get(p.first);
        ASSERT_TRUE(v) << "missing key: " << p.first;
        EXPECT_EQ(*v, p.second);
    }
}

void assertParseWithMediaAndPrefix(const std::string& uriStr, const std::string& prefix, const std::string& media) {
    ChannelUri uri = ChannelUri::parse(uriStr);

    EXPECT_EQ(uri.scheme(), ChannelUri::AERON_SCHEME);
    EXPECT_EQ(uri.prefix(), prefix);
    EXPECT_EQ(uri.media(), media);
}

void assertParseWithMedia(const std::string& uri, const std::string& media) {
    assertParseWithMediaAndPrefix(uri, "", media);
}

void assertInvalid(const std::string& str) {
    EXPECT_THROW({ ChannelUri::parse(str); }, aeron::util::IllegalArgumentException);
}

}  // namespace

TEST(ChannelUriTest, shouldParseSimpleDefaultUri) {
    assertParseWithMedia("aeron:udp", "udp");
    assertParseWithMedia("aeron:ipc", "ipc");
    assertParseWithMedia("aeron:", "");
    assertParseWithMediaAndPrefix("aeron-spy:aeron:ipc", "aeron-spy", "ipc");
}

TEST(ChannelUriTest, shouldRejectUriWithoutAeronPrefix) {
    assertInvalid(":udp");
    assertInvalid("aeron");
    assertInvalid("aron:");
    assertInvalid("eeron:");
}

TEST(ChannelUriTest, ShouldRejectUriWithOutOfPlaceColon) { assertInvalid("aeron:udp:"); }

TEST(ChannelUriTest, shouldParseWithSingleParameter) {
    assertParseWithParams("aeron:udp?endpoint=224.10.9.8", {{"endpoint", "224.10.9.8"}});
    assertParseWithParams("aeron:udp?add|ress=224.10.9.8", {{"add|ress", "224.10.9.8"}});
    assertParseWithParams("aeron:udp?endpoint=224.1=0.9.8", {{"endpoint", "224.1=0.9.8"}});
}

TEST(ChannelUriTest, shouldParseWithMultipleParameters) {
    assertParseWithParams("aeron:udp?endpoint=224.10.9.8|port=4567|interface=192.168.0.3|ttl=16",
                          {{"endpoint", "224.10.9.8"}, {"port", "4567"}, {"interface", "192.168.0.3"}, {"ttl", "16"}});
}

TEST(ChannelUriTest, shouldAllowReturnDefaultIfParamNotSpecified) {
    ChannelUri uri = ChannelUri::parse("aeron:udp?endpoint=224.10.9.8");

    EXPECT_FALSE(uri.get("interface"));

    auto v = uri.get("interface", "192.168.0.0");
    EXPECT_TRUE(v);
    EXPECT_EQ(*v, "192.168.0.0");
}

TEST(ChannelUriTest, shouldRoundTripToString) {
    std::string uriString{"aeron:udp?endpoint=224.10.9.8:777"};
    ChannelUri uri = ChannelUri::parse(uriString);

    EXPECT_EQ(uri.toString(), uriString);
}

// TODO: ChannelUriStringBuilder is not used in the archive client, so these test have not been implemented yet
// TEST(ChannelUriTest, shouldRoundTripToStringBuilder) { FAIL() << "Not implemented"; }
// TEST(ChannelUriTest, shouldRoundTripToStringBuilderWithPrefix) { FAIL() << "Not implemented"; }
