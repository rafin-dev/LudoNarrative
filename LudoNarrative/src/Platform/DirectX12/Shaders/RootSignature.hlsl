//
// ===========================
// Root Signature Layout (CPU)
// ===========================
// -> 0: VertexShaderMatrices
// {
//   -> 0: float4x4 "ViewProjectionMatrix"
//   -> 16: float4x4 "ModelMatrix"
// }
//
// ===========================
// Root Signature Layout (GPU)
// ===========================
// -> b0: VertexShaderMatrices
// {
//   -> 0: float4x4 "ViewProjectionMatrix"
//   -> 16: float4x4 "ModelMatrix"
// }
//

#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
"RootConstants(num32BitConstants=32, b0), " \
"CBV(b1)"