#version 130

uniform sampler2D texture;
uniform int u_mode; // 0 = Protanopia, 1 = Deuteranopia, 2 = Tritanopia

// Matrices de transformation pour différents types de daltonisme
const mat3 protanopiaMatrix = mat3(
    0.567, 0.433, 0.0,
    0.558, 0.442, 0.0,
    0.0, 0.242, 0.758
);

const mat3 deuteranopiaMatrix = mat3(
    0.625, 0.375, 0.0,
    0.7, 0.3, 0.0,
    0.0, 0.3, 0.7
);

const mat3 tritanopiaMatrix = mat3(
    0.95, 0.05, 0.0,
    0.0, 0.433, 0.567,
    0.0, 0.475, 0.525
);

void main()
{
    // Récupérer la couleur du pixel
    vec4 pixelColor = texture2D(texture, gl_TexCoord[0].xy);
    
    // Convertir en matrice de transformation selon le mode
    mat3 conversionMatrix;
    if (u_mode == 0)
        conversionMatrix = protanopiaMatrix;
    else if (u_mode == 1)
        conversionMatrix = deuteranopiaMatrix;
    else
        conversionMatrix = tritanopiaMatrix;
    
    // Appliquer la transformation
    vec3 transformedColor = conversionMatrix * pixelColor.rgb;
    
    // Sortie finale
    gl_FragColor = vec4(transformedColor, pixelColor.a);
}