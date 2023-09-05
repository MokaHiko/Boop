#ifndef TINYOBJIMPL_H
#define TINYOBJIMPL_H

#pragma once

#include <tiny_obj_loader.h>
#include "mesh.h"

namespace boop
{
	void pack_vertex(boop::Vertex_F32_PNCV &new_vert, tinyobj::real_t vx, tinyobj::real_t vy, tinyobj::real_t vz, tinyobj::real_t nx, tinyobj::real_t ny, tinyobj::real_t nz, tinyobj::real_t ux, tinyobj::real_t uy);

	template<typename VertexFormat>
	void extract_mesh_from_obj(tinyobj::attrib_t &attrb, std::vector<tinyobj::shape_t> &shapes, std::vector<VertexFormat> &vertices, std::vector<uint32_t> &indices);
}

#endif