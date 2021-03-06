#pragma once
#include "GLProgram.h"

struct GLBufferInfo
{
	u32 prog_id;
	u32 fp_id;
	u32 vp_id;
	Array<u8> fp_data;
	Array<u32> vp_data;
	ArrayString fp_shader;
	ArrayString vp_shader;
};

struct GLProgramBuffer
{
	Array<GLBufferInfo> m_buf;

	int SearchFp(const RSXShaderProgram& rsx_fp, GLShaderProgram& gl_fp);
	int SearchVp(const RSXVertexProgram& rsx_vp, GLVertexProgram& gl_vp);

	bool CmpVP(const u32 a, const u32 b) const;
	bool CmpFP(const u32 a, const u32 b) const;

	u32 GetProg(u32 fp, u32 vp) const;

	void Add(GLProgram& prog, GLShaderProgram& gl_fp, RSXShaderProgram& rsx_fp, GLVertexProgram& gl_vp, RSXVertexProgram& rsx_vp);
	void Clear();
};