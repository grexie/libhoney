// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <sstream>

#include "include/honey_parser.h"

#include "base/base64.h"
#include "base/strings/escape.h"
#include "base/threading/thread_restrictions.h"
#include "components/url_formatter/elide_url.h"
#include "net/base/mime_util.h"
#include "url/gurl.h"

bool HoneycombResolveURL(const HoneycombString& base_url,
                   const HoneycombString& relative_url,
                   HoneycombString& resolved_url) {
  GURL base_gurl(base_url.ToString());
  if (!base_gurl.is_valid()) {
    return false;
  }

  GURL combined_gurl = base_gurl.Resolve(relative_url.ToString());
  if (!combined_gurl.is_valid()) {
    return false;
  }

  resolved_url = combined_gurl.spec();
  return true;
}

bool HoneycombParseURL(const HoneycombString& url, HoneycombURLParts& parts) {
  GURL gurl(url.ToString());
  if (!gurl.is_valid()) {
    return false;
  }

  HoneycombString(&parts.spec).FromString(gurl.spec());
  HoneycombString(&parts.scheme).FromString(gurl.scheme());
  HoneycombString(&parts.username).FromString(gurl.username());
  HoneycombString(&parts.password).FromString(gurl.password());
  HoneycombString(&parts.host).FromString(gurl.host());
  HoneycombString(&parts.origin).FromString(gurl.DeprecatedGetOriginAsURL().spec());
  HoneycombString(&parts.port).FromString(gurl.port());
  HoneycombString(&parts.path).FromString(gurl.path());
  HoneycombString(&parts.query).FromString(gurl.query());
  HoneycombString(&parts.fragment).FromString(gurl.ref());

  return true;
}

bool HoneycombCreateURL(const HoneycombURLParts& parts, HoneycombString& url) {
  std::string spec = HoneycombString(parts.spec.str, parts.spec.length, false);
  std::string scheme = HoneycombString(parts.scheme.str, parts.scheme.length, false);
  std::string username =
      HoneycombString(parts.username.str, parts.username.length, false);
  std::string password =
      HoneycombString(parts.password.str, parts.password.length, false);
  std::string host = HoneycombString(parts.host.str, parts.host.length, false);
  std::string port = HoneycombString(parts.port.str, parts.port.length, false);
  std::string path = HoneycombString(parts.path.str, parts.path.length, false);
  std::string query = HoneycombString(parts.query.str, parts.query.length, false);
  std::string fragment =
      HoneycombString(parts.fragment.str, parts.fragment.length, false);

  GURL gurl;
  if (!spec.empty()) {
    gurl = GURL(spec);
  } else if (!scheme.empty() && !host.empty()) {
    std::stringstream ss;
    ss << scheme << "://";
    if (!username.empty()) {
      ss << username;
      if (!password.empty()) {
        ss << ":" << password;
      }
      ss << "@";
    }
    ss << host;
    if (!port.empty()) {
      ss << ":" << port;
    }
    if (!path.empty()) {
      ss << path;
    }
    if (!query.empty()) {
      ss << "?" << query;
    }
    if (!fragment.empty()) {
      ss << "#" << fragment;
    }
    gurl = GURL(ss.str());
  }

  if (gurl.is_valid()) {
    url = gurl.spec();
    return true;
  }

  return false;
}

HoneycombString HoneycombFormatUrlForSecurityDisplay(const HoneycombString& origin_url) {
  return url_formatter::FormatUrlForSecurityDisplay(
      GURL(origin_url.ToString()));
}

HoneycombString HoneycombGetMimeType(const HoneycombString& extension) {
  // Requests should not block on the disk!  On POSIX this goes to disk.
  // http://code.google.com/p/chromium/issues/detail?id=59849
  base::ScopedAllowBlockingForTesting allow_blocking;

  std::string mime_type;
  net::GetMimeTypeFromExtension(extension, &mime_type);
  return mime_type;
}

void HoneycombGetExtensionsForMimeType(const HoneycombString& mime_type,
                                 std::vector<HoneycombString>& extensions) {
  using VectorType = std::vector<base::FilePath::StringType>;
  VectorType ext;
  net::GetExtensionsForMimeType(mime_type, &ext);
  VectorType::const_iterator it = ext.begin();
  for (; it != ext.end(); ++it) {
    extensions.push_back(*it);
  }
}

HoneycombString HoneycombBase64Encode(const void* data, size_t data_size) {
  if (data_size == 0) {
    return HoneycombString();
  }

  base::StringPiece input(static_cast<const char*>(data), data_size);
  std::string output;
  base::Base64Encode(input, &output);
  return output;
}

HoneycombRefPtr<HoneycombBinaryValue> HoneycombBase64Decode(const HoneycombString& data) {
  if (data.size() == 0) {
    return nullptr;
  }

  const std::string& input = data;
  std::string output;
  if (base::Base64Decode(input, &output)) {
    return HoneycombBinaryValue::Create(output.data(), output.size());
  }
  return nullptr;
}

HoneycombString HoneycombURIEncode(const HoneycombString& text, bool use_plus) {
  return base::EscapeQueryParamValue(text.ToString(), use_plus);
}

HoneycombString HoneycombURIDecode(const HoneycombString& text,
                       bool convert_to_utf8,
                       honey_uri_unescape_rule_t unescape_rule) {
  const base::UnescapeRule::Type type =
      static_cast<base::UnescapeRule::Type>(unescape_rule);
  if (convert_to_utf8) {
    return base::UnescapeAndDecodeUTF8URLComponentWithAdjustments(
        text.ToString(), type, nullptr);
  } else {
    return base::UnescapeURLComponent(text.ToString(), type);
  }
}
