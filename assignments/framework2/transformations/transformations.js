/*
 * Student names: Derk Barten
 * Student numbers: 11043075
 *
 */

 // Translate by x, y and z.
function myTranslate(x, y,  z) {
    var mat = [
               1.0, 0.0, 0.0, 0.0,
               0.0, 1.0, 0.0, 0.0,
               0.0, 0.0, 1.0, 0.0,
               x, y, z, 1.0
              ];

   return mat;
}

// Scale by x, y and z.
function myScale(x, y, z) {
    var mat = [
               x, 0.0, 0.0, 0.0,
               0.0, y, 0.0, 0.0,
               0.0, 0.0, z, 0.0,
               0.0, 0.0, 0.0, 1.0
              ];

   return mat;
}

// Compute the argmin of a list of absolute values
function abs_argmin(list) {
    var min = -1;
    var argmin = -1;
    for (var i = 0; i < list.length; i++) {
        if (list[i] < min || min == -1) {
            min = list[i];
            argmin = i; 
        }
    }
    return argmin;
}

// Compute the cross product for 3D vectors
function cross_product3D(v, w) {
    return [ v[1] * w[2] - v[2] * w[1],
             v[2] * w[0] - v[0] * w[2],
             v[0] * w[1] - v[1] * w[0] ];
}

// Compute the vector length for 3D vectors
function vector_length3D(v) {
    return Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2) + Math.pow(v[2], 2));
}

// Divide any vector be a scalar
function vector_divide(v, value) {
    for (var i = 0; i < v.length; i++) {
        v[i] = v[i] / value;
    }
    return v;
}

// Compute the dotproduct of 3D vectors
function vector_dotproduct3D(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// Rotate by angle around [x, y, z]^T.
function myRotate(angle, x, y, z) {
    // Create the orthonormal basis

    var a = [x, y, z];
    // Normalize a to find the first orthonormal vector w
    w = vector_divide(a, vector_length3D(a));
    
    // Find the smalles element of w
    argmin_w = abs_argmin([Math.abs(w[0]), Math.abs(w[1]), Math.abs(w[2])]);
    if (argmin_w < 0 || argmin_w >= w.length) {
        console.Error("Error: argmin out of bouds");
        return -1;
    }

    // Replace the smalles element of w with a 1
    var t = w.slice(); 
    t[argmin_w] = 1;

    // Compute the second ortonormal vector u on plane t and w  
    var u = vector_divide(cross_product3D(t, w), vector_length3D(cross_product3D(t, w)));
    // Create the last orthonormal vector by taking the crossproduct of w and u
    var v = cross_product3D(w, u);

    // Debugging statements
    console.debug("Value of w: " + w);
    console.debug("Argmin of w: " + argmin_w);
    console.debug("t: " + t);
    console.debug("u: " + u);
    console.debug("w dot u: " + vector_dotproduct3D(w, u));
    console.debug("v: " + v);
    console.debug("v dot u: " + vector_dotproduct3D(v, u));
    
    // Set up the three matrices making up the rotation
    var A = [
        u[0], u[1], u[2], 0.0,
        v[0], v[1], v[2], 0.0,
        a[0], a[1], a[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    var B = [
             Math.cos(angle), Math.sin(angle), 0.0, 0.0,
             -Math.sin(angle), Math.cos(angle), 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0
            ];

    var C = [    
        u[0], v[0], a[0], 0.0,
        u[1], v[1], a[1], 0.0,
        u[2], v[2], a[2], 0.0,
        0.0, 0.0, 0.0, 1.0
    ];

    var mat = m4.multiply(A, m4.multiply(B, C));
    return mat;
}
