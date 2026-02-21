texture         DIFFUSEMAP_TEXTURE;

sampler	        DIFFUSE_SAMPLER = sampler_state
{
    Texture = <DIFFUSEMAP_TEXTURE>;
    AddressU = CLAMP;
    AddressV = CLAMP;
    MIPFILTER =	LINEAR;
    MINFILTER =	LINEAR;
    MAGFILTER =	LINEAR;
};

float4x4        WorldViewProj;

shared float4	cvDiffuseMin        : register(c21);
shared float4	cvDiffuseRange      : register(c22);
shared float4	cvEnvmapMin         : register(c23);
shared float4	cvEnvmapRange       : register(c24);
shared float4   cvPowers            : register(c25);
shared float4   cvClampAndScales    : register(c26);
shared float4   cvVinylScales       : register(c27);

struct VS_INPUT
{
    float4 position	: POSITION;
    float2 texcoord	: TEXCOORD;
};

struct VtoP
{
    float4 position	: POSITION;
    float2 tex		: TEXCOORD0;
    float4 col      : COLOR0;
};

VtoP vs_main(const VS_INPUT IN)
{
    VtoP OUT;
    OUT.position = mul(float4(IN.position.xyz, 1.0f), WorldViewProj);

    OUT.tex	= IN.texcoord;
    OUT.col = float4(cvDiffuseRange.x, cvDiffuseRange.y, cvDiffuseRange.z, 1.0f 
        + ((cvDiffuseMin.x + cvPowers.x + cvVinylScales.x + cvDiffuseRange.x + cvEnvmapMin.x + cvEnvmapRange.x + cvClampAndScales.x) * 0.0001f)); // here so we "use" the constants

    return OUT;
}

technique car
{
    pass p0
    {
        Texture[0] = DIFFUSEMAP_TEXTURE;
        ColorOp[0] = Add;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
        ColorOp[1] = Disable;

        VertexShader = compile vs_1_1 vs_main();
    }
}