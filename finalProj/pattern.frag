#version 330 compatibility


	// coefficients of each type of lighting


in  vec3  vN;			// normal vector
in  vec4  ECposition;			// vector from point to light
in  vec3  vE;			// vector from point to eye

#define NUM_LIGHTS 2

void
main( )
{
	vec3 Normal = normalize(vN);
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < NUM_LIGHTS; i++){
		vec3 LightPosition = gl_LightSource[i].position.xyz;
		vec3 vL = LightPosition - ECposition.xyz;		// vector from the point

	
		vec3 Light     = normalize(vL);
		vec3 Eye        = normalize(vE);
	


		vec4 ambient = gl_FrontLightProduct[i].ambient;

		float d = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
		vec4 diffuse = gl_FrontLightProduct[i].diffuse * d;


		float s = 0.;
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		s = pow( max( dot(Eye,ref),0. ), gl_FrontMaterial.shininess);
		vec4 specular = gl_FrontLightProduct[i].specular * s;
		finalColor += ambient + diffuse + specular;
	}
	gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor;

}