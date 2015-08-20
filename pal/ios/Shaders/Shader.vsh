//
//  Shader.vsh
//  palios
//
//  Created on 11-7-26.
//  Copyright 2011 XSKit.org. All rights reserved.
//

attribute vec4 position;
attribute vec4 color;

varying vec4 colorVarying;

uniform float translate;

void main()
{
    gl_Position = position;
    gl_Position.y += sin(translate) / 2.0;

    colorVarying = color;
}
