#include "levikno.h"
#include "levikno_internal.h"


#define LVN_ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

#define LVN_CONFIG_GLYPH(ux0,uy0,ux1,uy1,bx,by,u,a) {{ux0/128.0f,uy0/128.0f,ux1/128.0f,uy1/128.0f},{ux1-ux0,uy1-uy0},{bx,by},u,a}

static const char* s_VertexShaderSrc = R"(
#version 460

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout (binding = 0) uniform ObjectBuffer
{
	mat4 u_ProjMat;
	mat4 u_ViewMat;
};

void main()
{
	gl_Position = u_ProjMat * u_ViewMat * vec4(inPos, 0.0, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}
)";

static const char* s_FragmentShaderSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
	outColor = vec4(fragColor * vec3(texture(inTexture, fragTexCoord)), 1.0);
}
)";

static const char* s_FragmentShaderFontSrc = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
	float text = texture(inTexture, fragTexCoord).r;
	outColor = vec4(vec3(text) * fragColor, text);
}
)";

struct LvnUniformData
{
	LvnMat4 projMat;
	LvnMat4 viewMat;
};

struct LvnVertexData2d
{
	LvnVec2 pos;
	LvnColor color;
	LvnVec2 texCoords;
};

namespace lvn
{

static LvnFont         getDefaultFont();
static LvnResult       createRendererResources(const LvnWindowCreateInfo* windowCreateInfo);
static LvnRenderMode   createRenderMode2d(const LvnRenderer* renderer, const LvnTexture* texture, const char* fragmentShaderSrc);
static void            renderModeDraw2d(LvnRenderer* renderer, LvnRenderMode& renderMode);


static LvnFont getDefaultFont()
{
	// 128x128 1 channel (red) image of font atlas
	// each pixel is storred per bit
	const uint32_t c_DefaultFontData[] =
	{
		0xc042142a,0xec410510,0xe388e41b,0x39f39f24,0x28af142a,0x0c454891,0x144d1418,0x45044125,0x24a2bf2a,0x09f38891,0x04091200,0x44804125,0xc2428a02,0x04455044,0xe4095200,0x3883cf24,
		0x21070a02,0x00411045,0x03095100,0x4444507d,0x248a1f82,0x00001042,0x00891100,0x44445021,0x2a4a0500,0x00001042,0x10491080,0x44245121,0xca278502,0x00001045,0xe7dce080,0x38238e20,
		0x04020000,0x00000880,0x00000040,0x00000000,0x00000000,0x00000880,0x00000040,0x00000000,0x00000000,0x00000500,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x0603049c,0x70f3870e,0x44e7df1e,0x4105171f,0x18f8c4a2,0x89144891,0x45104122,0x63049404,0x60003022,0x05145650,0x44104142,0x55045404,0x18f8c03c,0x04f7d548,0x7c13cf42,0x49043404,
		0x060304a0,0x05145544,0x45d04142,0x41045404,0x000004a0,0x05144f44,0x45104142,0x41049404,0x00000210,0x89144080,0x45104122,0x41051444,0x0000000c,0x70f44f04,0x44e05f1e,0x417d139f,
		0x00000000,0x00000000,0x00000000,0x00000000,0xc70f1c46,0x14517ce3,0x177d1451,0x0e37f11c,0x48912246,0x14511114,0x11411451,0x10400110,0x5051414a,0x14511014,0x2140a291,0x1e000290,
		0xd04f414a,0x145110e3,0x2130a111,0x11000290,0x50414152,0xa2911101,0x4108410a,0x11000450,0x54414152,0xa2911102,0x4104428a,0x1e000450,0x48812262,0x41111114,0x81044444,0x00000010,
		0x57011c62,0x410e10e4,0x817c4444,0x00000010,0x00000000,0x00000000,0x01000000,0x00000011,0x00000000,0x00000000,0x01000000,0x00000011,0x00000000,0x00000000,0x07000000,0x0000001c,
		0x00000000,0x00000000,0x00000000,0x00000000,0xe61c8382,0x8d90a205,0x9c6b8e71,0x04525144,0x11228442,0x52508005,0x829a528a,0x02925144,0x113ee04e,0x5254b305,0x8c0a528a,0x01154a44,
		0x13829052,0x5252a23d,0xd00a528a,0x01154a45,0xe1029452,0x5251a245,0x920a528a,0x028d8444,0x011c9392,0x5052a245,0x8c0b8e72,0x04488438,0x01009012,0x0014a245,0x80020200,0x00000000,
		0x1100e00e,0x0014a045,0x80020200,0x00000000,0xe0000000,0x00001800,0x80020200,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x306b0fa2,0x4569c108,0x21871239,0x067e38fd,
		0x48888822,0x44922382,0x92088045,0x09004480,0x80888622,0x29082549,0x4b974005,0x0900ba80,0x00888122,0x11082128,0x92514038,0x0600aa00,0x010840a2,0x7c902128,0x23914044,0x00009a01,
		0x02082fbc,0x1168f128,0x00174044,0x0000aa00,0x01084020,0x7c002548,0x00088039,0x00004400,0x00888022,0x10002388,0x00070041,0x00003800,0x0088801c,0x0003d100,0x00000045,0x00000000,
		0x00888000,0x00000000,0x00000038,0x00000000,0x006b0000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0xbc8b1988,0x42424c44,0x10308222,0x00080242,
		0x2e88a208,0x62649268,0x0c400242,0x001451a5,0x2e88193e,0x41491246,0x00008161,0x00080000,0x2c882088,0x41449240,0x1c708145,0x001c71c7,0x28881b88,0x8a824ce0,0xa2884aaa,0x00228a28,
		0x28780000,0x8a800000,0xa2882a88,0x00228a28,0x2808003e,0x4e400000,0xbefa2e44,0x003efbef,0x28080000,0x48400000,0xa289c84e,0x00228a28,0x00000000,0x00000000,0xa2880000,0x00228a28,
		0x00000000,0x00000000,0xa2880000,0x00228a28,0x00000000,0x00000000,0xa2880000,0x00228a28,0x00000000,0x00000000,0x00000000,0x00000000,0x0818e3f0,0x01081801,0x20189878,0x22004810,
		0x86211028,0xa28620a2,0x18206488,0x14283428,0x00000828,0x00000000,0x00000108,0x08000000,0xdf7c09e4,0xf7df7df7,0x38388d3d,0x14383838,0x4104083c,0x41041010,0x44448d08,0x22444444,
		0x41040822,0x41041010,0x82829508,0x00828282,0xcf3d1022,0x410410f3,0x82829488,0x00828282,0x4104e3e2,0x41041010,0x8282a478,0x00828282,0x41044000,0x41041010,0x8282a400,0x00828282,
		0x41048000,0x41041010,0x4444c400,0x00444444,0xdf7c6000,0xf7df7df7,0x3838c401,0x00383838,0x00000000,0x00000000,0x00000000,0x00000000,0x00840cb8,0x1031c090,0x00080242,0x0021031c,
		0x51431044,0x0c42238c,0x001451a5,0x1450c422,0x000000a2,0x0001a480,0x00080000,0x00000002,0x8a28a292,0x1c70a8a2,0x771c71c7,0x1c71c702,0x8a28a28a,0x208128a2,0x88208208,0x228a28a2,
		0x8a28a244,0x3cf22494,0xff3cf3cf,0x3efbef9c,0x8a28a23a,0xa28a6394,0x08a28a28,0x02082088,0x8a28a200,0xa289a088,0x08a28a28,0x02082090,0x8a28a200,0x3cf00008,0x773cf3cf,0x1c71c70c,
		0x8a28a200,0x00000008,0x00000000,0x00000000,0x71c71c00,0x00000008,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x65340486,0x10048420,0x00210300,0x00550024,
		0x8288aa68,0x00a34a18,0x1450c42c,0x000028e3,0x00160000,0x7c000000,0x00000012,0x005d0120,0xe3106666,0x00e38e38,0xa28a28a9,0x00104528,0x14ac4444,0x11145145,0xa28a28a5,0x004944e8,
		0x14a24444,0x01145145,0xa28a2892,0x00004428,0x14a24444,0x01145145,0xa28a288d,0x00494408,0x14a24444,0x01145145,0xa28a2880,0x00004408,0xe49c0000,0x00e38e38,0x1c71c700,0x0055780f,
		0x00000000,0x00000000,0x00000000,0x00004008,0x00000000,0x00000000,0x80000000,0x00004408,0x00000000,0x00000000,0x00000000,0x00003807,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
		0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	};

	LvnFontGlyph glyphs[] =
	{
		// ux0,uy0,ux1,uy1,width,height,bearingx,bearingy,unicode,advance
		LVN_CONFIG_GLYPH(0,0,0,0,0,0,32,6),
		LVN_CONFIG_GLYPH(1,0,2,8,0,0,33,3),
		LVN_CONFIG_GLYPH(3,0,6,3,0,1,34,5),
		LVN_CONFIG_GLYPH(7,0,14,8,0,0,35,9),
		LVN_CONFIG_GLYPH(15,0,20,9,0,1,36,7),
		LVN_CONFIG_GLYPH(21,0,28,9,0,1,37,9),
		LVN_CONFIG_GLYPH(29,0,35,8,0,0,38,8),
		LVN_CONFIG_GLYPH(36,0,37,3,0,1,39,3),
		LVN_CONFIG_GLYPH(38,0,41,11,0,2,40,5),
		LVN_CONFIG_GLYPH(42,0,45,11,0,2,41,5),
		LVN_CONFIG_GLYPH(46,0,51,5,0,-1,42,7),
		LVN_CONFIG_GLYPH(52,0,57,5,0,-1,43,7),
		LVN_CONFIG_GLYPH(58,0,60,4,0,-6,44,4),
		LVN_CONFIG_GLYPH(61,0,66,1,0,-3,45,7),
		LVN_CONFIG_GLYPH(67,0,69,2,0,-6,46,4),
		LVN_CONFIG_GLYPH(70,0,75,10,0,1,47,7),
		LVN_CONFIG_GLYPH(76,0,81,8,0,0,48,7),
		LVN_CONFIG_GLYPH(82,0,85,8,0,0,49,5),
		LVN_CONFIG_GLYPH(86,0,91,8,0,0,50,7),
		LVN_CONFIG_GLYPH(92,0,97,8,0,0,51,7),
		LVN_CONFIG_GLYPH(98,0,103,8,0,0,52,7),
		LVN_CONFIG_GLYPH(104,0,109,8,0,0,53,7),
		LVN_CONFIG_GLYPH(110,0,115,8,0,0,54,7),
		LVN_CONFIG_GLYPH(116,0,121,8,0,0,55,7),
		LVN_CONFIG_GLYPH(122,0,127,8,0,0,56,7),
		LVN_CONFIG_GLYPH(1,12,6,20,0,0,57,7),
		LVN_CONFIG_GLYPH(7,12,8,18,0,-1,58,3),
		LVN_CONFIG_GLYPH(9,12,11,19,0,-1,59,4),
		LVN_CONFIG_GLYPH(12,12,18,17,0,-1,60,8),
		LVN_CONFIG_GLYPH(19,12,24,16,0,-1,61,7),
		LVN_CONFIG_GLYPH(25,12,31,17,0,-1,62,8),
		LVN_CONFIG_GLYPH(32,12,37,20,0,0,63,7),
		LVN_CONFIG_GLYPH(38,12,45,20,0,0,64,9),
		LVN_CONFIG_GLYPH(46,12,51,20,0,0,65,7),
		LVN_CONFIG_GLYPH(52,12,57,20,0,0,66,7),
		LVN_CONFIG_GLYPH(58,12,64,20,0,0,67,8),
		LVN_CONFIG_GLYPH(65,12,71,20,0,0,68,8),
		LVN_CONFIG_GLYPH(72,12,77,20,0,0,69,7),
		LVN_CONFIG_GLYPH(78,12,83,20,0,0,70,7),
		LVN_CONFIG_GLYPH(84,12,89,20,0,0,71,7),
		LVN_CONFIG_GLYPH(90,12,95,20,0,0,72,7),
		LVN_CONFIG_GLYPH(96,12,101,20,0,0,73,7),
		LVN_CONFIG_GLYPH(102,12,107,20,0,0,74,7),
		LVN_CONFIG_GLYPH(108,12,113,20,0,0,75,7),
		LVN_CONFIG_GLYPH(114,12,119,20,0,0,76,7),
		LVN_CONFIG_GLYPH(120,12,127,20,0,0,77,9),
		LVN_CONFIG_GLYPH(1,21,7,29,0,0,78,8),
		LVN_CONFIG_GLYPH(8,21,15,29,0,0,79,9),
		LVN_CONFIG_GLYPH(16,21,21,29,0,0,80,7),
		LVN_CONFIG_GLYPH(22,21,29,29,0,0,81,9),
		LVN_CONFIG_GLYPH(30,21,35,29,0,0,82,7),
		LVN_CONFIG_GLYPH(36,21,41,29,0,0,83,7),
		LVN_CONFIG_GLYPH(42,21,47,29,0,0,84,7),
		LVN_CONFIG_GLYPH(48,21,53,29,0,0,85,7),
		LVN_CONFIG_GLYPH(54,21,59,29,0,0,86,7),
		LVN_CONFIG_GLYPH(60,21,69,29,0,0,87,11),
		LVN_CONFIG_GLYPH(70,21,75,29,0,0,88,7),
		LVN_CONFIG_GLYPH(76,21,81,29,0,0,89,7),
		LVN_CONFIG_GLYPH(82,21,87,29,0,0,90,7),
		LVN_CONFIG_GLYPH(88,21,91,32,0,2,91,5),
		LVN_CONFIG_GLYPH(92,21,97,31,0,1,92,7),
		LVN_CONFIG_GLYPH(98,21,101,32,0,2,93,5),
		LVN_CONFIG_GLYPH(102,21,107,27,0,2,94,7),
		LVN_CONFIG_GLYPH(108,21,115,22,0,-7,95,9),
		LVN_CONFIG_GLYPH(116,21,119,23,0,2,96,5),
		LVN_CONFIG_GLYPH(120,21,125,27,0,-2,97,7),
		LVN_CONFIG_GLYPH(1,33,5,41,0,0,98,6),
		LVN_CONFIG_GLYPH(6,33,11,39,0,-2,99,7),
		LVN_CONFIG_GLYPH(12,33,16,41,0,0,100,6),
		LVN_CONFIG_GLYPH(17,33,22,39,0,-2,101,7),
		LVN_CONFIG_GLYPH(23,33,27,41,0,0,102,6),
		LVN_CONFIG_GLYPH(28,33,33,42,0,-3,103,7),
		LVN_CONFIG_GLYPH(34,33,39,41,0,0,104,7),
		LVN_CONFIG_GLYPH(40,33,42,40,0,-1,105,4),
		LVN_CONFIG_GLYPH(43,33,46,42,0,-1,106,5),
		LVN_CONFIG_GLYPH(47,33,51,41,0,0,107,6),
		LVN_CONFIG_GLYPH(52,33,53,41,0,0,108,3),
		LVN_CONFIG_GLYPH(54,33,61,39,0,-2,109,9),
		LVN_CONFIG_GLYPH(62,33,66,39,0,-2,110,6),
		LVN_CONFIG_GLYPH(67,33,72,39,0,-2,111,7),
		LVN_CONFIG_GLYPH(73,33,77,42,0,-2,112,6),
		LVN_CONFIG_GLYPH(78,33,82,42,0,-2,113,6),
		LVN_CONFIG_GLYPH(83,33,88,39,0,-2,114,7),
		LVN_CONFIG_GLYPH(89,33,93,39,0,-2,115,6),
		LVN_CONFIG_GLYPH(94,33,97,42,0,1,116,5),
		LVN_CONFIG_GLYPH(98,33,103,39,0,-2,117,7),
		LVN_CONFIG_GLYPH(104,33,109,39,0,-2,118,7),
		LVN_CONFIG_GLYPH(110,33,117,39,0,-2,119,9),
		LVN_CONFIG_GLYPH(118,33,123,39,0,-2,120,7),
		LVN_CONFIG_GLYPH(1,43,6,52,0,-2,121,7),
		LVN_CONFIG_GLYPH(7,43,12,49,0,-2,122,7),
		LVN_CONFIG_GLYPH(13,43,18,54,0,2,123,7),
		LVN_CONFIG_GLYPH(19,43,20,54,0,2,124,3),
		LVN_CONFIG_GLYPH(21,43,26,54,0,2,125,7),
		LVN_CONFIG_GLYPH(27,43,34,46,0,-2,126,9),
		LVN_CONFIG_GLYPH(0,0,0,0,0,0,160,6),
		LVN_CONFIG_GLYPH(35,43,36,51,0,0,161,3),
		LVN_CONFIG_GLYPH(37,43,43,52,0,1,162,8),
		LVN_CONFIG_GLYPH(44,43,50,52,0,1,163,8),
		LVN_CONFIG_GLYPH(51,43,57,49,0,0,164,8),
		LVN_CONFIG_GLYPH(58,43,63,51,0,0,165,7),
		LVN_CONFIG_GLYPH(64,43,65,52,0,1,166,3),
		LVN_CONFIG_GLYPH(66,43,71,53,0,1,167,7),
		LVN_CONFIG_GLYPH(73,43,77,45,0,1,168,6),
		LVN_CONFIG_GLYPH(78,43,85,51,0,0,169,9),
		LVN_CONFIG_GLYPH(86,43,90,48,0,1,170,6),
		LVN_CONFIG_GLYPH(91,43,97,48,0,-1,171,8),
		LVN_CONFIG_GLYPH(98,43,104,46,0,-3,172,8),
		LVN_CONFIG_GLYPH(61,0,66,1,0,-3,173,7),
		LVN_CONFIG_GLYPH(105,43,112,51,0,0,174,9),
		LVN_CONFIG_GLYPH(113,43,119,44,0,1,175,8),
		LVN_CONFIG_GLYPH(120,43,124,47,0,1,176,6),
		LVN_CONFIG_GLYPH(1,55,6,62,0,-1,177,7),
		LVN_CONFIG_GLYPH(7,55,10,60,0,1,178,5),
		LVN_CONFIG_GLYPH(11,55,14,60,0,1,179,5),
		LVN_CONFIG_GLYPH(15,55,18,57,0,1,180,5),
		LVN_CONFIG_GLYPH(19,55,24,63,0,-2,181,7),
		LVN_CONFIG_GLYPH(25,55,30,63,0,0,182,7),
		LVN_CONFIG_GLYPH(31,55,32,56,0,-3,183,3),
		LVN_CONFIG_GLYPH(33,55,36,58,0,-8,184,5),
		LVN_CONFIG_GLYPH(37,55,40,60,0,1,185,5),
		LVN_CONFIG_GLYPH(41,55,45,60,0,1,186,6),
		LVN_CONFIG_GLYPH(46,55,52,60,0,-1,187,8),
		LVN_CONFIG_GLYPH(53,55,60,63,0,0,188,9),
		LVN_CONFIG_GLYPH(61,55,68,63,0,0,189,9),
		LVN_CONFIG_GLYPH(69,55,76,63,0,0,190,9),
		LVN_CONFIG_GLYPH(77,55,82,63,0,0,191,7),
		LVN_CONFIG_GLYPH(83,55,88,66,0,3,192,7),
		LVN_CONFIG_GLYPH(89,55,94,66,0,3,193,7),
		LVN_CONFIG_GLYPH(95,55,100,66,0,3,194,7),
		LVN_CONFIG_GLYPH(101,55,106,66,0,3,195,7),
		LVN_CONFIG_GLYPH(107,55,112,66,0,3,196,7),
		LVN_CONFIG_GLYPH(113,55,118,66,0,3,197,7),
		LVN_CONFIG_GLYPH(1,67,10,75,0,0,198,11),
		LVN_CONFIG_GLYPH(11,67,17,78,0,0,199,8),
		LVN_CONFIG_GLYPH(18,67,23,78,0,3,200,7),
		LVN_CONFIG_GLYPH(24,67,29,78,0,3,201,7),
		LVN_CONFIG_GLYPH(30,67,35,78,0,3,202,7),
		LVN_CONFIG_GLYPH(36,67,41,78,0,3,203,7),
		LVN_CONFIG_GLYPH(42,67,47,78,0,3,204,7),
		LVN_CONFIG_GLYPH(48,67,53,78,0,3,205,7),
		LVN_CONFIG_GLYPH(54,67,59,78,0,3,206,7),
		LVN_CONFIG_GLYPH(60,67,65,78,0,3,207,7),
		LVN_CONFIG_GLYPH(66,67,73,78,0,0,208,9),
		LVN_CONFIG_GLYPH(74,67,80,78,0,3,209,8),
		LVN_CONFIG_GLYPH(81,67,88,78,0,3,210,9),
		LVN_CONFIG_GLYPH(89,67,96,78,0,3,211,9),
		LVN_CONFIG_GLYPH(97,67,104,78,0,3,212,9),
		LVN_CONFIG_GLYPH(105,67,112,78,0,3,213,9),
		LVN_CONFIG_GLYPH(113,67,120,78,0,3,214,9),
		LVN_CONFIG_GLYPH(121,67,126,78,0,-2,215,7),
		LVN_CONFIG_GLYPH(1,79,8,86,0,-1,216,9),
		LVN_CONFIG_GLYPH(9,79,14,90,0,3,217,7),
		LVN_CONFIG_GLYPH(15,79,20,90,0,3,218,7),
		LVN_CONFIG_GLYPH(21,79,26,90,0,3,219,7),
		LVN_CONFIG_GLYPH(27,79,32,90,0,3,220,7),
		LVN_CONFIG_GLYPH(33,79,38,90,0,3,221,7),
		LVN_CONFIG_GLYPH(39,79,44,87,0,0,222,7),
		LVN_CONFIG_GLYPH(45,79,50,87,0,0,223,7),
		LVN_CONFIG_GLYPH(51,79,56,88,0,1,224,7),
		LVN_CONFIG_GLYPH(57,79,62,88,0,1,225,7),
		LVN_CONFIG_GLYPH(63,79,68,88,0,1,226,7),
		LVN_CONFIG_GLYPH(69,79,74,88,0,1,227,7),
		LVN_CONFIG_GLYPH(75,79,80,88,0,1,228,7),
		LVN_CONFIG_GLYPH(81,79,86,88,0,1,229,7),
		LVN_CONFIG_GLYPH(87,79,96,88,0,1,230,11),
		LVN_CONFIG_GLYPH(97,79,102,88,0,-2,231,7),
		LVN_CONFIG_GLYPH(103,79,108,88,0,1,232,7),
		LVN_CONFIG_GLYPH(109,79,114,88,0,1,233,7),
		LVN_CONFIG_GLYPH(115,79,120,88,0,1,234,7),
		LVN_CONFIG_GLYPH(121,79,126,88,0,1,235,7),
		LVN_CONFIG_GLYPH(1,91,4,99,0,0,236,5),
		LVN_CONFIG_GLYPH(5,91,8,99,0,0,237,5),
		LVN_CONFIG_GLYPH(9,91,12,99,0,0,238,5),
		LVN_CONFIG_GLYPH(13,91,16,99,0,0,239,5),
		LVN_CONFIG_GLYPH(17,91,22,100,0,1,240,7),
		LVN_CONFIG_GLYPH(23,91,27,100,0,1,241,6),
		LVN_CONFIG_GLYPH(28,91,33,100,0,1,242,7),
		LVN_CONFIG_GLYPH(34,91,39,100,0,1,243,7),
		LVN_CONFIG_GLYPH(40,91,45,100,0,1,244,7),
		LVN_CONFIG_GLYPH(46,91,51,100,0,1,245,7),
		LVN_CONFIG_GLYPH(52,91,57,100,0,1,246,7),
		LVN_CONFIG_GLYPH(58,91,63,96,0,-1,247,7),
		LVN_CONFIG_GLYPH(64,91,70,98,0,-1,248,8),
		LVN_CONFIG_GLYPH(71,91,76,100,0,1,249,7),
		LVN_CONFIG_GLYPH(77,91,82,100,0,1,250,7),
		LVN_CONFIG_GLYPH(83,91,88,100,0,1,251,7),
		LVN_CONFIG_GLYPH(89,91,94,100,0,1,252,7),
		LVN_CONFIG_GLYPH(95,91,100,103,0,1,253,7),
		LVN_CONFIG_GLYPH(101,91,105,97,0,-2,254,6),
		LVN_CONFIG_GLYPH(106,91,111,103,0,1,255,7),
	};

	uint8_t* imgbuff = (uint8_t*)LVN_ALLOC(128 * 128);

	for (uint32_t i = 0; i < LVN_ARRAY_LEN(c_DefaultFontData); i++)
	{
		for(uint32_t j = 0; j < 32; j++)
		{
			uint8_t px = (c_DefaultFontData[i] & (1 << j)) ? 255 : 0;
			imgbuff[i * 32 + j] = px;
		}
	}

	LvnImageData imageData{};
	imageData.pixels = LvnData<uint8_t>(imgbuff, 128 * 128);
	imageData.width = 128;
	imageData.height = 128;
	imageData.channels = 1;
	imageData.size = 128 * 128 * 1;

	LvnFont font{};
	font.fontSize = 13;
	font.atlas = imageData;
	font.glyphs = LvnData<LvnFontGlyph>(glyphs, LVN_ARRAY_LEN(glyphs));
	font.codepoints = lvn::getDefaultSupportedCodepoints();

	LVN_FREE(imgbuff);

	return font;
}

static LvnResult createRendererResources(const LvnWindowCreateInfo* windowCreateInfo)
{
	LvnContext* lvnctx = lvn::getContext();

	if (lvnctx->renderer)
		return Lvn_Result_AlreadyCalled;

	lvnctx->renderer = std::make_unique<LvnRenderer>();
	LvnRenderer* renderer = lvnctx->renderer.get();

	if (lvn::createWindow(&renderer->window, windowCreateInfo) != Lvn_Result_Success)
		return Lvn_Result_Failure;

	// set background clear color
	renderer->clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	// texture
	uint8_t whiteTextureData[] = { 0xff, 0xff, 0xff, 0xff };
	LvnImageData imageData;
	imageData.pixels = LvnData<uint8_t>(whiteTextureData, sizeof(whiteTextureData) / sizeof(uint8_t));
	imageData.width = 1;
	imageData.height = 1;
	imageData.channels = 4;
	imageData.size = 4;

	LvnTextureCreateInfo textureCreateInfo{};
	textureCreateInfo.imageData = imageData;
	textureCreateInfo.format = Lvn_TextureFormat_Unorm;
	textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
	textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;
	textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
	textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;

	// create texture
	lvn::createTexture(&renderer->defaultWhiteTexture, &textureCreateInfo);

	// load default font
	renderer->defaultFont = lvn::getDefaultFont();
	textureCreateInfo.imageData = renderer->defaultFont.atlas;
	lvn::createTexture(&renderer->defaultFontTexture, &textureCreateInfo);

	// render modes
	renderer->renderModes.resize(Lvn_RenderMode_Max_Value);
	renderer->renderModes[Lvn_RenderMode_2d] = std::move(lvn::createRenderMode2d(renderer, renderer->defaultWhiteTexture, s_FragmentShaderSrc));
	renderer->renderModes[Lvn_RenderMode_2dText] = std::move(lvn::createRenderMode2d(renderer, renderer->defaultFontTexture, s_FragmentShaderFontSrc));


	return Lvn_Result_Success;
}

static LvnRenderMode createRenderMode2d(const LvnRenderer* renderer, const LvnTexture* texture, const char* fragmentShaderSrc)
{
	LvnRenderMode renderMode{};
	renderMode.maxVertexCount = 5000;
	renderMode.maxIndexCount = 5000;

	uint32_t stride = sizeof(LvnVertexData2d);

	// create buffer
	LvnBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
	bufferCreateInfo.usage = Lvn_BufferUsage_Dynamic;
	bufferCreateInfo.data = nullptr;
	bufferCreateInfo.size = renderMode.maxVertexCount * stride + renderMode.maxIndexCount * sizeof(uint32_t);

	lvn::createBuffer(&renderMode.buffer, &bufferCreateInfo);
	renderMode.indexOffset = renderMode.maxVertexCount * stride;

	// attributes and bindings

	LvnVertexBindingDescription bindingDescriptions[] = {LvnVertexBindingDescription{ 0, stride }};
	LvnVertexAttribute attributes[] =
	{
		{ 0, Lvn_AttributeLocation_Position, Lvn_AttributeFormat_Vec2_f32, 0 },
		{ 0, Lvn_AttributeLocation_Color, Lvn_AttributeFormat_Vec3_un8, (2 * sizeof(float)) },
		{ 0, Lvn_AttributeLocation_TexCoords, Lvn_AttributeFormat_Vec2_f32, (2 * sizeof(float) + 4 * sizeof(uint8_t)) },
	};

	// create pipeline
	LvnShaderCreateInfo shaderCreateInfo{};
	shaderCreateInfo.vertexSrc = s_VertexShaderSrc;
	shaderCreateInfo.fragmentSrc = fragmentShaderSrc;

	LvnShader* shader;
	lvn::createShaderFromSrc(&shader, &shaderCreateInfo);

	// descriptor binding
	LvnDescriptorBinding descriptorUniformBinding{};
	descriptorUniformBinding.binding = 0;
	descriptorUniformBinding.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformBinding.shaderStage = Lvn_ShaderStage_Vertex;
	descriptorUniformBinding.descriptorCount = 1;
	descriptorUniformBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorTextureBinding{};
	descriptorTextureBinding.binding = 1;
	descriptorTextureBinding.descriptorType = Lvn_DescriptorType_ImageSampler;
	descriptorTextureBinding.shaderStage = Lvn_ShaderStage_Fragment;
	descriptorTextureBinding.descriptorCount = 1;
	descriptorTextureBinding.maxAllocations = 1;

	LvnDescriptorBinding descriptorBindings[] =
	{
		descriptorUniformBinding, descriptorTextureBinding,
	};

	// descriptor layout
	LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
	descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;
	descriptorLayoutCreateInfo.descriptorBindingCount = LVN_ARRAY_LEN(descriptorBindings);
	descriptorLayoutCreateInfo.maxSets = 1;

	lvn::createDescriptorLayout(&renderMode.descriptorLayout, &descriptorLayoutCreateInfo);
	lvn::allocateDescriptorSet(&renderMode.descriptorSet, renderMode.descriptorLayout);

	LvnRenderPass* renderPass = lvn::windowGetRenderPass(renderer->window);
	LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();

	// pipeline create info struct
	LvnPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
	pipelineCreateInfo.pVertexAttributes = attributes;
	pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
	pipelineCreateInfo.pVertexBindingDescriptions = bindingDescriptions;
	pipelineCreateInfo.vertexBindingDescriptionCount = LVN_ARRAY_LEN(bindingDescriptions);
	pipelineCreateInfo.pDescriptorLayouts = &renderMode.descriptorLayout;
	pipelineCreateInfo.descriptorLayoutCount = 1;
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = renderPass;

	// create pipeline
	lvn::createPipeline(&renderMode.pipeline, &pipelineCreateInfo);
	lvn::destroyShader(shader);


	// uniform buffer
	LvnUniformBufferCreateInfo uniformBufferCreateInfo{};
	uniformBufferCreateInfo.type = Lvn_BufferType_Uniform;
	uniformBufferCreateInfo.size = sizeof(LvnUniformData);

	// create uniform buffer
	lvn::createUniformBuffer(&renderMode.uniformBuffer, &uniformBufferCreateInfo);


	// update descriptor set
	LvnUniformBufferInfo bufferInfo{};
	bufferInfo.buffer = renderMode.uniformBuffer;
	bufferInfo.range = sizeof(LvnUniformData);
	bufferInfo.offset = 0;

	LvnDescriptorUpdateInfo descriptorUniformUpdateInfo{};
	descriptorUniformUpdateInfo.descriptorType = Lvn_DescriptorType_UniformBuffer;
	descriptorUniformUpdateInfo.binding = 0;
	descriptorUniformUpdateInfo.descriptorCount = 1;
	descriptorUniformUpdateInfo.bufferInfo = &bufferInfo;

	LvnDescriptorUpdateInfo descriptorTextureUpdateInfo{};
	descriptorTextureUpdateInfo.descriptorType = Lvn_DescriptorType_ImageSampler;
	descriptorTextureUpdateInfo.binding = 1;
	descriptorTextureUpdateInfo.descriptorCount = 1;
	descriptorTextureUpdateInfo.pTextureInfos = &texture;

	LvnDescriptorUpdateInfo descriptorUpdateInfos[] = { descriptorUniformUpdateInfo, descriptorTextureUpdateInfo, };
	lvn::updateDescriptorSetData(renderMode.descriptorSet, descriptorUpdateInfos, LVN_ARRAY_LEN(descriptorUpdateInfos));


	renderMode.drawFunc = lvn::renderModeDraw2d;

	return renderMode;
}

static void renderModeDraw2d(LvnRenderer* renderer, LvnRenderMode& renderMode)
{
	if (renderMode.drawList.empty())
		return;

	int width, height;
	lvn::windowGetSize(renderer->window, &width, &height);

	LvnUniformData uniformData{};
	uniformData.projMat = lvn::ortho((float)width * -0.5f, (float)width * 0.5f, (float)height * -0.5f, (float)height * 0.5f, -1.0f, 1.0f);
	uniformData.viewMat = LvnMat4(1.0f);

	lvn::updateUniformBufferData(renderMode.uniformBuffer, &uniformData, sizeof(LvnUniformData), 0);

	lvn::bufferUpdateData(renderMode.buffer, renderMode.drawList.vertices(), renderMode.drawList.vertex_size(), 0);
	lvn::bufferUpdateData(renderMode.buffer, renderMode.drawList.indices(), renderMode.drawList.index_size(), renderMode.indexOffset);

	lvn::renderCmdBindPipeline(renderer->window, renderMode.pipeline);
	lvn::renderCmdBindDescriptorSets(renderer->window, renderMode.pipeline, 0, 1, &renderMode.descriptorSet);

	lvn::renderCmdBindVertexBuffer(renderer->window, 0, 1, &renderMode.buffer, 0);
	lvn::renderCmdBindIndexBuffer(renderer->window, renderMode.buffer, renderMode.indexOffset);

	lvn::renderCmdDrawIndexed(renderer->window, renderMode.drawList.index_count());
}


LvnResult renderInit(const char* title, int width, int height)
{
	LvnWindowCreateInfo windowCreateInfo = lvn::configWindowInit(title, width, height);
	return createRendererResources(&windowCreateInfo);
}

LvnResult renderInit(const LvnWindowCreateInfo* createInfo)
{
	return createRendererResources(createInfo);
}

void renderTerminate()
{
	LvnContext* lvnctx = lvn::getContext();

	if (!lvnctx->renderer)
		return;

	LvnRenderer* renderer = lvnctx->renderer.get();

	for (auto& renderMode : renderer->renderModes)
	{
		lvn::destroyPipeline(renderMode.pipeline);
		lvn::destroyDescriptorLayout(renderMode.descriptorLayout);
		lvn::destroyBuffer(renderMode.buffer);
		lvn::destroyUniformBuffer(renderMode.uniformBuffer);
	}

	lvn::destroyTexture(renderer->defaultWhiteTexture);
	lvn::destroyTexture(renderer->defaultFontTexture);
	lvn::destroyWindow(renderer->window);

	lvnctx->renderer.reset(nullptr);
}

LvnWindow* getRendererWindow()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	return renderer->window;
}

bool renderWindowOpen()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	return lvn::windowOpen(renderer->window);
}

void drawBegin()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	lvn::windowUpdate(renderer->window);

	for (auto& renderMode : renderer->renderModes)
		renderMode.drawList.clear();

	lvn::renderBeginNextFrame(renderer->window);
	lvn::renderBeginCommandRecording(renderer->window);
	lvn::renderCmdBeginRenderPass(renderer->window, renderer->clearColor.r, renderer->clearColor.g, renderer->clearColor.b, renderer->clearColor.a);
}

void drawEnd()
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();

	for (auto& renderMode : renderer->renderModes)
	{
		renderMode.drawFunc(renderer, renderMode);
	}

	lvn::renderCmdEndRenderPass(renderer->window);
	lvn::renderEndCommandRecording(renderer->window);
	lvn::renderDrawSubmit(renderer->window);
}

void drawClearColor(float r, float g, float b, float a)
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->clearColor = { r, g, b, a };
}

void drawClearColor(const LvnColor& color)
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->clearColor = { (float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f, (float)color.a/255.0f };
}

void drawTriangle(const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color)
{
	LvnVertexData2d vertices[] =
	{
		{ v1, color, {0.0f, 0.0f} },
		{ v2, color, {0.5f, 1.0f} },
		{ v3, color, {1.0f, 0.0f} },
	};

	uint32_t indices[] = { 0, 1, 2 };

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexCount = 3;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 3;
	drawCmd.vertexStride = sizeof(LvnVertexData2d);

	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->renderModes[Lvn_RenderMode_2d].drawList.push_back(drawCmd);
}

void drawRect(const LvnVec2& pos, const LvnVec2& size, const LvnColor& color)
{
	LvnVertexData2d vertices[] =
	{
		{{ pos.x, pos.y + size.y },          color, {0.0f, 1.0f} },
		{{ pos.x, pos.y },                   color, {0.0f, 0.0f} },
		{{ pos.x + size.x, pos.y },          color, {1.0f, 0.0f} },
		{{ pos.x + size.x, pos.y + size.y }, color, {1.0f, 1.0f} },
	};

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices;
	drawCmd.vertexCount = 4;
	drawCmd.pIndices = indices;
	drawCmd.indexCount = 6;
	drawCmd.vertexStride = sizeof(LvnVertexData2d);

	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->renderModes[Lvn_RenderMode_2d].drawList.push_back(drawCmd);
}

void drawCircle(const LvnVec2& pos, float radius, const LvnColor& color)
{
	lvn::drawPolyNgon(pos, radius, 36, color);
}

void drawPolyNgon(const LvnVec2& pos, float radius, uint32_t nSides, const LvnColor& color)
{
	if (nSides < 3)
		return;

	std::vector<LvnVertexData2d> vertices(nSides + 1);
	std::vector<uint32_t> indices(nSides * 3);

	float twoPi = (float)(2 * M_PI);
	float angle = twoPi / (float)nSides;

	vertices[0] = { pos, color, {0.5f,0.5f} };

	for (uint32_t i = 0; i < nSides; i++)
	{
		float circlex = cos(i * angle);
		float circley = sin(i * angle);

		float posx = pos.x + (radius * circlex);
		float posy = pos.y + (radius * circley);

		vertices[i + 1] = { {posx,posy}, color, {(circlex + 1) * 0.5f , (circley + 1) * 0.5f} };
		indices[i * 3 + 0] = (0);
		indices[i * 3 + 1] = (i + 1);
		indices[i * 3 + 2] = (i + 1) % nSides + 1;
	}

	LvnDrawCommand drawCmd{};
	drawCmd.pVertices = vertices.data();
	drawCmd.vertexCount = vertices.size();
	drawCmd.pIndices = indices.data();
	drawCmd.indexCount = indices.size();
	drawCmd.vertexStride = sizeof(LvnVertexData2d);

	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	renderer->renderModes[Lvn_RenderMode_2d].drawList.push_back(drawCmd);
}

void drawText(const char* text, const LvnVec2& pos, const LvnColor& color, float scale)
{
	LvnRenderer* renderer = lvn::getContext()->renderer.get();
	LvnVec2 pen = pos;

	for (uint32_t i = 0; i < strlen(text);)
	{
		uint32_t codePointBytes = 0;
		uint32_t codepoint = lvn::decodeCodepointUTF8(&text[i], &codePointBytes);
		LvnFontGlyph glyph = lvn::fontGetGlyph(renderer->defaultFont, codepoint);
		i += codePointBytes;

		if (codepoint == '\n')
		{
			pen.y -= renderer->defaultFont.fontSize * scale;
			pen.x = pos.x;
			continue;
		}

		float xpos = pen.x + glyph.bearing.x * scale;
		float ypos = pen.y - (glyph.size.y - glyph.bearing.y) * scale;

		float w = glyph.size.x * scale;
		float h = glyph.size.y * scale;

		pen.x += glyph.advance * scale;

		LvnVertexData2d vertices[] =
		{
			/*         pos         | color |         texUVs            */
			{ {xpos, ypos + h},      color, {glyph.uv.x0, glyph.uv.y0} },
			{ {xpos, ypos},          color, {glyph.uv.x0, glyph.uv.y1} },
			{ {xpos + w, ypos + h},  color, {glyph.uv.x1, glyph.uv.y0} },
			{ {xpos + w, ypos},      color, {glyph.uv.x1, glyph.uv.y1} },
		};

		uint32_t indices[] =
		{
			0, 1, 2,
			2, 1, 3,
		};

		LvnDrawCommand drawCmd{};
		drawCmd.pVertices = vertices;
		drawCmd.vertexCount = 4;
		drawCmd.pIndices = indices;
		drawCmd.indexCount = 6;
		drawCmd.vertexStride = sizeof(LvnVertexData2d);

		renderer->renderModes[Lvn_RenderMode_2dText].drawList.push_back(drawCmd);
	}
}

} /* namespace lvn */
