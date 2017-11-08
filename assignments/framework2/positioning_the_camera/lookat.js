/*
 * Student Names: Derk Barten
 * Student Numbers: 11043075
 *
 */
function vector_norm3D(v){
    var len = vector_length3D(v);
    v[0] = v[0] / len;
    v[1] = v[1] / len;
    v[2] = v[2] / len;
    return v;
}

function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2));
}

function cross_product3D(v, w) {
    return [ v[1] * w[2] - v[2] * w[1],
             v[2] * w[0] - v[0] * w[2],
             v[0] * w[1] - v[1] * w[0] ];
}

function vector_dotproduct3D(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2));
}

function myLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ) {

    // Translate everything relative to the camera
    var translate = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -eyeX, -eyeY, -eyeZ, 1.0
    ];

    var cz =vector_norm3D([centerX - eyeX, centerY - eyeY, centerZ - eyeZ]);
    var cx = vector_norm3D(cross_product3D([upX, upY, upZ],cz));
    var cy = cross_product3D(cx, cz);

    // Checks to see if the basis is orthagonal
    console.log("cx: " + cx);
    console.log("cy: " + cy);
    console.log("cz: " + cz);

    console.log("cx length: " + vector_length3D(cx));
    console.log("cy length: " + vector_length3D(cy));
    console.log("cz length: " + vector_length3D(cz));

    console.log("cx dot cy " + vector_dotproduct3D(cx, cy));
    console.log("cx dot cz " + vector_dotproduct3D(cx, cz));
    console.log("cy dot cz " + vector_dotproduct3D(cy, cz));
    
  
    // TODO: Question, why all inverted?
    // Is R in the assignment pdf correct?
    var R_inv = [
        -cx[0], -cy[0], -cz[0], 0.0,
        -cx[1], -cy[1], -cz[1], 0.0,
        -cx[2], -cy[2], -cz[2], 0.0,
        0.0, 0.0, 0.0, 1
    ];
    
    // We can simply transpose R since it is ortagonal
    /*
    var R_inv = [
        cx[0], cx[1], cx[2], 0.0,
        cy[0], cy[1], cy[2], 0.0,
        -cz[0], -cz[1], -cz[2], 0.0,
        0.0, 0.0, 0.0, 1
    ];
    */
    mat = m4.identity();
    m4.multiply(R_inv, translate, mat);

    return mat;
}
