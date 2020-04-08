#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;



layout(std140, binding=2) uniform primitives{
    vec3 primitiveCoordinates[3430];
};




void firstIntersect(){

}




void intersect(){

}



void trace(Ray ray){
    for (int i=0;i<primitiveCoordinates.length();i=i+3){
        if(intersect(ray, primitiveCoordinates[i],primitiveCoordinates[i+1],primitiveCoordinates[i+2])){

        }
    }
}



void main()
{

    FragColor = vec4(primitiveCoordinates[2].x-575.5f, primitiveCoordinates[2].y-56.0f, primitiveCoordinates[2].z-704.0f, 1.0f);
}