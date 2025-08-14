/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include <doctest.h>
#include "network/Uri.h"

using namespace ax;
using namespace ax::network;


TEST_SUITE("network/Uri") {
    TEST_CASE("standard_url") {
        std::string s("http://www.facebook.com/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("www.facebook.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("www.facebook.com", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("fragment", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("url_with_port") {
        std::string s("http://www.facebook.com:8080/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("www.facebook.com", u.getHost());
        CHECK_EQ(8080, u.getPort());
        CHECK_EQ("www.facebook.com:8080", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("fragment", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("url_with_ip") {
        std::string s("http://127.0.0.1:8080/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("127.0.0.1", u.getHost());
        CHECK_EQ(8080, u.getPort());
        CHECK_EQ("127.0.0.1:8080", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("fragment", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("url_with_ipv6") {
        std::string s("http://[::1]:8080/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("[::1]", u.getHost());
        CHECK_EQ("::1", u.getHostName());
        CHECK_EQ(8080, u.getPort());
        CHECK_EQ("[::1]:8080", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("fragment", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("url_with_ipv6_port") {
        std::string s("http://[2401:db00:20:7004:face:0:29:0]:8080/hello/world?query");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("[2401:db00:20:7004:face:0:29:0]", u.getHost());
        CHECK_EQ("2401:db00:20:7004:face:0:29:0", u.getHostName());
        CHECK_EQ(8080, u.getPort());
        CHECK_EQ("[2401:db00:20:7004:face:0:29:0]:8080", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("url_with_ipv6") {
        std::string s("http://[2401:db00:20:7004:face:0:29:0]/hello/world?query");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("[2401:db00:20:7004:face:0:29:0]", u.getHost());
        CHECK_EQ("2401:db00:20:7004:face:0:29:0", u.getHostName());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("[2401:db00:20:7004:face:0:29:0]", u.getAuthority());
        CHECK_EQ("/hello/world", u.getPath());
        CHECK_EQ("query", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());  // canonical
    }

    TEST_CASE("user_password") {
        std::string s("http://user:pass@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("user", u.getUserName());
        CHECK_EQ("pass", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("user:pass@host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());
    }

    TEST_CASE("user") {
        std::string s("http://user@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("user", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("user@host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());
    }

    TEST_CASE("user_empty_password") {
        std::string s("http://user:@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("user", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("user@host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ("http://user@host.com/", u.toString());
    }

    TEST_CASE("empty_user_password") {
        std::string s("http://:pass@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("pass", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ(":pass@host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());
    }

    TEST_CASE("empty_user_empty_password") {
        std::string s("http://@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ("http://host.com/", u.toString());
    }

    TEST_CASE("empty_user_empty_password") {
        std::string s("http://:@host.com/");
        Uri u = Uri::parse(s);
        CHECK_EQ("http", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("host.com", u.getHost());
        CHECK_EQ(80, u.getPort());
        CHECK_EQ("host.com", u.getAuthority());
        CHECK_EQ("/", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ("http://host.com/", u.toString());
    }

    TEST_CASE("local_file_tripple_slash") {
        std::string s("file:///etc/motd");
        Uri u = Uri::parse(s);
        CHECK_EQ("file", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("", u.getHost());
        CHECK_EQ(0, u.getPort());
        CHECK_EQ("", u.getAuthority());
        CHECK_EQ("/etc/motd", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());
    }

    TEST_CASE("local_file"){
        std::string s("file://etc/motd");
        Uri u = Uri::parse(s);
        CHECK_EQ("file", u.getScheme());
        CHECK_EQ("", u.getUserName());
        CHECK_EQ("", u.getPassword());
        CHECK_EQ("etc", u.getHost());
        CHECK_EQ(0, u.getPort());
        CHECK_EQ("etc", u.getAuthority());
        CHECK_EQ("/motd", u.getPath());
        CHECK_EQ("", u.getQuery());
        CHECK_EQ("", u.getFragment());
        CHECK_EQ(s, u.toString());
    }

    TEST_CASE("query_parameters") {
        // test query parameters
        std::string s("http://localhost?&key1=foo&key2=&key3&=bar&=bar=&");
        Uri u           = Uri::parse(s);
        auto paramsList = u.getQueryParams();
        std::map<std::string, std::string> params;
        for (auto&&param : paramsList)
        {
            params[param.first] = param.second;
        }
        CHECK_EQ(3, params.size());
        CHECK_EQ("foo", params["key1"]);
        CHECK_NE(params.end(), params.find("key2"));
        CHECK_EQ("", params["key2"]);
        CHECK_NE(params.end(), params.find("key3"));
        CHECK_EQ("", params["key3"]);
    }

    TEST_CASE("invalid_parameters") {
        // test query parameters
        std::string s("http://localhost?&&&&&&&&&&&&&&&");
        Uri u       = Uri::parse(s);
        auto params = u.getQueryParams();
        CHECK(params.empty());
    }

    TEST_CASE("invalid_parameters_2") {
        // test query parameters
        std::string s("http://localhost?&=invalid_key&key2&key3=foo");
        Uri u           = Uri::parse(s);
        auto paramsList = u.getQueryParams();
        std::map<std::string, std::string> params;
        for (auto&&param : paramsList)
        {
            params[param.first] = param.second;
        }
        CHECK_EQ(2, params.size());
        CHECK_NE(params.end(), params.find("key2"));
        CHECK_EQ("", params["key2"]);
        CHECK_EQ("foo", params["key3"]);
    }

    TEST_CASE("invalid_parameters_3") {
        // test query parameters
        std::string s("http://localhost?&key1=====&&=key2&key3=");
        Uri u           = Uri::parse(s);
        auto paramsList = u.getQueryParams();
        std::map<std::string, std::string> params;
        for (auto&&param : paramsList)
        {
            params[param.first] = param.second;
        }
        CHECK_EQ(1, params.size());
        CHECK_NE(params.end(), params.find("key3"));
        CHECK_EQ("", params["key3"]);
    }

    TEST_CASE("various") {
        // test query parameters
        std::string s("ws://localhost:90?key1=foo=bar&key2=foobar&");
        Uri u           = Uri::parse(s);

        SUBCASE("invalid_parameters_4") {
            auto paramsList = u.getQueryParams();
            std::map<std::string, std::string> params;
            for (auto&& param : paramsList)
            {
                params[param.first] = param.second;
            }
            CHECK_EQ(1, params.size());
            CHECK_EQ("foobar", params["key2"]);
        }

        SUBCASE("copy_constructor") {
            Uri v(u);
            u = v = u;
            CHECK(v.isValid());
            CHECK_EQ("ws", v.getScheme());
            CHECK_EQ("localhost", v.getHost());
            CHECK_EQ("localhost", v.getHostName());
            CHECK_EQ("/", v.getPath());
            CHECK_EQ(90, v.getPort());
            CHECK_EQ("", v.getFragment());
            CHECK_EQ("key1=foo=bar&key2=foobar&", v.getQuery());
            CHECK_EQ(u, v);
        }

        SUBCASE("copy_assign_operator") {
            Uri v;
            v = u;
            CHECK(v.isValid());
            CHECK_EQ("ws", v.getScheme());
            CHECK_EQ("localhost", v.getHost());
            CHECK_EQ("localhost", v.getHostName());
            CHECK_EQ("/", v.getPath());
            CHECK_EQ(90, v.getPort());
            CHECK_EQ("", v.getFragment());
            CHECK_EQ("key1=foo=bar&key2=foobar&", v.getQuery());
            CHECK_EQ(u, v);
        }

        SUBCASE("self_assignment") {
            u = u;
            CHECK(u.isValid());
        }

        SUBCASE("self_move_assignment") {
            u = std::move(u);
            CHECK(u.isValid());
        }

        SUBCASE("move_constructor"){
            Uri v = std::move(u);
            CHECK_FALSE(u.isValid());
            CHECK(v.isValid());
            CHECK_EQ("ws", v.getScheme());
            CHECK_EQ("localhost", v.getHost());
            CHECK_EQ("localhost", v.getHostName());
            CHECK_EQ("/", v.getPath());
            CHECK_EQ(90, v.getPort());
            CHECK_EQ("", v.getFragment());
            CHECK_EQ("key1=foo=bar&key2=foobar&", v.getQuery());
            u = std::move(v);
        }

        SUBCASE("copy_assign_operator") {
            Uri v;
            v = std::move(u);
            CHECK_FALSE(u.isValid());
            CHECK(v.isValid());
            CHECK_EQ("ws", v.getScheme());
            CHECK_EQ("localhost", v.getHost());
            CHECK_EQ("localhost", v.getHostName());
            CHECK_EQ("/", v.getPath());
            CHECK_EQ(90, v.getPort());
            CHECK_EQ("", v.getFragment());
            CHECK_EQ("key1=foo=bar&key2=foobar&", v.getQuery());
            u = v;
        }
    }

    TEST_CASE("invalid_url") {
        std::string s("2http://www.facebook.com");

        Uri u = Uri::parse(s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("invalid_url_2") {
        std::string s("www[facebook]com");

        Uri u = Uri::parse("http://" + s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("invalid_url_3") {
        std::string s("http://[::1:8080/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("invalid_url_4") {
        std::string s("http://::1]:8080/hello/world?query#fragment");

        Uri u = Uri::parse(s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("invalid_url_5") {
        std::string s("http://::1:8080/hello/world?query#fragment");
        Uri u = Uri::parse(s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("invalid_url_6") {
        std::string s("http://2401:db00:20:7004:face:0:29:0/hello/world?query");
        Uri u = Uri::parse(s);
        CHECK_FALSE(u.isValid());
    }

    TEST_CASE("various_checks") {
        Uri http       = Uri::parse("http://google.com");
        Uri https      = Uri::parse("https://www.google.com:90");
        Uri query      = Uri::parse("http://google.com:8080/foo/bar?foo=bar");
        Uri localhost  = Uri::parse("http://localhost:8080");
        Uri ipv6       = Uri::parse("https://[2001:0db8:85a3:0042:1000:8a2e:0370:7334]");
        Uri ipv6short  = Uri::parse("http://[2001:db8:85a3:42:1000:8a2e:370:7334]");
        Uri ipv6port   = Uri::parse("http://[2001:db8:85a3:42:1000:8a2e:370:7334]:90");
        Uri ipv6abbrev = Uri::parse("http://[2001::7334:a:90]");
        Uri ipv6http   = Uri::parse("http://[2001::7334:a]:90");
        Uri ipv6query  = Uri::parse("http://[2001::7334:a]:90/foo/bar?foo=bar");

        CHECK_EQ(http.getScheme(), "http");
        CHECK_EQ(http.getPort(), 80);
        CHECK_EQ(http.getHost(), "google.com");
        CHECK_EQ(https.getScheme(), "https");
        CHECK_EQ(https.getPort(), 90);
        CHECK_EQ(https.getHost(), "www.google.com");
        CHECK_EQ(query.getPort(), 8080);
        CHECK_EQ(query.getPathEtc(), "/foo/bar?foo=bar");
        CHECK_EQ(localhost.getScheme(), "http");
        CHECK_EQ(localhost.getHost(), "localhost");
        CHECK_EQ(localhost.getPort(), 8080);
        CHECK_EQ(ipv6.getScheme(), "https");
        CHECK_EQ(ipv6.getHostName(), "2001:0db8:85a3:0042:1000:8a2e:0370:7334");
        CHECK_EQ(ipv6.getPort(), 443);
        CHECK_EQ(ipv6short.getScheme(), "http");
        CHECK_EQ(ipv6short.getHostName(), "2001:db8:85a3:42:1000:8a2e:370:7334");
        CHECK_EQ(ipv6short.getPort(), 80);
        CHECK_EQ(ipv6port.getScheme(), "http");
        CHECK_EQ(ipv6port.getHostName(), "2001:db8:85a3:42:1000:8a2e:370:7334");
        CHECK_EQ(ipv6port.getPort(), 90);
        CHECK_EQ(ipv6abbrev.getScheme(), "http");
        CHECK_EQ(ipv6abbrev.getHostName(), "2001::7334:a:90");
        CHECK_EQ(ipv6abbrev.getPort(), 80);
        CHECK_EQ(ipv6http.getScheme(), "http");
        CHECK_EQ(ipv6http.getPort(), 90);
        CHECK_EQ(ipv6http.getHostName(), "2001::7334:a");
        CHECK_EQ(ipv6query.getScheme(), "http");
        CHECK_EQ(ipv6query.getPort(), 90);
        CHECK_EQ(ipv6query.getHostName(), "2001::7334:a");
        CHECK_EQ(ipv6query.getPathEtc(), "/foo/bar?foo=bar");
    }

    TEST_CASE("various_checks_2") {
        Uri u0 = Uri::parse("http://localhost:84/foo.html?&q=123");
        Uri u1 = Uri::parse("https://localhost:82/foo.html?&q=1");
        Uri u2 = Uri::parse("ws://localhost/foo");
        Uri u3 = Uri::parse("localhost/foo");
        Uri u4 = Uri::parse("localhost:8080");
        Uri u5 = Uri::parse("bb://localhost?&foo=12:4&ccc=13");
        Uri u6 = Uri::parse("cc://localhost:91?&foo=321&bbb=1");

        CHECK_EQ(u0.getScheme(), "http");
        CHECK_EQ(u0.getHost(), "localhost");
        CHECK_EQ(u0.getPort(), 84);
        CHECK_EQ(u0.getPath(), "/foo.html");
        CHECK_EQ(u0.getPathEtc(), "/foo.html?&q=123");

        CHECK_EQ(u1.getScheme(), "https");
        CHECK_EQ(u1.getHost(), "localhost");
        CHECK_EQ(u1.getPort(), 82);
        CHECK_EQ(u1.getPathEtc(), "/foo.html?&q=1");

        CHECK_EQ(u2.getScheme(), "ws");
        CHECK_EQ(u2.getHost(), "localhost");
        CHECK_EQ(u2.getPort(), 80);
        CHECK_EQ(u2.getPath(), "/foo");

        CHECK_EQ(u3.getScheme(), "");
        CHECK_EQ(u3.getHost(), "localhost");
        CHECK_EQ(u3.getPort(), 0);
        CHECK_EQ(u3.getPath(), "/foo");

        CHECK_EQ(u4.getScheme(), "");
        CHECK_EQ(u4.getHost(), "localhost");
        CHECK_EQ(u4.getPort(), 8080);
        CHECK_EQ(u4.getPath(), "/");
        CHECK_EQ(u4.getPathEtc(), "/");

        CHECK_EQ(u5.getScheme(), "bb");
        CHECK_EQ(u5.getHost(), "localhost");
        CHECK_EQ(u5.getPort(), 0);
        CHECK_EQ(u5.getPath(), "/");
        CHECK_EQ(u5.getPathEtc(), "/?&foo=12:4&ccc=13");
        CHECK_EQ(u5.getQuery(), "&foo=12:4&ccc=13");

        CHECK_EQ(u6.getScheme(), "cc");
        CHECK_EQ(u6.getHost(), "localhost");
        CHECK_EQ(u6.getPort(), 91);
        CHECK_EQ(u6.getPath(), "/");
        CHECK_EQ(u6.getPathEtc(), "/?&foo=321&bbb=1");
        CHECK_EQ(u6.getQuery(), "&foo=321&bbb=1");
    }
}
