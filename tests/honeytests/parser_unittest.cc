// Copyright (c) 2011 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/honey_parser.h"
#include "tests/gtest/include/gtest/gtest.h"

// Create the URL using the spec.
TEST(ParserTest, CreateURLSpec) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.spec)
      .FromASCII(
          "http://user:pass@www.example.com:88/path/"
          "to.html?foo=test&bar=test2");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2",
      url.ToString().c_str());
}

// Test that host is required.
TEST(ParserTest, CreateURLHostRequired) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  EXPECT_FALSE(HoneycombCreateURL(parts, url));
}

// Test that scheme is required.
TEST(ParserTest, CreateURLSchemeRequired) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.host).FromASCII("www.example.com");
  EXPECT_FALSE(HoneycombCreateURL(parts, url));
}

// Create the URL using scheme and host.
TEST(ParserTest, CreateURLSchemeHost) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  HoneycombString(&parts.host).FromASCII("www.example.com");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/", url.ToString().c_str());
}

// Create the URL using scheme, host and path.
TEST(ParserTest, CreateURLSchemeHostPath) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  HoneycombString(&parts.host).FromASCII("www.example.com");
  HoneycombString(&parts.path).FromASCII("/path/to.html");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/path/to.html", url.ToString().c_str());
}

// Create the URL using scheme, host, path and query.
TEST(ParserTest, CreateURLSchemeHostPathQuery) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  HoneycombString(&parts.host).FromASCII("www.example.com");
  HoneycombString(&parts.path).FromASCII("/path/to.html");
  HoneycombString(&parts.query).FromASCII("foo=test&bar=test2");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2",
               url.ToString().c_str());
}

// Create the URL using scheme, host, path, query and Fragment
TEST(ParserTest, CreateURLSchemeHostPathQueryFragment) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  HoneycombString(&parts.host).FromASCII("www.example.com");
  HoneycombString(&parts.path).FromASCII("/path/to.html");
  HoneycombString(&parts.query).FromASCII("foo=test&bar=test2");
  HoneycombString(&parts.fragment).FromASCII("ref");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2#ref",
               url.ToString().c_str());
}

// Create the URL using all the various components.
TEST(ParserTest, CreateURLAll) {
  HoneycombURLParts parts;
  HoneycombString url;
  HoneycombString(&parts.scheme).FromASCII("http");
  HoneycombString(&parts.username).FromASCII("user");
  HoneycombString(&parts.password).FromASCII("pass");
  HoneycombString(&parts.host).FromASCII("www.example.com");
  HoneycombString(&parts.port).FromASCII("88");
  HoneycombString(&parts.path).FromASCII("/path/to.html");
  HoneycombString(&parts.query).FromASCII("foo=test&bar=test2");
  HoneycombString(&parts.fragment).FromASCII("ref");
  EXPECT_TRUE(HoneycombCreateURL(parts, url));
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2#ref",
      url.ToString().c_str());
}

// Parse the URL using scheme and host.
TEST(ParserTest, ParseURLSchemeHost) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("http://www.example.com");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/", spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  HoneycombString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  HoneycombString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  HoneycombString path(&parts.path);
  EXPECT_STREQ("/", path.ToString().c_str());
  EXPECT_EQ(0U, parts.query.length);
}

// Parse the URL using scheme, host and path.
TEST(ParserTest, ParseURLSchemeHostPath) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("http://www.example.com/path/to.html");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/path/to.html", spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  HoneycombString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  HoneycombString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  HoneycombString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  EXPECT_EQ(0U, parts.query.length);
}

// Parse the URL using scheme, host, path and query.
TEST(ParserTest, ParseURLSchemeHostPathQuery) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("http://www.example.com/path/to.html?foo=test&bar=test2");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2",
               spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  HoneycombString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  HoneycombString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  HoneycombString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  HoneycombString query(&parts.query);
  EXPECT_STREQ("foo=test&bar=test2", query.ToString().c_str());
}

// Parse the URL using scheme, host, path, query and fragment.
TEST(ParserTest, ParseURLSchemeHostPathQueryFragment) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("http://www.example.com/path/to.html?foo=test&bar=test2#ref");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ("http://www.example.com/path/to.html?foo=test&bar=test2#ref",
               spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  HoneycombString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  EXPECT_EQ(0U, parts.port.length);
  HoneycombString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com/");
  HoneycombString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  HoneycombString query(&parts.query);
  EXPECT_STREQ("foo=test&bar=test2", query.ToString().c_str());
  HoneycombString ref(&parts.fragment);
  EXPECT_STREQ("ref", ref.ToString().c_str());
}

// Parse the URL using all the various components.
TEST(ParserTest, ParseURLAll) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII(
      "http://user:pass@www.example.com:88/path/"
      "to.html?foo=test&bar=test2#ref");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ(
      "http://user:pass@www.example.com:88/path/to.html?foo=test&bar=test2#ref",
      spec.ToString().c_str());
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("http", scheme.ToString().c_str());
  HoneycombString username(&parts.username);
  EXPECT_STREQ("user", username.ToString().c_str());
  HoneycombString password(&parts.password);
  EXPECT_STREQ("pass", password.ToString().c_str());
  HoneycombString host(&parts.host);
  EXPECT_STREQ("www.example.com", host.ToString().c_str());
  HoneycombString port(&parts.port);
  EXPECT_STREQ("88", port.ToString().c_str());
  HoneycombString origin(&parts.origin);
  EXPECT_STREQ(origin.ToString().c_str(), "http://www.example.com:88/");
  HoneycombString path(&parts.path);
  EXPECT_STREQ("/path/to.html", path.ToString().c_str());
  HoneycombString query(&parts.query);
  EXPECT_STREQ("foo=test&bar=test2", query.ToString().c_str());
  HoneycombString ref(&parts.fragment);
  EXPECT_STREQ("ref", ref.ToString().c_str());
}

// Parse an invalid URL.
TEST(ParserTest, ParseURLInvalid) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("www.example.com");
  EXPECT_FALSE(HoneycombParseURL(url, parts));
}

// Parse a non-standard scheme.
TEST(ParserTest, ParseURLNonStandard) {
  HoneycombURLParts parts;
  HoneycombString url;
  url.FromASCII("custom:something%20else?foo#ref");
  EXPECT_TRUE(HoneycombParseURL(url, parts));

  HoneycombString spec(&parts.spec);
  EXPECT_STREQ("custom:something%20else?foo#ref", spec.ToString().c_str());
  EXPECT_EQ(0U, parts.username.length);
  EXPECT_EQ(0U, parts.password.length);
  HoneycombString scheme(&parts.scheme);
  EXPECT_STREQ("custom", scheme.ToString().c_str());
  EXPECT_EQ(0U, parts.host.length);
  EXPECT_EQ(0U, parts.port.length);
  EXPECT_EQ(0U, parts.origin.length);
  HoneycombString path(&parts.path);
  EXPECT_STREQ("something%20else", path.ToString().c_str());
  HoneycombString query(&parts.query);
  EXPECT_STREQ("foo", query.ToString().c_str());
  HoneycombString ref(&parts.fragment);
  EXPECT_STREQ("ref", ref.ToString().c_str());
}

// Combine and parse an absolute and relative URL.
TEST(ParserTest, ParseAbsoluteAndRelativeURL) {
  HoneycombString base_url;
  base_url.FromASCII("https://www.example.com");
  HoneycombString relative_url;
  relative_url.FromASCII("/example");
  HoneycombString resolved_url;
  EXPECT_TRUE(HoneycombResolveURL(base_url, relative_url, resolved_url));
  EXPECT_STREQ("https://www.example.com/example",
               resolved_url.ToString().c_str());
}

TEST(ParserTest, FormatUrlForSecurityDisplay) {
  HoneycombString result;

  // Omits the protocol if it's standard.
  result = HoneycombFormatUrlForSecurityDisplay("http://tests.com/foo.html");
  EXPECT_STREQ("http://tests.com", result.ToString().c_str());

  // Omits the port if it's the expected value for the protocol.
  result = HoneycombFormatUrlForSecurityDisplay("http://tests.com:80/foo.html");
  EXPECT_STREQ("http://tests.com", result.ToString().c_str());

  // Don't omit non-standard ports.
  result = HoneycombFormatUrlForSecurityDisplay("http://tests.com:8088/foo.html");
  EXPECT_STREQ("http://tests.com:8088", result.ToString().c_str());

  // Don't omit the protocol for file URLs.
  result = HoneycombFormatUrlForSecurityDisplay("file:///c/tests/foo.html");
  EXPECT_STREQ("file:///c/tests/foo.html", result.ToString().c_str());
}

TEST(ParserTest, GetMimeType) {
  HoneycombString mime_type;

  mime_type = HoneycombGetMimeType("html");
  EXPECT_STREQ("text/html", mime_type.ToString().c_str());

  mime_type = HoneycombGetMimeType("txt");
  EXPECT_STREQ("text/plain", mime_type.ToString().c_str());

  mime_type = HoneycombGetMimeType("gif");
  EXPECT_STREQ("image/gif", mime_type.ToString().c_str());
}

TEST(ParserTest, Base64Encode) {
  const std::string& test_str_decoded = "A test string";
  const std::string& test_str_encoded = "QSB0ZXN0IHN0cmluZw==";
  const HoneycombString& encoded_value =
      HoneycombBase64Encode(test_str_decoded.data(), test_str_decoded.size());
  EXPECT_STREQ(test_str_encoded.c_str(), encoded_value.ToString().c_str());
}

TEST(ParserTest, Base64Decode) {
  const std::string& test_str_decoded = "A test string";
  const std::string& test_str_encoded = "QSB0ZXN0IHN0cmluZw==";
  HoneycombRefPtr<HoneycombBinaryValue> decoded_value = HoneycombBase64Decode(test_str_encoded);
  EXPECT_TRUE(decoded_value.get());

  const size_t decoded_size = decoded_value->GetSize();
  EXPECT_EQ(test_str_decoded.size(), decoded_size);

  std::string decoded_str;
  decoded_str.resize(decoded_size + 1);  // Include space for NUL-terminator.
  const size_t get_data_result = decoded_value->GetData(
      const_cast<char*>(decoded_str.data()), decoded_size, 0);
  EXPECT_EQ(decoded_size, get_data_result);
  EXPECT_STREQ(test_str_decoded.c_str(), decoded_str.c_str());
}

TEST(ParserTest, URIEncode) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A%20test%20string%3D";
  const HoneycombString& encoded_value = HoneycombURIEncode(test_str_decoded, false);
  EXPECT_STREQ(test_str_encoded.c_str(), encoded_value.ToString().c_str());
}

TEST(ParserTest, URIEncodeWithPlusSpace) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A+test+string%3D";
  const HoneycombString& encoded_value = HoneycombURIEncode(test_str_decoded, true);
  EXPECT_STREQ(test_str_encoded.c_str(), encoded_value.ToString().c_str());
}

TEST(ParserTest, URIDecode) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A%20test%20string%3D";
  const HoneycombString& decoded_value = HoneycombURIDecode(
      test_str_encoded, false,
      static_cast<honey_uri_unescape_rule_t>(
          UU_SPACES | UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS));
  EXPECT_STREQ(test_str_decoded.c_str(), decoded_value.ToString().c_str());
}

TEST(ParserTest, URIDecodeWithPlusSpace) {
  const std::string& test_str_decoded = "A test string=";
  const std::string& test_str_encoded = "A+test+string%3D";
  const HoneycombString& decoded_value =
      HoneycombURIDecode(test_str_encoded, false,
                   static_cast<honey_uri_unescape_rule_t>(
                       UU_SPACES | UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS |
                       UU_REPLACE_PLUS_WITH_SPACE));
  EXPECT_STREQ(test_str_decoded.c_str(), decoded_value.ToString().c_str());
}

TEST(ParserTest, ParseJSONInvalid) {
  const char data[] = "This is my test data";
  HoneycombRefPtr<HoneycombValue> value = HoneycombParseJSON(data, JSON_PARSER_RFC);
  EXPECT_FALSE(value.get());
}

TEST(ParserTest, ParseJSONNull) {
  const char data[] = "{\"key1\":null}";
  HoneycombRefPtr<HoneycombValue> value = HoneycombParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value->GetType() == VTYPE_DICTIONARY);
  EXPECT_FALSE(value->IsOwned());
  HoneycombRefPtr<HoneycombDictionaryValue> dict = value->GetDictionary();
  HoneycombDictionaryValue::KeyList key_list;
  EXPECT_TRUE(dict->GetKeys(key_list));
  EXPECT_EQ(1U, key_list.size());
  EXPECT_EQ("key1", key_list[0].ToString());
  EXPECT_EQ(VTYPE_NULL, dict->GetType("key1"));

  // generate string from parsed result
  HoneycombString result = HoneycombWriteJSON(value, JSON_WRITER_DEFAULT);
  HoneycombString expected_result = data;
  EXPECT_EQ(expected_result, result);
}

TEST(ParserTest, WriteJSONBinary) {
  const char data[] = "\00\01\02";
  HoneycombRefPtr<HoneycombDictionaryValue> dict = HoneycombDictionaryValue::Create();
  HoneycombRefPtr<HoneycombBinaryValue> binary = HoneycombBinaryValue::Create(data, sizeof(data));
  dict->SetBinary("key1", binary);
  HoneycombRefPtr<HoneycombValue> node = HoneycombValue::Create();
  node->SetDictionary(dict);
  HoneycombString result = HoneycombWriteJSON(node, JSON_WRITER_DEFAULT);
  HoneycombString expect_result = "";
  // binary data will be omitted.
  EXPECT_EQ(expect_result, result);
}

TEST(ParserTest, ParseJSONDictionary) {
  const char data[] = "{\"key1\":\"value1\",\"key2\":123,\"key3\":[1,2,3]}";
  HoneycombRefPtr<HoneycombValue> value = HoneycombParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_FALSE(value->IsOwned());
  EXPECT_TRUE(value->GetType() == VTYPE_DICTIONARY);
  HoneycombRefPtr<HoneycombDictionaryValue> dict = value->GetDictionary();
  HoneycombDictionaryValue::KeyList key_list;
  EXPECT_TRUE(dict->GetKeys(key_list));
  EXPECT_EQ(3U, key_list.size());
  EXPECT_EQ("key1", key_list[0].ToString());
  EXPECT_EQ("key2", key_list[1].ToString());
  EXPECT_EQ("key3", key_list[2].ToString());
  EXPECT_EQ(VTYPE_STRING, dict->GetType("key1"));
  EXPECT_EQ(dict->GetString("key1"), "value1");
  EXPECT_EQ(VTYPE_INT, dict->GetType("key2"));
  EXPECT_EQ(123, dict->GetInt("key2"));
  EXPECT_EQ(VTYPE_LIST, dict->GetType("key3"));
  HoneycombRefPtr<HoneycombListValue> key3 = dict->GetList("key3");
  EXPECT_TRUE(nullptr != key3);
  EXPECT_TRUE(key3->IsValid());
  EXPECT_EQ(3U, key3->GetSize());
  EXPECT_EQ(1, key3->GetInt(0));
  EXPECT_EQ(2, key3->GetInt(1));
  EXPECT_EQ(3, key3->GetInt(2));

  // generate string from parsed result
  HoneycombString result = HoneycombWriteJSON(value, JSON_WRITER_DEFAULT);
  HoneycombString expected_result = data;
  EXPECT_EQ(expected_result, result);
}

TEST(ParserTest, ParseJSONList) {
  const char data[] = "[\"value1\", 123, {\"key3\": [1, 2, 3]}]";
  HoneycombRefPtr<HoneycombValue> value = HoneycombParseJSON(data, JSON_PARSER_RFC);
  EXPECT_TRUE(value.get());
  EXPECT_TRUE(value->IsValid());
  EXPECT_TRUE(value->GetType() == VTYPE_LIST);
  EXPECT_FALSE(value->IsOwned());
  HoneycombRefPtr<HoneycombListValue> list = value->GetList();
  EXPECT_TRUE(nullptr != list);
  EXPECT_TRUE(list->IsValid());
  EXPECT_EQ(3U, list->GetSize());

  EXPECT_EQ(VTYPE_STRING, list->GetType(0));
  EXPECT_EQ(list->GetString(0), "value1");
  EXPECT_EQ(VTYPE_INT, list->GetType(1));
  EXPECT_EQ(123, list->GetInt(1));
  EXPECT_EQ(VTYPE_DICTIONARY, list->GetType(2));
  HoneycombRefPtr<HoneycombDictionaryValue> dict = list->GetDictionary(2);
  HoneycombDictionaryValue::KeyList key_list2;
  EXPECT_TRUE(dict->GetKeys(key_list2));
  EXPECT_EQ(1U, key_list2.size());
  HoneycombRefPtr<HoneycombListValue> list2 = dict->GetList("key3");
  EXPECT_EQ(3U, list2->GetSize());
  EXPECT_EQ(1, list2->GetInt(0));
  EXPECT_EQ(2, list2->GetInt(1));
  EXPECT_EQ(3, list2->GetInt(2));

  // generate string from parsed result
  HoneycombString result = HoneycombWriteJSON(value, JSON_WRITER_DEFAULT);
  HoneycombString expected_result = "[\"value1\",123,{\"key3\":[1,2,3]}]";
  EXPECT_EQ(expected_result.ToString(), result.ToString());
}

TEST(ParserTest, ParseJSONAndReturnErrorInvalid) {
  const char data[] = "This is my test data";
  HoneycombString error_msg;
  HoneycombRefPtr<HoneycombValue> value =
      HoneycombParseJSONAndReturnError(data, JSON_PARSER_RFC, error_msg);
  HoneycombString expect_error_msg = "Line: 1, column: 1, Unexpected token.";
  EXPECT_FALSE(value.get());
  EXPECT_EQ(expect_error_msg, error_msg);
}

TEST(ParserTest, ParseJSONAndReturnErrorTrailingComma) {
  const char data[] = "{\"key1\":123,}";
  HoneycombString error_msg;
  HoneycombRefPtr<HoneycombValue> value =
      HoneycombParseJSONAndReturnError(data, JSON_PARSER_RFC, error_msg);
  HoneycombString expect_error_msg =
      "Line: 1, column: 13, Trailing comma not allowed.";
  EXPECT_FALSE(value.get());
  EXPECT_EQ(expect_error_msg, error_msg);
}
