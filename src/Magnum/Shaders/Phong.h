#ifndef Magnum_Shaders_Phong_h
#define Magnum_Shaders_Phong_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Class @ref Magnum::Shaders::Phong
 */

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Phong shader

Uses ambient, diffuse and specular color or texture. For a colored mesh you
need to provide the @ref Position and @ref Normal attributes in your triangle
mesh. By default, the shader renders the mesh with a white color in an identity
transformation. Use @ref setTransformationMatrix(), @ref setNormalMatrix(),
@ref setProjectionMatrix(), @ref setLightPosition() and others to configure
the shader.

@image html shaders-phong.png width=256px

@section Shaders-Phong-colored Colored rendering

Common mesh setup:

@snippet MagnumShaders.cpp Phong-usage-colored1

Common rendering setup:

@snippet MagnumShaders.cpp Phong-usage-colored2

@section Shaders-Phong-texture Textured rendering

If you want to use textures, you need to provide also the
@ref TextureCoordinates attribute. Pass appropriate @ref Flag combination to
the constructor and then at render time don't forget to also call appropriate
subset of @ref bindAmbientTexture(), @ref bindDiffuseTexture() and
@ref bindSpecularTexture() (or the combined @ref bindTextures()). The texture
is multipled by the color, which is by default set to fully opaque white for
enabled textures. Mesh setup with a diffuse and a specular texture:

@snippet MagnumShaders.cpp Phong-usage-texture1

Common rendering setup:

@snippet MagnumShaders.cpp Phong-usage-texture2

@section Shaders-Phong-alpha Alpha blending and masking

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

To avoid specular highlights on transparent areas, specular alpha should be
always set to @cpp 0.0f @ce. On the other hand, non-zero specular alpha can be
for example used to render transparent materials which are still expected to
have specular highlights such as glass or soap bubbles.

An alternative is to enable @ref Flag::AlphaMask and tune @ref setAlphaMask()
for simple binary alpha-masked drawing that doesn't require depth sorting or
blending enabled. Note that this feature is implemented using the GLSL
@glsl discard @ce operation which is known to have considerable performance
impact on some platforms. With proper depth sorting and blending you'll usually
get much better performance and output quality.

For general alpha-masked drawing you need to provide an ambient texture with
alpha channel and set alpha channel of the diffuse/specular color to @cpp 0.0f @ce
so only ambient alpha will be taken into account. If you have a diffuse texture
combined with the alpha mask, you can use that texture for both ambient and
diffuse part and then separate the alpha like this:

@snippet MagnumShaders.cpp Phong-usage-alpha

@section Shaders-Phong-object-id Object ID output

The shader supports writing object ID to the framebuffer for object picking or
other annotation purposes. Enable it using @ref Flag::ObjectId and set up an
integer buffer attached to the @ref ObjectIdOutput attachment. If you have a
batch of meshes with different object IDs, enable @ref Flag::InstancedObjectId
and supply per-vertex IDs to the @ref ObjectId attribute. The output will
contain a sum of the per-vertex ID and ID coming from @ref setObjectId().

The functionality is practically the same as in the @ref Flat shader, see
@ref Shaders-Flat-object-id "its documentation" for more information and usage
example.

@requires_gles30 Object ID output requires integer buffer attachments, which
    are not available in OpenGL ES 2.0 or WebGL 1.0.

@section Shaders-Phong-instancing Instanced rendering

Enabling @ref Flag::InstancedTransformation will turn the shader into an
instanced one. It'll take per-instance transformation and normal matrix from
the @ref TransformationMatrix and @ref NormalMatrix attributes, applying those
before the matrix set by @ref setTransformationMatrix() and
@ref setNormalMatrix(). Besides that, @ref Flag::VertexColor (and the
@ref Color3 / @ref Color4) attributes can work as both per-vertex and
per-instance, and for texturing it's possible to have per-instance texture
offset taken from @ref TextureOffset when @ref Flag::InstancedTextureOffset is
enabled (similarly to transformation, applied before @ref setTextureMatrix()).
The snippet below shows adding a buffer with per-instance transformation to a
mesh --- note how a normal matrix attribute has to be populated and supplied as
well to ensure lighting works:

@snippet MagnumShaders.cpp Phong-usage-instancing

@requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
@requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
    @gl_extension{EXT,instanced_arrays} or @gl_extension{NV,instanced_arrays}
    in OpenGL ES 2.0.
@requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays} in WebGL
    1.0.

@section Shaders-Phong-zero-lights Zero lights

Creating this shader with zero lights makes its output equivalent to the
@ref Flat3D shader --- only @ref setAmbientColor() and @ref bindAmbientTexture()
(if @ref Flag::AmbientTexture is enabled) are taken into account, which
correspond to @ref Flat::setColor() and @ref Flat::bindTexture(). This is
useful to reduce complexity in apps that render models with pre-baked lights.
For instanced workflows using zero lights means the @ref NormalMatrix instance
attribute doesn't need to be supplied either. In addition, enabling
@ref Flag::VertexColor and using a default ambient color with no texturing
makes this shader equivalent to @ref VertexColor.

@see @ref shaders
*/
class MAGNUM_SHADERS_EXPORT Phong: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef Generic3D::Position Position;

        /**
         * @brief Normal direction
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3".
         */
        typedef Generic3D::Normal Normal;

        /**
         * @brief Tangent direction
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector3 "Vector3", used only if
         * @ref Flag::NormalTexture is set.
         */
        typedef Generic3D::Tangent Tangent;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2", used only if at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture and
         * @ref Flag::SpecularTexture is set.
         */
        typedef Generic3D::TextureCoordinates TextureCoordinates;

        /**
         * @brief Three-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
         * either this or the @ref Color4 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef Generic3D::Color3 Color3;

        /**
         * @brief Four-component vertex color
         * @m_since{2019,10}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
         * either this or the @ref Color3 attribute. Used only if
         * @ref Flag::VertexColor is set.
         */
        typedef Generic3D::Color4 Color4;

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief (Instanced) object ID
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::UnsignedInt.
         * Used only if @ref Flag::InstancedObjectId is set.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        typedef Generic3D::ObjectId ObjectId;
        #endif

        /**
         * @brief (Instanced) transformation matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix4.
         * Used only if @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef Generic3D::TransformationMatrix TransformationMatrix;

        /**
         * @brief (Instanced) normal matrix
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Matrix3x3.
         * Used only if @ref Flag::InstancedTransformation is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef Generic3D::NormalMatrix NormalMatrix;

        /**
         * @brief (Instanced) texture offset
         * @m_since{2020,06}
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Vector2. Used
         * only if @ref Flag::InstancedTextureOffset is set.
         * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
         * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
         *      @gl_extension{EXT,instanced_arrays} or
         *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
         *      in WebGL 1.0.
         */
        typedef typename Generic3D::TextureOffset TextureOffset;

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             * @m_since{2019,10}
             */
            ColorOutput = Generic3D::ColorOutput,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Object ID shader output. @ref shaders-generic "Generic output",
             * present only if @ref Flag::ObjectId is set. Expects a
             * single-component unsigned integral attachment. Writes the value
             * set in @ref setObjectId() there, see
             * @ref Shaders-Phong-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,texture_integer}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectIdOutput = Generic3D::ObjectIdOutput
            #endif
        };

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedShort {
            /**
             * Multiply ambient color with a texture.
             * @see @ref setAmbientColor(), @ref bindAmbientTexture()
             */
            AmbientTexture = 1 << 0,

            /**
             * Multiply diffuse color with a texture.
             * @see @ref setDiffuseColor(), @ref bindDiffuseTexture()
             */
            DiffuseTexture = 1 << 1,

            /**
             * Multiply specular color with a texture.
             * @see @ref setSpecularColor(), @ref bindSpecularTexture()
             */
            SpecularTexture = 1 << 2,

            /**
             * Modify normals according to a texture. Requires the
             * @ref Tangent attribute to be present.
             * @m_since{2019,10}
             */
            NormalTexture = 1 << 4,

            /**
             * Enable alpha masking. If the combined fragment color has an
             * alpha less than the value specified with @ref setAlphaMask(),
             * given fragment is discarded.
             *
             * This uses the @glsl discard @ce operation which is known to have
             * considerable performance impact on some platforms. While useful
             * for cheap alpha masking that doesn't require depth sorting,
             * with proper depth sorting and blending you'll usually get much
             * better performance and output quality.
             */
            AlphaMask = 1 << 3,

            /**
             * Multiply diffuse color with a vertex color. Requires either
             * the @ref Color3 or @ref Color4 attribute to be present.
             * @m_since{2019,10}
             */
            VertexColor = 1 << 5,

            /**
             * Enable texture coordinate transformation. If this flag is set,
             * the shader expects that at least one of
             * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
             * @ref Flag::SpecularTexture or @ref Flag::NormalTexture is
             * enabled as well.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 6,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Enable object ID output. See @ref Shaders-Phong-object-id
             * for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2019,10}
             */
            ObjectId = 1 << 7,

            /**
             * Instanced object ID. Retrieves a per-instance / per-vertex
             * object ID from the @ref ObjectId attribute, outputting a sum of
             * the per-vertex ID and ID coming from @ref setObjectId().
             * Implicitly enables @ref Flag::ObjectId. See
             * @ref Shaders-Phong-object-id for more information.
             * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
             * @requires_gles30 Object ID output requires integer support in
             *      shaders, which is not available in OpenGL ES 2.0 or WebGL
             *      1.0.
             * @m_since{2020,06}
             */
            InstancedObjectId = (1 << 8)|ObjectId,
            #endif

            /**
             * Instanced transformation. Retrieves a per-instance
             * transformation and normal matrix from the
             * @ref TransformationMatrix / @ref NormalMatrix attributes and
             * uses them together with matrices coming from
             * @ref setTransformationMatrix() and @ref setNormalMatrix() (first
             * the per-instance, then the uniform matrix). See
             * @ref Shaders-Phong-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTransformation = 1 << 9,

            /**
             * Instanced texture offset. Retrieves a per-instance offset vector
             * from the @ref TextureOffset attribute and uses it together with
             * the matrix coming from @ref setTextureMatrix() (first the
             * per-instance vector, then the uniform matrix). Instanced texture
             * scaling and rotation is not supported at the moment, you can
             * specify that only via the uniform @ref setTextureMatrix().
             * Implicitly enables @ref Flag::TextureTransformation. See
             * @ref Shaders-Phong-instancing for more information.
             * @requires_gl33 Extension @gl_extension{ARB,instanced_arrays}
             * @requires_gles30 Extension @gl_extension{ANGLE,instanced_arrays},
             *      @gl_extension{EXT,instanced_arrays} or
             *      @gl_extension{NV,instanced_arrays} in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{ANGLE,instanced_arrays}
             *      in WebGL 1.0.
             * @m_since{2020,06}
             */
            InstancedTextureOffset = (1 << 10)|TextureTransformation
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Flags
         * @param lightCount    Count of light sources
         */
        explicit Phong(Flags flags = {}, UnsignedInt lightCount = 1);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit Phong(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        Phong(const Phong&) = delete;

        /** @brief Move constructor */
        Phong(Phong&&) noexcept = default;

        /** @brief Copying is not allowed */
        Phong& operator=(const Phong&) = delete;

        /** @brief Move assignment */
        Phong& operator=(Phong&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /** @brief Light count */
        UnsignedInt lightCount() const { return _lightCount; }

        /**
         * @brief Set ambient color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::AmbientTexture is set, default value is
         * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
         * ambient texture, otherwise default value is @cpp 0x00000000_rgbaf @ce.
         * @see @ref bindAmbientTexture()
         */
        Phong& setAmbientColor(const Magnum::Color4& color);

        /**
         * @brief Bind an ambient texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AmbientTexture
         * enabled.
         * @see @ref bindTextures(), @ref setAmbientColor()
         */
        Phong& bindAmbientTexture(GL::Texture2D& texture);

        /**
         * @brief Set diffuse color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce. If @ref lightCount() is
         * zero, this function is a no-op, as diffuse color doesn't contribute
         * to the output in that case.
         * @see @ref bindDiffuseTexture()
         */
        Phong& setDiffuseColor(const Magnum::Color4& color);

        /**
         * @brief Bind a diffuse texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::DiffuseTexture
         * enabled. If @ref lightCount() is zero, this function is a no-op, as
         * diffuse color doesn't contribute to the output in that case.
         * @see @ref bindTextures(), @ref setDiffuseColor()
         */
        Phong& bindDiffuseTexture(GL::Texture2D& texture);

        /**
         * @brief Bind a normal texture
         * @return Reference to self (for method chaining)
         * @m_since{2019,10}
         *
         * Expects that the shader was created with @ref Flag::NormalTexture
         * enabled and the @ref Tangent attribute was supplied. If
         * @ref lightCount() is zero, this function is a no-op, as normals
         * dosn't contribute to the output in that case.
         * @see @ref bindTextures()
         */
        Phong& bindNormalTexture(GL::Texture2D& texture);

        /**
         * @brief Set specular color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffff00_rgbaf @ce. Color will be multiplied
         * with specular texture if @ref Flag::SpecularTexture is set. If you
         * want to have a fully diffuse material, set specular color to
         * @cpp 0x00000000_rgbaf @ce. If @ref lightCount() is zero, this
         * function is a no-op, as specular color doesn't contribute to the
         * output in that case.
         * @see @ref bindSpecularTexture()
         */
        Phong& setSpecularColor(const Magnum::Color4& color);

        /**
         * @brief Bind a specular texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::SpecularTexture
         * enabled. If @ref lightCount() is zero, this function is a no-op, as
         * specular color doesn't contribute to the output in that case.
         * @see @ref bindTextures(), @ref setSpecularColor()
         */
        Phong& bindSpecularTexture(GL::Texture2D& texture);

        /**
         * @brief Bind textures
         * @return Reference to self (for method chaining)
         *
         * A particular texture has effect only if particular texture flag from
         * @ref Phong::Flag "Flag" is set, you can use @cpp nullptr @ce for the
         * rest. Expects that the shader was created with at least one of
         * @ref Flag::AmbientTexture, @ref Flag::DiffuseTexture,
         * @ref Flag::SpecularTexture or @ref Flag::NormalTexture enabled. More
         * efficient than setting each texture separately.
         * @see @ref bindAmbientTexture(), @ref bindDiffuseTexture(),
         *      @ref bindSpecularTexture(), @ref bindNormalTexture()
         */
        Phong& bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular, GL::Texture2D* normal
            #ifdef MAGNUM_BUILD_DEPRECATED
            = nullptr
            #endif
        );

        /**
         * @brief Set shininess
         * @return Reference to self (for method chaining)
         *
         * The larger value, the harder surface (smaller specular highlight).
         * Initial value is @cpp 80.0f @ce. If @ref lightCount() is zero, this
         * function is a no-op, as specular color doesn't contribute to the
         * output in that case.
         */
        Phong& setShininess(Float shininess);

        /**
         * @brief Set alpha mask value
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AlphaMask
         * enabled. Fragments with alpha values smaller than the mask value
         * will be discarded. Initial value is @cpp 0.5f @ce. See the flag
         * documentation for further information.
         */
        Phong& setAlphaMask(Float mask);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Set object ID
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::ObjectId
         * enabled. Value set here is written to the @ref ObjectIdOutput, see
         * @ref Shaders-Phong-object-id for more information. Default is
         * @cpp 0 @ce.
         * @requires_gl30 Extension @gl_extension{EXT,gpu_shader4}
         * @requires_gles30 Object ID output requires integer support in
         *      shaders, which is not available in OpenGL ES 2.0 or WebGL 1.0.
         */
        Phong& setObjectId(UnsignedInt id);
        #endif

        /**
         * @brief Set transformation matrix
         * @return Reference to self (for method chaining)
         *
         * You need to set also @ref setNormalMatrix() with a corresponding
         * value. Initial value is an identity matrix.
         */
        Phong& setTransformationMatrix(const Matrix4& matrix);

        /**
         * @brief Set normal matrix
         * @return Reference to self (for method chaining)
         *
         * The matrix doesn't need to be normalized, as renormalization is done
         * per-fragment anyway. You need to set also
         * @ref setTransformationMatrix() with a corresponding value. Initial
         * value is an identity matrix. If @ref lightCount() is zero, this
         * function is a no-op, as normals don't contribute to the output in
         * that case.
         * @see @ref Math::Matrix4::normalMatrix()
         */
        Phong& setNormalMatrix(const Matrix3x3& matrix);

        /**
         * @brief Set projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix (i.e., an orthographic
         * projection of the default @f$ [ -\boldsymbol{1} ; \boldsymbol{1} ] @f$
         * cube).
         */
        Phong& setProjectionMatrix(const Matrix4& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         */
        Phong& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set light positions
         * @return Reference to self (for method chaining)
         *
         * Initial values are zero vectors --- that will in most cases cause
         * the object to be rendered black (or in the ambient color), as the
         * lights are is inside of it. Expects that the size of the @p lights
         * array is the same as @ref lightCount().
         * @see @ref setLightPosition(UnsignedInt, const Vector3&),
         *      @ref setLightPosition(const Vector3&)
         */
        Phong& setLightPositions(Containers::ArrayView<const Vector3> lights);

        /** @overload */
        Phong& setLightPositions(std::initializer_list<Vector3> lights);

        /**
         * @brief Set position for given light
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setLightPosition() updates just a single light position.
         * Expects that @p id is less than @ref lightCount().
         * @see @ref setLightPosition(const Vector3&)
         */
        Phong& setLightPosition(UnsignedInt id, const Vector3& position);

        /**
         * @brief Set light position
         * @return Reference to self (for method chaining)
         *
         * Convenience alternative to @ref setLightPositions() when there is
         * just one light.
         * @see @ref setLightPosition(UnsignedInt, const Vector3&)
         */
        Phong& setLightPosition(const Vector3& position) {
            return setLightPositions({&position, 1});
        }

        /**
         * @brief Set light colors
         * @return Reference to self (for method chaining)
         *
         * Initial values are @cpp 0xffffffff_rgbaf @ce. Expects that the size
         * of the @p colors array is the same as @ref lightCount().
         */
        Phong& setLightColors(Containers::ArrayView<const Magnum::Color4> colors);

        /** @overload */
        Phong& setLightColors(std::initializer_list<Magnum::Color4> colors);

        /**
         * @brief Set position for given light
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setLightColors() updates just a single light color.
         * Expects that @p id is less than @ref lightCount().
         * @see @ref setLightColor(const Magnum::Color4&)
         */
        Phong& setLightColor(UnsignedInt id, const Magnum::Color4& color);

        /**
         * @brief Set light color
         * @return Reference to self (for method chaining)
         *
         * Convenience alternative to @ref setLightColors() when there is just
         * one light.
         * @see @ref setLightColor(UnsignedInt, const Magnum::Color4&)
         */
        Phong& setLightColor(const Magnum::Color4& color) {
            return setLightColors({&color, 1});
        }

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Flags _flags;
        UnsignedInt _lightCount;
        Int _transformationMatrixUniform{0},
            _projectionMatrixUniform{1},
            _normalMatrixUniform{2},
            _textureMatrixUniform{3},
            _ambientColorUniform{4},
            _diffuseColorUniform{5},
            _specularColorUniform{6},
            _shininessUniform{7},
            _alphaMaskUniform{8};
            #ifndef MAGNUM_TARGET_GLES2
            Int _objectIdUniform{9};
            #endif
        Int _lightPositionsUniform{10},
            _lightColorsUniform; /* 10 + lightCount, set in the constructor */
};

/** @debugoperatorclassenum{Phong,Phong::Flag} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, Phong::Flag value);

/** @debugoperatorclassenum{Phong,Phong::Flags} */
MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, Phong::Flags value);

CORRADE_ENUMSET_OPERATORS(Phong::Flags)

}}

#endif
