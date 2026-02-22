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

technique car_normalmap
{
    pass p0
    {
        Texture[0] = DIFFUSEMAP_TEXTURE;
        ColorOp[0] = SelectArg1;
        ColorArg1[0] = Texture;
    }
}