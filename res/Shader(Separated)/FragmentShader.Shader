#version 330 core

uniform vec2 u_resolution;
uniform float u_time;

layout(location = 0) out vec4 color;

//extra info  for cook-Torrance Shading 

uniform float F0;
uniform float roughness; 
uniform float k;


//materials (object)
uniform vec3 Mdiffuse;
uniform vec3 Mspecular;
uniform vec3 Mambient;
uniform float Mshininess;

//materials (Light)
uniform vec3 Lambient;
uniform vec3 Ldiffuse;
uniform vec3 Lspecular;

uniform vec4 u_color;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

//Modeling option
uniform int ModelChoice;

//TextureActivation
uniform int TextureState;

in vec3 vertexColor;
in vec2 TexCoord;
in vec3 Normal;

in vec3 FragPos;

uniform sampler2D ourTexture;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


void main ()
{
	
	//phong model 
	if (ModelChoice == 1)
	{
		//Ambient
		vec3 ambient = Lambient * Mambient;

		//Diffuse
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse =   diff * Mdiffuse ;

		//Specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), Mshininess);
		vec3 specular =  spec * Mspecular ;

		//Result
		vec3 result = (ambient + diffuse + specular) ;
		
		if (TextureState == 0)
		{

			color = vec4(result, 1.0);
		}
			
				
		else 
				color = texture(texture_diffuse1, TexCoord) * vec4(result, 1.0);
				
	}
	else
	{
		//blinn phong model
		if (ModelChoice == 2)
		{
			//Ambient
			vec3 ambient = Lambient * Mambient;

			//Diffuse
			vec3 norm = normalize(Normal);
			vec3 lightDir = normalize(lightPos - FragPos);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = Ldiffuse * (diff * Mdiffuse);

			//Specular
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);

			vec3 halfwayDir = normalize(lightDir + viewDir);

			float spec = pow(max(dot(norm, halfwayDir), 0.0), Mshininess);
			vec3 specular = Lspecular * (spec * Mspecular);


			//Result
			vec3 result = (ambient + diffuse + specular);

			if (TextureState == 0)
				color = vec4(result, 1.0);
			else
				color = texture(ourTexture, TexCoord) * vec4(result, 1.0);

		}
		else
		{
			//cook torrance model
			if (ModelChoice == 3)
			{
				
			
				vec3 norm = normalize(Normal);
				vec3 lightDir = normalize(lightPos - FragPos);
				vec3 viewDir = normalize(viewPos - FragPos);

				//Diffuse
				float diff = max(dot(norm, lightDir), 0.0);
				vec3 diffuse = Ldiffuse * (diff * Mdiffuse);


				//Ambient
				vec3 ambient = Lambient * Mambient * 0.5;

				//specular 
				float NdotL = max(0, dot(norm, lightDir));
				float Rs = 0.0;
				
				if (NdotL > 0)
				{

					vec3 H = normalize(lightDir + viewDir);
					float NdotH = max(0, dot(norm, H));
					float NdotV = max(0, dot(norm, viewDir));
					float VdotH = max(0, dot(viewDir, H));
					float HdotL = max(0, dot(lightDir, H));

					// Fresnel reflectance

					float F = pow(1.0 - VdotH, 5.0);
					F *= (1.0 - F0);
					F += F0;

					//float F = F0 + (1 - F0) * pow(1 - NdotV, 5.0);
					
					// Microfacet distribution by Beckmann
					float m_squared = roughness * roughness;
					float r1 = 1.0 / (m_squared * pow(NdotH, 4.0));
					float r2 =  - ( 1.0 - NdotH * NdotH ) * (1.0 - NdotH * NdotH) / (m_squared * NdotH * NdotH);
					float D = r1 * exp(r2);

					// Geometric shadowing
					float two_NdotH = 2.0 * NdotH;
					float g1 = (two_NdotH * NdotV) / VdotH;
					float g2 = (two_NdotH * NdotL) / HdotL;
					float G = min(1.0, min(g1, g2));

					Rs = (F * D * G) / (3.14159265359 * NdotL * NdotV);
				}

				vec3 result = Mdiffuse * Lambient * NdotL + Lambient * Mspecular * NdotL * (k + Rs * (1.0 - k)) ;

				if (TextureState == 0)
					color = vec4(result, 1.0);
				else
					color = texture(ourTexture, TexCoord) * vec4(result, 1.0);
			}
			else
			{
				//Lambert
				if (ModelChoice == 4)
				{

					//Ambient
					vec3 ambient = Lambient * Mambient;

					//Diffuse
					vec3 norm = normalize(Normal);
					vec3 lightDir = normalize(lightPos - FragPos);
					float diff = max(dot(norm, lightDir), 0.0);
					vec3 diffuse = Ldiffuse * (diff * Mdiffuse);


					//Result
					vec3 result = (ambient + diffuse);

					if (TextureState == 0)
						color = vec4(result, 1.0);
					else
						color = texture(ourTexture, TexCoord) * vec4(result, 1.0);

				}
				else {

					// toon shader
					if (ModelChoice == 5)
					{

						vec3 norm = normalize(Normal);
						vec3 lightDir = normalize(lightPos - FragPos);
						float intensity;
						vec4 ToonColor ;

						const int levels = 3;
						const float scaleFactor = 1.0 / levels;

						float cosine = max(0.0, dot(lightDir, Normal));
						vec3 diffuse = Mdiffuse * floor(cosine * levels) * scaleFactor;

						ToonColor = vec4 ((Lambient + Ldiffuse + Lspecular) * (Mambient + diffuse),1.0);

						if (TextureState == 0)
							color = ToonColor;
						else
							color = texture(ourTexture, TexCoord) * ToonColor ;
						
					
					}
				
				}
				

			
			}
		}
	}
	


			
	
			
	
	

	
};