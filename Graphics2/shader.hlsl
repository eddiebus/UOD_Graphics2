
cbuffer ConstantBuffer 
{
	matrix worldViewProj; 
};

struct VertexIn
{
	float3 InputPosition : POSITION;
    float4 Colour		 : COLOR;
};

struct VertexOut
{
	float4 OutputPosition : SV_POSITION;
    float4 Colour		  : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProj, float4(vin.InputPosition, 1.0f));
	
	// Just pass vertex color into the pixel shader.
    vout.Colour = vin.Colour;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Colour;
}


