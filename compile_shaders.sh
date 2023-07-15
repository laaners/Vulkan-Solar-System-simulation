for file in shaders/*.frag;
do
    echo $file
    out="${file/Shader/Frag}"
    out="${out/.frag/.spv}"
    glslc "$file" -o "$out"
done

for file in shaders/*.vert;
do
    echo $file
    out="${file/Shader/Vert}"
    out="${out/.vert/.spv}"
    glslc "$file" -o "$out"
done

#glslc PhongShader.vert -o PhongVert.spv
