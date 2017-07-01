#include "tLoader.h"

/// Filename can be KTX or DDS files
GLuint createTexture(char const* Filename)
{
	gli::texture Texture = gli::load(Filename);
	if (Texture.empty())
		return 0;
	gli::gl GL;
	gli::gl::format const Format = GL.translate(Texture.format());
	GLenum Target = GL.translate(Texture.target());
	GLuint TextureName = 0;
	glGenTextures(1, &TextureName);
	glBindTexture(Target, TextureName);
	glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzle[0]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzle[1]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzle[2]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzle[3]);
	glm::tvec3<GLsizei> const Dimensions(Texture.dimensions());
	GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());
	switch (Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(Target, static_cast<GLint>(Texture.levels()), Format.Internal, Dimensions.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Dimensions.x, Texture.target() == gli::TARGET_2D ? Dimensions.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Dimensions.x, Dimensions.y, Texture.target() == gli::TARGET_3D ? Dimensions.z : FaceTotal);
		break;
	default: assert(0); break;
	}
	for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
		for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				GLsizei const LayerGL = static_cast<GLsizei>(Layer);
				glm::tvec3<GLsizei> Dimensions(Texture.dimensions(Level));
				if (gli::is_target_cube(Texture.target()))
					Target = static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face);
				switch (Texture.target())
				{
				case gli::TARGET_1D:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Dimensions.x,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Dimensions.x, Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage2D(
							Target, static_cast<GLint>(Level), 0, 0,
							Dimensions.x, Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Dimensions.y,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage2D(
							Target, static_cast<GLint>(Level), 0, 0,
							Dimensions.x, Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Dimensions.y,
							Format.External, Format.Type, Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage3D(
							Target, static_cast<GLint>(Level), 0, 0, 0,
							Dimensions.x, Dimensions.y, Texture.target() == gli::TARGET_3D ? Dimensions.z : LayerGL,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage3D(
							Target, static_cast<GLint>(Level), 0, 0, 0,
							Dimensions.x, Dimensions.y, Texture.target() == gli::TARGET_3D ? Dimensions.z : LayerGL,
							Format.External, Format.Type, Texture.data(Layer, Face, Level));
					break;
				default: assert(0); break;
				}
			}
	return TextureName;
}