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
#include "TestUtils.h"
#include "platform/FileUtils.h"

USING_NS_AX;


TEST_SUITE("platform/FileUtils") {
    // !!!Don't invoke FileUtils::getInstacne at here, it's dangerous due to
    // The test suite function will invoke before entrypoint `main`, it will cause
    // crash on Linux(maybe others), crt not initalized properly yet.
#define fu FileUtils::getInstance()

    TEST_CASE("getWritablePath") {
        REQUIRE(not fu->getWritablePath().empty());
        REQUIRE(fu->getWritablePath().back() == '/');
    }


    TEST_CASE("getDefaultResourceRootPath") {
        REQUIRE(not fu->getDefaultResourceRootPath().empty());
        REQUIRE(fu->getDefaultResourceRootPath().back() == '/');
    }


    TEST_CASE("fullPathForDirectory") {
        CHECK(fu->fullPathForDirectory("") == "");

        CHECK(fu->fullPathForDirectory("doesnt_exist") == "");
        CHECK(fu->fullPathForDirectory("/doesnt_exist") == "/doesnt_exist/");

        auto path = fu->fullPathForDirectory("text");
        REQUIRE(not path.empty());
        CHECK(path.back() == '/');
        CHECK(fu->isAbsolutePath(path));

        CHECK(fu->fullPathForDirectory(path) == path);
    }

    TEST_CASE("path_ops")
    {
        CHECK(FileUtils::getPathDirName("a/b/c.txt") == "a/b");
        CHECK(FileUtils::getPathBaseName("a/b/c.txt") == "c.txt");
        CHECK(FileUtils::getPathBaseNameNoExtension("a/b/c.txt") == "c");
        CHECK(FileUtils::getPathBaseNameNoExtension("a/b/c") == "c");
        CHECK(FileUtils::getPathBaseNameNoExtension("abc.txt") == "abc");
        CHECK(FileUtils::getPathBaseNameNoExtension("a/b/c.txt.bytes") == "c.txt");
    }

    TEST_CASE("search_paths") {
        fu->purgeCachedEntries();

        SUBCASE("addSearchPath") {
            auto originalSearchPaths = fu->getOriginalSearchPaths();

            CHECK(fu->fullPathForFilename("123.txt") == "");

            auto path = fu->fullPathForDirectory("text");
            REQUIRE(not path.empty());

            fu->addSearchPath(path);
            CHECK(fu->fullPathForFilename("123.txt") == path + "123.txt");

            fu->setSearchPaths(originalSearchPaths);
            CHECK(fu->fullPathForFilename("123.txt") == "");
        }


        SUBCASE("setDefaultResourceRootPath") {
            auto originalDefaultResourceRootPath = fu->getDefaultResourceRootPath();

            REQUIRE(not originalDefaultResourceRootPath.empty());

            CHECK(fu->fullPathForFilename("123.txt") == "");

            fu->setDefaultResourceRootPath(originalDefaultResourceRootPath + "text");
            CHECK(fu->fullPathForFilename("123.txt") == originalDefaultResourceRootPath + "text/123.txt");

            fu->setDefaultResourceRootPath(originalDefaultResourceRootPath);
            CHECK(fu->fullPathForFilename("123.txt") == "");
        }
    }


    TEST_CASE("isFileExist" * doctest::timeout(10)) {
        CHECK(fu->isFileExist("text/123.txt"));
        CHECK(fu->isFileExist(fu->fullPathForFilename("text/123.txt")));
        CHECK(not fu->isFileExist("text/doesnt_exist.txt"));
        CHECK(not fu->isFileExist("/text/doesnt_exist"));
        CHECK(not fu->isFileExist("text"));
        CHECK(not fu->isFileExist(fu->fullPathForDirectory("text")));

        auto run = AsyncRunner<bool>();
        fu->isFileExist("text/123.txt", [&](bool exists) {
            run.finish(exists);
        });
        CHECK(run());
    }


    TEST_CASE("isDirectoryExist") {
        CHECK(fu->isDirectoryExist("text"));
        CHECK(fu->isDirectoryExist(fu->fullPathForDirectory("text")));
        CHECK(not fu->isDirectoryExist("doesnt_exist"));
        CHECK(not fu->isDirectoryExist("/doesnt_exist"));
        CHECK(not fu->isDirectoryExist("text/123.txt"));
        CHECK(not fu->isDirectoryExist(fu->fullPathForFilename("text/123.txt")));

        auto run = AsyncRunner<bool>();
        fu->isDirectoryExist(fu->fullPathForDirectory("text"), [&](bool exists) {
            run.finish(exists);
        });
        CHECK(run());
    }


    TEST_CASE("getFileSize") {
        CHECK(fu->getFileSize(fu->fullPathForFilename("text/123.txt")) == 3);
        CHECK(fu->getFileSize(fu->fullPathForFilename("text/hello.txt")) == 12);
        CHECK(fu->getFileSize("text/doesnt_exist.txt") == -1);

        auto run = AsyncRunner<int>();
        fu->getFileSize("text/123.txt", [&](int size) {
            run.finish(size);
        });
        CHECK(run() == 3);
    }


    TEST_CASE("renameFile") {
        auto file1 = fu->getWritablePath() + "__test1.txt";
        auto file2 = fu->getWritablePath() + "__test2.txt";

        if (fu->isFileExist(file1))
            REQUIRE(fu->removeFile(file1));
        if (fu->isFileExist(file2))
            REQUIRE(fu->removeFile(file2));

        REQUIRE(fu->writeStringToFile("Hello!", file1));
        REQUIRE(fu->isFileExist(file1));

        REQUIRE(fu->renameFile(fu->getWritablePath(), "__test1.txt", "__test2.txt"));
        CHECK(not fu->isFileExist(file1));
        CHECK(fu->isFileExist(file2));

        auto run = AsyncRunner<bool>();
        fu->renameFile(fu->getWritablePath(), "__test2.txt", "__test1.txt", [&](bool success) {
            run.finish(success);
        });
        CHECK(run());
        CHECK(fu->isFileExist(file1));

        CHECK(fu->removeFile(file1));
    }


    TEST_CASE("removeFile") {
        auto file = fu->getWritablePath() + "__test.txt";

        if (fu->isFileExist(file))
            REQUIRE(fu->removeFile(file));

        REQUIRE(not fu->isFileExist(file));
        REQUIRE(fu->writeStringToFile("Hello!", file));
        REQUIRE(fu->isFileExist(file));

        SUBCASE("normal") {
            CHECK(fu->removeFile(file));
            CHECK(not fu->isFileExist(file));
            CHECK(not fu->removeFile(file));
        }


        SUBCASE("async") {
            auto run = AsyncRunner<bool>();
            fu->removeFile(file, [&](bool success) {
                run.finish(success);
            });
            CHECK(run());
            CHECK(not fu->isFileExist(file));
        }
    }


    TEST_CASE("file_ops") {
        auto file1 = fu->getWritablePath() + "__test1.txt";
        auto file2 = fu->getWritablePath() + "__test2.txt";

        if (fu->isFileExist(file1))
            REQUIRE(fu->removeFile(file1));
        if (fu->isFileExist(file2))
            REQUIRE(fu->removeFile(file2));

        REQUIRE(not fu->isFileExist(file1));
        REQUIRE(not fu->isFileExist(file2));

        SUBCASE("writeStringToFile,getStringFromFile") {
            REQUIRE(fu->writeStringToFile("Hello!", file1));
            CHECK(fu->getStringFromFile(file1) == "Hello!");
            REQUIRE(fu->removeFile(file1));

            auto run = AsyncRunner<bool>();
            fu->writeStringToFile("Hello!", file1, [&](bool success) {
                run.finish(success);
            });
            REQUIRE(run());

            auto run2 = AsyncRunner<std::string>();
            fu->getStringFromFile(file1, [&](std::string_view value) {
                run2.finish(std::string(value));
            });
            CHECK(run2() == "Hello!");

            CHECK(fu->removeFile(file1));
        }
    }


    TEST_CASE("dir_ops") {
        auto dir = fu->getWritablePath() + "__test_dir/";
        auto subdirs = dir + "dir1/dir2/";

        if (fu->isDirectoryExist(dir))
            REQUIRE(fu->removeDirectory(dir));

        SUBCASE("createDirectory,removeDirectory,isDirectoryExist") {
            REQUIRE(not fu->isDirectoryExist(dir));
            REQUIRE(fu->createDirectories(subdirs));
            REQUIRE(fu->isDirectoryExist(dir));
            REQUIRE(fu->isDirectoryExist(subdirs));
            REQUIRE(fu->removeDirectory(dir));
            CHECK(not fu->isDirectoryExist(dir));
        }
    }


    TEST_CASE("ResizableBufferAdapter") {
        yasio::byte_buffer buffer;

        fu->getContents("text/123.txt", &buffer);
        REQUIRE(buffer.size() == 3);
        CHECK(buffer[0] == '1');
        CHECK(buffer[1] == '2');
        CHECK(buffer[2] == '3');

        fu->getContents("text/hello.txt", &buffer);
        REQUIRE(buffer.size() == 12);
        auto expected = std::string_view("Hello world!");
        for (size_t i = 0; i < expected.size(); ++i)
            CHECK(buffer[i] == expected[i]);
    }


    TEST_CASE("getContents") {
        static const std::string FileErrors[] = { "OK", "NotExists", "OpenFailed", "ReadFailed", "NotInitialized", "TooLarge", "ObtainSizeFailed", };

        auto file = "text/binary.bin";
        std::vector<char> binary = {'\r', '\n', '\r', '\n', '\0', '\0', '\r', '\n'};
        std::string text(binary.begin(), binary.end());


        SUBCASE("requirements") {
            REQUIRE(fu->isFileExist(file));
        }


        SUBCASE("string") {
            std::string bs;
            REQUIRE(fu->getContents(file, &bs) == FileUtils::Status::OK);
            CHECK(bs.size() == binary.size());
            CHECK(bs == text);
        }


        SUBCASE("getStringFromFile") {
            std::string ts = fu->getStringFromFile(file);
            CHECK(ts.size() == binary.size());
            CHECK(ts == text);
        }


        SUBCASE("vector") {
            std::vector<int> vbuf;
            REQUIRE(fu->getContents(file, &vbuf) == FileUtils::Status::OK);
            CHECK(vbuf.size() == binary.size() / sizeof(int));
            CHECK(memcmp(vbuf.data(), binary.data(), binary.size()) == 0);
        }


        SUBCASE("Data") {
            Data dbuf;
            REQUIRE(fu->getContents(file, &dbuf) == FileUtils::Status::OK);
            CHECK(dbuf.getSize() == binary.size());
            CHECK(std::equal(dbuf.getBytes(), dbuf.getBytes() + dbuf.getSize(), binary.begin()));
        }
    }


    TEST_CASE("write_data") {
        auto file = fu->getWritablePath() + "__test.txt";

        if (fu->isFileExist(file))
            REQUIRE(fu->removeFile(file));


        SUBCASE("writeDataToFile") {
            std::string writeDataStr = "Hello World!";
            Data writeData;
            writeData.copy((unsigned char*)writeDataStr.c_str(), writeDataStr.size());

            REQUIRE(fu->writeDataToFile(writeData, file));
            REQUIRE(fu->isFileExist(file));
            CHECK(fu->getStringFromFile(file) == writeDataStr);
            fu->removeFile(file);

            auto run = AsyncRunner<bool>();
            fu->writeDataToFile(writeData, file, [&](bool success) {
                run.finish(success);
            });
            REQUIRE(run());
            CHECK(fu->getStringFromFile(file) == writeDataStr);
            fu->removeFile(file);
        }


        SUBCASE("getDataFromFile") {
            Data readData = FileUtils::getInstance()->getDataFromFile("text/hello.txt");
            REQUIRE(not readData.isNull());
            CHECK(readData.getSize() == 12);
            CHECK(std::equal(readData.getBytes(), readData.getBytes() + readData.getSize(), "Hello world!"));

            readData.clear();
            auto run = AsyncRunner<Data>();
            fu->getDataFromFile("text/hello.txt", [&](const Data& readData) {
                run.finish(readData);
            });
            auto result = run();
            REQUIRE(not result.isNull());
            CHECK(result.getSize() == 12);
            CHECK(std::equal(result.getBytes(), result.getBytes() + result.getSize(), "Hello world!"));
        }
    }


    TEST_CASE("ValueMap") {
        auto file = fu->getWritablePath() + "__test.txt";

        if (fu->isFileExist(file))
            REQUIRE(fu->removeFile(file));

        ValueMap valueMap;
        {
            ValueMap mapInValueMap;
            mapInValueMap["string1"] = "string in dictInMap key 0";
            mapInValueMap["string2"] = "string in dictInMap key 1";
            mapInValueMap["none"].getType();

            valueMap["data0"] = Value(mapInValueMap);

            valueMap["data1"] = Value("string in array");

            ValueVector arrayInMap;
            arrayInMap.emplace_back(Value("string 0 in arrayInMap"));
            arrayInMap.emplace_back(Value("string 1 in arrayInMap"));
            valueMap["data2"] = arrayInMap;

            // add boolean to the plist
            auto booleanObject = Value(true);
            valueMap["data3"]  = booleanObject;

            // add integer to the plist
            auto intObject    = Value(1024);
            valueMap["data4"] = intObject;

            // add float to the plist
            auto floatObject  = Value(1024.125f);
            valueMap["data5"] = floatObject;

            // add double to the plist
            auto doubleObject = Value(1024.5);
            valueMap["data6"] = doubleObject;
        }

        REQUIRE(fu->writeValueMapToFile(valueMap, file));

        ValueMap readValueMap = FileUtils::getInstance()->getValueMapFromFile(file);
        REQUIRE(not readValueMap.empty());

        CHECK(readValueMap.size() == 7);
        CHECK(readValueMap["data0"].getType() == Value::Type::MAP);
        CHECK(readValueMap["data1"].getType() == Value::Type::STRING);
        CHECK(readValueMap["data2"].getType() == Value::Type::VECTOR);
        CHECK(readValueMap["data3"].getType() == Value::Type::BOOLEAN);
        CHECK(readValueMap["data4"].getType() == Value::Type::INTEGER);
        CHECK(readValueMap["data5"].getType() == Value::Type::DOUBLE);
        CHECK(readValueMap["data6"].getType() == Value::Type::DOUBLE);

        auto mapInMap = readValueMap["data0"].asValueMap();
        CHECK(mapInMap["string1"].asString() == "string in dictInMap key 0");
        CHECK(mapInMap["string2"].asString() == "string in dictInMap key 1");

        CHECK(readValueMap["data1"].asString() == "string in array");

        auto arrayInMap = readValueMap["data2"].asValueVector();
        CHECK(arrayInMap.at(0).asString() == "string 0 in arrayInMap");
        CHECK(arrayInMap.at(1).asString() == "string 1 in arrayInMap");

        CHECK(readValueMap["data3"].asBool() == true);
        CHECK(readValueMap["data4"].asInt() == 1024);
        CHECK(readValueMap["data5"].asFloat() == 1024.125f);
        CHECK(readValueMap["data6"].asDouble() == 1024.5);

        CHECK(fu->removeFile(file));
    }


    TEST_CASE("ValueVector") {
        auto file = fu->getWritablePath() + "__test.txt";

        if (fu->isFileExist(file))
            REQUIRE(fu->removeFile(file));

        ValueVector valueVector;
        {
            ValueMap mapInArray;
            mapInArray["string1"] = "string in dictInArray key 0";
            mapInArray["string2"] = "string in dictInArray key 1";
            valueVector.emplace_back(Value(mapInArray));

            valueVector.emplace_back(Value("string in array"));

            ValueVector arrayInArray;
            arrayInArray.emplace_back(Value("string 0 in arrayInArray"));
            arrayInArray.emplace_back(Value("string 1 in arrayInArray"));
            valueVector.emplace_back(Value(arrayInArray));

            // add boolean to the plist
            auto booleanObject = Value(true);
            valueVector.emplace_back(booleanObject);

            // add integer to the plist
            auto intObject = Value(1024);
            valueVector.emplace_back(intObject);

            // add float to the plist
            auto floatObject = Value(1024.125f);
            valueVector.emplace_back(floatObject);

            // add double to the plist
            auto doubleObject = Value(1024.5);
            valueVector.emplace_back(doubleObject);
        }

        REQUIRE(fu->writeValueVectorToFile(valueVector, file));

        ValueVector readValueVector = FileUtils::getInstance()->getValueVectorFromFile(file);
        REQUIRE(not readValueVector.empty());

        CHECK(readValueVector.size() == 7);
        CHECK(readValueVector.at(0).getType() == Value::Type::MAP);
        CHECK(readValueVector.at(1).getType() == Value::Type::STRING);
        CHECK(readValueVector.at(2).getType() == Value::Type::VECTOR);
        CHECK(readValueVector.at(3).getType() == Value::Type::BOOLEAN);
        CHECK(readValueVector.at(4).getType() == Value::Type::INTEGER);
        CHECK(readValueVector.at(5).getType() == Value::Type::DOUBLE);
        CHECK(readValueVector.at(6).getType() == Value::Type::DOUBLE);

        auto mapInArray = readValueVector.at(0).asValueMap();
        CHECK(mapInArray["string1"].asString() == "string in dictInArray key 0");
        CHECK(mapInArray["string2"].asString() == "string in dictInArray key 1");

        CHECK(readValueVector.at(1).asString() == "string in array");

        auto arrayInArray = readValueVector.at(2).asValueVector();
        CHECK(arrayInArray.at(0).asString() == "string 0 in arrayInArray");
        CHECK(arrayInArray.at(1).asString() == "string 1 in arrayInArray");

        CHECK(readValueVector.at(3).asBool() == true);
        CHECK(readValueVector.at(4).asInt() == 1024);
        CHECK(readValueVector.at(5).asFloat() == 1024.125f);
        CHECK(readValueVector.at(6).asDouble() == 1024.5);

        REQUIRE(fu->removeFile(file));
    }


    TEST_CASE("unicode") {
        auto dir = fu->getWritablePath() + "中文路径/";
        auto filename = dir + "测试文件.test";

        if (fu->isDirectoryExist(dir))
            REQUIRE(fu->removeDirectory(dir));
        if (fu->isFileExist(filename))
            REQUIRE(fu->removeFile(filename));

        REQUIRE(not fu->isDirectoryExist(dir));
        REQUIRE(fu->createDirectory(dir));
        REQUIRE(fu->isDirectoryExist(dir));

        std::string writeDataStr = " 测试字符串.";

        REQUIRE(not fu->isFileExist(filename));
        REQUIRE(fu->writeStringToFile(writeDataStr, filename));
        REQUIRE(fu->isFileExist(filename));
        CHECK(fu->getStringFromFile(filename) == writeDataStr);

        REQUIRE(fu->removeFile(filename));
        CHECK(not fu->isFileExist(filename));

        REQUIRE(fu->removeDirectory(dir));
        CHECK(not fu->isDirectoryExist(dir));
    }


    TEST_CASE("listFiles") {
        auto list = fu->listFiles("text");
        std::sort(list.begin(), list.end());
        auto path = fu->fullPathForDirectory("text");

        CHECK(list.size() == 3);
        CHECK(list[0] == path + "123.txt");
        CHECK(list[1] == path + "binary.bin");
        CHECK(list[2] == path + "hello.txt");
    }
}
