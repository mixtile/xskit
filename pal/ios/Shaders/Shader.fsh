//
//  Shader.fsh
//  palios
//
//  Created on 11-7-26.
//  Copyright 2011 XSKit.org. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
