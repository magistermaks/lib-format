
#pragma once
#include "external.hpp"

enum BinaryNode : uint8_t {

	// compounds
	DICT   = 0xF4,
	LIST   = 0xE4,
	TEXT   = 0xD4,
	BLOB   = 0xC4,

	// numerical
	FLOAT  = 0x14, // float
	DOUBLE = 0x18, // double
	BYTE   = 0x21, // byte
	SHORT  = 0x22, // short
	INT    = 0x24, // int
	LONG   = 0x28, // long

/*
	// vectors
	VEC2F  = 0x38, // 2x float
	VEC3F  = 0x3C, // 3x float
	VEC2I  = 0x48, // 2x int
	VEC3I  = 0x4C, // 3x int
	VEC2S  = 0x34, // 2x short
	VEC3S  = 0x36, // 3x short
	VEC4S  = 0x58, // 4x short
	VEC2B  = 0x32, // 2x byte
	VEC3B  = 0x33, // 3x byte
	VEC4B  = 0x54, // 4x byte
*/

};
