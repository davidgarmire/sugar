use("xformstack.lua")

-- First check out the basics

rx45 = xform_ox(45)
ry45 = xform_oy(45)
rz45 = xform_oz(45)
trans = xform_translate {1, 2, 3}

x1 = xform_apply(rx45,  {1, 0, 0})
x2 = xform_apply(ry45,  {1, 0, 0})
x3 = xform_apply(rz45,  {1, 0, 0})
x4 = xform_apply(trans, {1, 0, 0})
x5 = xform_applyA(trans, {1, 0, 0})

print("Rotate e1 by rx45: ",     x1[1], x1[2], x1[3])
print("Rotate e1 by ry45: ",     x2[1], x2[2], x2[3])
print("Rotate e1 by rz45: ",     x3[1], x3[2], x3[3])
print("Translate e1 by 1,2,3: ", x4[1], x4[2], x4[3])
print("Apply A e1 by 1,2,3: ",   x5[1], x5[2], x5[3])

-- Now check out composition

T = xform_compose(trans, rz45)
x = xform_apply(T, {1, 0, 0})

print("Rotate then translate: ", x[1], x[2], x[3])

undoT = xform_compose(xform_oz(-45), xform_translate{-1,-2,-3})
xform_apply(undoT, x)

print("After undo operation: ", x[1], x[2], x[3])

-- Now check out subnetization operation

subnetized1 = 
subnet(
  function(p)
    top_xform(p)
    print("In subnetized1: ", p[1], p[2], p[3])
  end
)

subnetized2 = 
subnet(
  function(p)
    local p2 = {p[1], p[2], p[3]; oz=90};
    subnetized1(p2)
    top_xform(p)
    print("In subnetized2: ", p[1], p[2], p[3])
  end
)

subnetized2 {1, 0, 0; oz=45}

