
#define PI 3.14159265358979323846

float LinearizeDepth(float depth, float zNear, float zFar)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));    
}