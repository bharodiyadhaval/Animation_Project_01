#ifndef _cShaderManager_HG_
#define _cShaderManager_HG_

#include <string>
#include <vector>
#include <map>
#include "sTextureBindBlendInfo.h"

#include "globalOpenGL_GLFW.h"


class CTextureManager;	// Forward declare to avoid include

class cShaderManager
{
public:
	class cShader  {
	public:
		cShader();
		~cShader();
		enum eShaderType
		{
			VERTEX_SHADER,
			FRAGMENT_SHADER,
			GEOMETRY_SHADER,
			TESSELATION_SHADER,
			COMPUTE_SHADER,
			UNKNOWN
		};
		eShaderType shaderType;
		std::string getShaderTypeString(void);

		unsigned int ID;	// or "name" from OpenGL
		std::vector<std::string> vecSource;
		void parseStringIntoMultiLine(std::string singleLineSouce);
		bool bSourceIsMultiLine;
		std::string fileName;

		// Vertex attribute description
		class cAttributeDesc
		{
		public:
			cAttributeDesc();
			std::string programName;
			GLuint		programID;
			std::string name;
			GLuint		index;
			GLenum		type;
			std::string typeAsString;		// English word for GLenum type...
			GLint		size;		// size based on type
			void Clear(void);
		};

		// Uniform description
		class cUniformDesc
		{
		public:
			cUniformDesc();
			std::string programName;
			GLuint		programID;
			std::string name;
			GLuint		index;		// Different from "location": get with glGetActiveUniform() 
			GLint		location;	// Note that this is different from the "index": get with glGetUniformLocation()
			GLenum		type;
			std::string typeAsString;		// English word for GLenum type...
			GLint		size;				// size based on type	(GL_UNIFORM_SIZE)
											// Added
			GLint		NUB_Index;			// -1 if in default block (GL_UNIFORM_BLOCK_INDEX)
			std::string NUB_Name;			// name of the named uniform block (if in default, this is BLANK - rather than "DEFAULT")
			GLint		blockOffsetInBytes;		// GL_UNIFORM_OFFSET
			GLint		blockOffsetInFloats;
			bool		bIsInDefaultBlock;
			GLint		nameLength;			// GL_UNIFORM_NAME_LENGTH
			GLint		arrayStride;		// GL_UNIFORM_ARRAY_STRIDE (-1 if not an array)
			bool		bIsAnArray;			// false by default (see arrayStride, above)
			GLint		matrixStride;		// GL_UNIFORM_MATRIX_STRIDE (-1 if default block)
			bool		bIsRowMajor;		// GL_UNIFORM_IS_ROW_MAJOR = 1 means row-major
											// -1 if col-major in default block, or non-matrix
			bool		bIsMatrix;			// false by default



											// This is used to compare different uniform blocks to see if they match. 
											// Basically, you call this on each uniform in a block, in the index order, and compare them.
											// (this is only done when you are loading interface blocks so it's only done when created 
											//  or assinged, so performance isn't a big deal)
			std::string GenerateInterfaceDefinitionHash(void);

			// Predicate function for sorting
			static bool predicateSortUniformByByteOffset(const cUniformDesc &uniformA, const cUniformDesc &uniformB);
			// 
			void Clear(void);
		};


	};// class cShader

	class cShaderProgram
	{
	public:
		cShaderProgram();
		~cShaderProgram();
		unsigned int ID;	// ID from OpenGL (calls it a "name")
		std::string friendlyName;	// We give it this name
		// Eventually, will load all the unform variables and blocks
		void loadUniforms(void);	// 

		// Named Uniform Block 
		class CNamedUniformBlockDesc
		{
		public:
			CNamedUniformBlockDesc();
			std::string		programName;
			GLuint			programID;
			std::string		name;
			GLint			nameLength;			// GL_UNIFORM_BLOCK_NAME_LENGTH?
			GLint			bindingPoint;		// GL_UNIFORM_BLOCK_BINDING
			GLint			uniformBlockIndex;
			GLint			dataSizeInBytes;			// GL_UNIFORM_BLOCK_DATA_SIZE ("in basic machine units")
			GLint			numberOfActiveUniforms;		// GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS;
			std::vector< CNamedUniformBlockDesc > vecUniformVariables;
			bool bReferencedFromVertexShader;
			bool bReferencedFromTessControlShader;
			bool bReferencedFromTessEvaluationShader;
			bool bReferencedFromGeometryShader;
			bool bReferencedFromFragmentShader;
			bool bReferencedFromComputeShader;

			void Clear(void);
		};//class CNamedUniformBlockDesc



// *************************************************************
// START OF: Ignore this portion of the shader for now
//
		// Connects the OBJECTs textures to the bindings in the shader
		void set2DTextureBindings(std::vector<sTextureBindBlendInfo> &vec2DBindings, CTextureManager* pTextureManager);
		void setCubeTextureBindings(std::vector<sTextureBindBlendInfo> &vecCubeBindings, CTextureManager* pTextureManage);
		// Used to set up the current texture unit, sampler, and blend values
		struct sShaderTextureState	// Represents the state of a texture sampler+blend ratio in shader
		{	// -1 means invalid or not set yet
			static const int INVALID_VALUE = -1;
			sShaderTextureState() :
				texUnit(INVALID_VALUE), texID(INVALID_VALUE), sampler_LocID(INVALID_VALUE),
				blend_LocID(INVALID_VALUE), blendValue(0.0f),
				bIsInUseThisPass(false) {};
			// These values will change based on what texture is bound to this 'slot' (sampler+blend ratio)
			int texUnit;				// Current texture Unit being used
			int texID;				// Current Texture ID (from OpenGL)
			std::string name;		// Name of texture (friendly name)
									// These samplers values will be CONSTANT once shader is loaded
			int sampler_LocID;		// sampler uniform ID (in shader) - will be constant 
			int blend_LocID;		// blend ratio unform ID (in shader) - will be constant
			float blendValue;	// The blend to use for this texture (0.0 to 1.0)
			bool bIsInUseThisPass;			// true if in use. 
		};
		std::vector<sShaderTextureState> vec2DTexBindings;
		static const int CUBE_MAP_TEXTURE_UNIT_START_INDEX = 16;
		std::vector<sShaderTextureState> vecCubeTexBindings;
	private:
		// These are used to find and assign the objects wanted textures to the "slots" 
		//	in the shader. Note that the units are assigned based on the index
		bool m_FindAndAssign2DTexture( sTextureBindBlendInfo *pTexBlendInfo );
		bool m_Assign2DTextureToFirstEmptySlot( sTextureBindBlendInfo *pTexBlendInfo );
		bool m_bTextureSamplersLoaded;
//
//END OF: Ignore this portion
// ************************************************************
	}; // class cShaderProgram

	cShaderManager();
	~cShaderManager();

	bool useShaderProgram( unsigned int ID );
	bool useShaderProgram( std::string friendlyName );
	bool createProgramFromFile( std::string friendlyName, 
		                        cShader &vertexShad, 
					            cShader &fragShader );
// *********************
	bool createProgramFromFile(std::string friendlyName,
							   cShader &vertexShad,
							   cShader &geomShad,			// NEW for 2018! Geometry shading!
							   cShader &fragShader);
// *********************
	bool createProgramFromSource( std::string friendlyName, 
								  cShader &vertexShad,
								  cShader &fragShader );
	void setBasePath( std::string basepath );
	unsigned int getIDFromFriendlyName( std::string friendlyName );

	// Returns NULL if not found
	cShaderProgram* getShaderProgPointerFromID(unsigned int shaderID);
	cShaderProgram* getShaderProgPointerFromFriendlyName( std::string friendlyName );


	// Loads uniforms, vertex attributes, and NUBs
	bool LoadActiveShaderVariables(GLuint shaderID);
	bool LoadActiveShaderVariables(std::string shaderName);

	// Clears last error
	std::string getLastError(void);
private:
	// Returns an empty string if it didn't work
	bool m_loadSourceFromFile( cShader &shader );
	std::string m_basepath;

	bool m_compileShaderFromSource( cShader &shader, std::string &error );
	// returns false if no error
	bool m_wasThereACompileError( unsigned int shaderID, std::string &errorText );
	bool m_wasThereALinkError( unsigned int progID, std::string &errorText );

	std::string m_lastError;

	std::map< unsigned int /*ID*/, cShaderProgram > m_ID_to_Shader;
	std::map< std::string /*name*/, unsigned int /*ID*/ > m_name_to_ID;
};

#endif