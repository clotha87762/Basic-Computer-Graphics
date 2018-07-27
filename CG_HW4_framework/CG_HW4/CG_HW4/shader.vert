attribute vec2 av2texCoord;
attribute vec4 av4position;
attribute vec3 av3normal;
attribute vec4 normal4;

uniform mat4 mvp;
uniform mat4 um4modelMatrix;

varying vec2 v2texCoord;
varying vec4 vv4vertice,vv4colour;

varying vec4 v4position;
varying vec3 v3normal;
varying vec4 v4normal; 

struct LightSourceParameters {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
	vec4 halfVector;
	vec3 spotDirection;
	float spotExponent;
	float spotCutoff; // (range: [0.0,90.0], 180.0)
	float spotCosCutoff; // (range: [1.0,0.0],-1.0)
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct MaterialParameters {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalLight;
uniform mat4 nmvp;
uniform MaterialParameters Material;
uniform LightSourceParameters LightSource;
uniform vec4 eye;

varying vec4 vv4color;
varying vec4 vv4ambient, vv4diffuse;
varying vec3 vv3normal, vv3halfVector;

uniform int type;  // 1= directional  2=point 3=spot

uniform int lightSwitch;

uniform int shader;



void main() {
	vec4 color =vec4(0,0,0,0);
	
	
	v4normal = normal4;
	v4position = av4position;
	v3normal = av3normal;
	
	
	const float constantAttenuation = 0.4;
	const float linearAttenuation = 0.4;
	const float quadraticAttenuation = 0.4;
	globalLight = vec4(0.2f,0.2f,0.2f,1.0f)*Material.ambient;
	
	vec4 v = mvp*av4position;
 	vec3 normal;
 	vec3 lightDir;
 	vec4 vv4diffuse;
 	vec4 vv4specular;
 	vec3 E = normalize(eye-v.xyz); 
    vec3 R;
    float dist;
    vec4 spotDir;
    float att;
    float spotLightEffect;

    dist = sqrt(dot(LightSource.position-v,LightSource.position-v));
    att = 1.0 / (constantAttenuation + linearAttenuation * dist + quadraticAttenuation * dist * dist);

 	normal = normalize(nmvp*normal4).xyz;


	if(type==1){
	lightDir = normalize(LightSource.position.xyz);
	}
	else if(type==2||type==3){
	lightDir = normalize(LightSource.position.xyz-v.xyz);
	}


	R = normalize(reflect(lightDir,normal));  
	
	float NdotL = max(dot(normal,lightDir),0.0);

	vv4specular = LightSource.specular*pow(max(dot(R,E),0.0),
	20.0)*Material.specular;
	
	vv4diffuse = LightSource.diffuse * Material.diffuse;
	vv4diffuse=vv4diffuse* NdotL;
	vv4diffuse =clamp(vv4diffuse, 0.0, 1.0); 
	vv4ambient = LightSource.ambient*Material.ambient;

	spotDir = vec4(0.0,0.0,-1.0,0.0);
	spotDir = normalize(spotDir);
 
    if(type==3){
	
	
    	spotLightEffect = dot(spotDir,normalize(LightSource.position.xyz-v.xyz));
    	if (spotLightEffect>LightSource.spotCosCutoff) {
        	
 			spotLightEffect = pow(spotLightEffect,LightSource.spotExponent);
    	}
    	else{
    		spotLightEffect = 0.0;
    	}
		
 	}

	if(lightSwitch==0){
		color = globalLight;
	}
	else if(type==3){
	color= (vv4diffuse + vv4ambient +vv4specular) ;
	color=color*spotLightEffect;
	color=color+globalLight;
	}
	else if(type==1||type==2){
	color= (vv4diffuse + vv4ambient +vv4specular) ;
	color = color +globalLight;
	}
	
	
	
	
	vv4colour = color;
	

	gl_Position = mvp * av4position;
	
	v2texCoord = av2texCoord;
	vv4vertice = um4modelMatrix * av4position;
	
	

}
