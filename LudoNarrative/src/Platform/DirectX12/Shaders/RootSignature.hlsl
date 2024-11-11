//
// ===========================
// Root Signature Layout (CPU)
// ===========================
// -> 0: uint2 "Size"
//
// ===========================
// Root Signature Layout (GPU)
// ===========================
// -> b0: uint2 "Size"
//

#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
"RootConstants(num32BitConstants=16, b0)"