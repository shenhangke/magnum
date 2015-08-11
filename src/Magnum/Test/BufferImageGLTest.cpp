/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/BufferImage.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct BufferImageGLTest: AbstractOpenGLTester {
    explicit BufferImageGLTest();

    void construct();
    void constructCompressed();
    void constructCopy();
    void constructCopyCompressed();
    void constructMove();
    void constructMoveCompressed();

    void setData();
    void setDataCompressed();
};

BufferImageGLTest::BufferImageGLTest() {
    addTests({&BufferImageGLTest::construct,
              &BufferImageGLTest::constructCompressed,
              &BufferImageGLTest::constructCopy,
              &BufferImageGLTest::constructCopyCompressed,
              &BufferImageGLTest::constructMove,
              &BufferImageGLTest::constructMoveCompressed,

              &BufferImageGLTest::setData,
              &BufferImageGLTest::setDataCompressed});
}

void BufferImageGLTest::construct() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0 };
    BufferImage2D a(PixelFormat::Red, PixelType::UnsignedByte, {1, 3}, data, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), PixelFormat::Red);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(a.size(), Vector2i(1, 3));

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const char>{data},
                       TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(a.size(), Vector2i(4, 4));
    CORRADE_COMPARE(a.dataSize(), 8);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const char>{data},
                       TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<BufferImage2D, const BufferImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<BufferImage2D, const BufferImage2D&>{}));
}

void BufferImageGLTest::constructCopyCompressed() {
    CORRADE_VERIFY(!(std::is_constructible<CompressedBufferImage2D, const CompressedBufferImage2D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CompressedBufferImage2D, const CompressedBufferImage2D&>{}));
}

void BufferImageGLTest::constructMove() {
    const char data[4] = { 'a', 'b', 'c', 'd' };
    BufferImage2D a(PixelFormat::Red, PixelType::UnsignedByte, {4, 1}, data, BufferUsage::StaticDraw);
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    BufferImage2D b(std::move(a));

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());

    CORRADE_COMPARE(b.format(), PixelFormat::Red);
    CORRADE_COMPARE(b.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(4, 1));
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned short data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    BufferImage2D c(PixelFormat::RGBA, PixelType::UnsignedShort, {1, 2}, data2, BufferUsage::StaticDraw);
    const Int cId = c.buffer().id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(1, 2));

    CORRADE_COMPARE(c.format(), PixelFormat::Red);
    CORRADE_COMPARE(c.type(), PixelType::UnsignedByte);
    CORRADE_COMPARE(c.size(), Vector2i(4, 1));
    CORRADE_COMPARE(c.buffer().id(), id);
}

void BufferImageGLTest::constructMoveCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};
    const Int id = a.buffer().id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    CompressedBufferImage2D b{std::move(a)};

    CORRADE_COMPARE(a.buffer().id(), 0);
    CORRADE_COMPARE(a.size(), Vector2i());
    CORRADE_COMPARE(a.dataSize(), 0);

    CORRADE_COMPARE(b.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(b.size(), Vector2i(4, 4));
    CORRADE_COMPARE(b.dataSize(), 8);
    CORRADE_COMPARE(b.buffer().id(), id);

    const unsigned char data2[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0, 'd', 0, 0, 0 };
    CompressedBufferImage2D c{CompressedPixelFormat::RGBAS3tcDxt1, {8, 4}, data2, BufferUsage::StaticDraw};
    const Int cId = c.buffer().id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.buffer().id(), cId);
    CORRADE_COMPARE(b.size(), Vector2i(8, 4));
    CORRADE_COMPARE(b.dataSize(), 16);

    CORRADE_COMPARE(c.format(), CompressedPixelFormat::RGBAS3tcDxt1);
    CORRADE_COMPARE(c.size(), Vector2i(4, 4));
    CORRADE_COMPARE(c.dataSize(), 8);
    CORRADE_COMPARE(c.buffer().id(), id);
}

void BufferImageGLTest::setData() {
    const char data[4] = { 'a', 'b', 'c', 'd' };
    BufferImage2D a(PixelFormat::Red, PixelType::UnsignedByte, {4, 1}, data, BufferUsage::StaticDraw);

    const UnsignedShort data2[2*4] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    a.setData(PixelFormat::RGBA, PixelType::UnsignedShort, {1, 2}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data<UnsignedShort>();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), PixelFormat::RGBA);
    CORRADE_COMPARE(a.type(), PixelType::UnsignedShort);
    CORRADE_COMPARE(a.size(), Vector2i(1, 2));

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const UnsignedShort>{data2},
                       TestSuite::Compare::Container);
    #endif
}

void BufferImageGLTest::setDataCompressed() {
    const char data[] = { 'a', 0, 0, 0, 'b', 0, 0, 0 };
    CompressedBufferImage2D a{CompressedPixelFormat::RGBAS3tcDxt1, {4, 4}, data, BufferUsage::StaticDraw};

    const char data2[] = { 'a', 0, 0, 0, 'b', 0, 0, 0, 'c', 0, 0, 0, 'd', 0, 0, 0 };
    a.setData(CompressedPixelFormat::RGBAS3tcDxt3, {8, 4}, data2, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES
    const auto imageData = a.buffer().data();
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(a.format(), CompressedPixelFormat::RGBAS3tcDxt3);
    CORRADE_COMPARE(a.size(), Vector2i(8, 4));
    CORRADE_COMPARE(a.dataSize(), 16);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(imageData, Containers::ArrayView<const char>{data2},
                       TestSuite::Compare::Container);
    #endif
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::BufferImageGLTest)
