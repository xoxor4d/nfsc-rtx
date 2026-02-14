texture DIFFUSEMAP_TEXTURE;

float4x4	cmWorldViewProj			: WorldViewProj;

shared float4	cvDiffuseMin      : register(c21);
shared float4	cvDiffuseRange    : register(c22);
shared float4   cvPowers          : register(c25);
shared float4   cvClampAndScales  : register(c26);

struct VS_INPUT
{
    float4 position	: POSITION;
    float4 texcoord	: TEXCOORD;
};

struct VtoP
{
    float4 position	: POSITION;
    float4 tex		: TEXCOORD0;
    float4 col      : COLOR;
};

VtoP vs_main(const VS_INPUT IN)
{
    VtoP OUT;
    OUT.position = mul(float4(IN.position.xyz, 1.0f), cmWorldViewProj);
    OUT.tex	= IN.texcoord;
    OUT.col = float4(cvDiffuseMin.x + cvPowers.x, cvDiffuseRange.x, cvClampAndScales.x, 1.0f);
    return OUT;
}

technique car
{
    pass p0
    {
        Texture[0] = DIFFUSEMAP_TEXTURE;
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Texture;

        VertexShader = compile vs_1_1 vs_main();
    }
}