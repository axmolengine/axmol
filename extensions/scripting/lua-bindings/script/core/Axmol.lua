
ax = ax or {}

-- Native vec2_new use array[2] to store x,y w,h u,v performance is better
-- p[1] alias: x w u width
-- p[2] alias: y h v height
local nvec2 = vec2_new
local nvec3 = vec3_new
local nvec4 = vec4_new

function ax.clampf(value, min_inclusive, max_inclusive)
    -- body
    local temp = 0
    if min_inclusive > max_inclusive then
        temp = min_inclusive
        min_inclusive =  max_inclusive
        max_inclusive = temp
    end

    if value < min_inclusive then
        return min_inclusive
    elseif value < max_inclusive then
        return value
    else
        return max_inclusive
    end
end

--Vec2/Point
function ax.vec2(_x, _y)
    if nil == _y then
        return nvec2(_x.x, _x.y)
    else
        return nvec2(_x, _y)
    end

end
function ax.p(_x, ...)
    return ax.vec2(_x, ...)
end

function ax.pAdd(pt1,pt2)
    return ax.p(pt1.x + pt2.x , pt1.y + pt2.y )
end

function ax.pSub(pt1,pt2)
    return ax.p(pt1.x - pt2.x , pt1.y - pt2.y )
end

function ax.pMul(pt1,factor)
    return ax.p( pt1.x * factor , pt1.y * factor )
end

function ax.pMidpoint(pt1,pt2)
    return ax.p( (pt1.x + pt2.x) / 2.0 , ( pt1.y + pt2.y) / 2.0 )
end

function ax.pForAngle(a)
    return ax.p( math.cos(a), math.sin(a) )
end

function ax.pGetLength(pt)
    return math.sqrt( pt.x * pt.x + pt.y * pt.y )
end

function ax.pNormalize(pt)
    local length = ax.pGetLength(pt)
    if 0 == length then
        return ax.p(  1.0, 0.0 )
    end

    return ax.p(  pt.x / length,  pt.y / length )
end

function ax.pCross(self,other)
    return self.x * other.y - self.y * other.x
end

function ax.pDot(self,other)
    return self.x * other.x + self.y * other.y
end

function ax.pToAngleSelf(self)
    return math.atan2(self.y, self.x)
end

function ax.pGetAngle(self,other)
    local a2 = ax.pNormalize(self)
    local b2 = ax.pNormalize(other)
    local angle = math.atan2(ax.pCross(a2, b2), ax.pDot(a2, b2) )
    if math.abs(angle) < 1.192092896e-7 then
        return 0.0
    end

    return angle
end

function ax.pGetDistance(startP,endP)
    return ax.pGetLength(ax.pSub(startP,endP))
end

function ax.pIsLineIntersect(A, B, C, D, s, t)
    if ((A.x == B.x) and (A.y == B.y)) or ((C.x == D.x) and (C.y == D.y))then
        return false, s, t
    end

    local BAx = B.x - A.x
    local BAy = B.y - A.y
    local DCx = D.x - C.x
    local DCy = D.y - C.y
    local ACx = A.x - C.x
    local ACy = A.y - C.y

    local denom = DCy * BAx - DCx * BAy
    s = DCx * ACy - DCy * ACx
    t = BAx * ACy - BAy * ACx

    if (denom == 0) then
        if (s == 0 or t == 0) then
            return true, s , t
        end

        return false, s, t
    end

    s = s / denom
    t = t / denom

    return true,s,t
end

function ax.pPerp(pt)
    return ax.p(  -pt.y,  pt.x )
end

function ax.RPerp(pt)
    return ax.p(  pt.y,   -pt.x )
end

function ax.pProject(pt1, pt2)
    return ax.p(  pt2.x * (ax.pDot(pt1,pt2) / ax.pDot(pt2,pt2)) ,  pt2.y * (ax.pDot(pt1,pt2) / ax.pDot(pt2,pt2)) )
end

function ax.pRotate(pt1, pt2)
    return ax.p(  pt1.x * pt2.x - pt1.y * pt2.y,  pt1.x * pt2.y + pt1.y * pt2.x )
end

function ax.pUnrotate(pt1, pt2)
    return ax.p(  pt1.x * pt2.x + pt1.y * pt2.y, pt1.y * pt2.x - pt1.x * pt2.y )
end
--Calculates the square length of pt
function ax.pLengthSQ(pt)
    return ax.pDot(pt,pt)
end
--Calculates the square distance between pt1 and pt2
function ax.pDistanceSQ(pt1,pt2)
    return ax.pLengthSQ(ax.pSub(pt1,pt2))
end

function ax.pGetClampPoint(pt1,pt2,pt3)
    return ax.p(  ax.clampf(pt1.x, pt2.x, pt3.x),  ax.clampf(pt1.y, pt2.y, pt3.y) )
end

function ax.pFromSize(sz)
    return ax.p( sz.width,  sz.height )
end

function ax.pLerp(pt1,pt2,alpha)
    return ax.pAdd(ax.pMul(pt1, 1.0 - alpha), ax.pMul(pt2,alpha) )
end

function ax.pFuzzyEqual(pt1,pt2,variance)
    if (pt1.x - variance <= pt2.x) and (pt2.x <= pt1.x + variance) and (pt1.y - variance <= pt2.y) and (pt2.y <= pt1.y + variance) then
        return true
    else
        return false
    end
end

function ax.pRotateByAngle(pt1, pt2, angle)
    return ax.pAdd(pt2, ax.pRotate( ax.pSub(pt1, pt2),ax.pForAngle(angle)))
end

function ax.pIsSegmentIntersect(pt1,pt2,pt3,pt4)
    local s,t,ret = 0,0,false
    ret,s,t =ax.pIsLineIntersect(pt1, pt2, pt3, pt4,s,t)

    if ret and  s >= 0.0 and s <= 1.0 and t >= 0.0 and t <= 1.0 then
        return true
    end

    return false
end

function ax.pGetIntersectPoint(pt1,pt2,pt3,pt4)
    local s,t, ret = 0,0,false
    ret,s,t = ax.pIsLineIntersect(pt1,pt2,pt3,pt4,s,t)
    if ret then
        return ax.p(pt1.x + s * (pt2.x - pt1.x), pt1.y + s * (pt2.y - pt1.y))
    else
        return ax.p(0,0)
    end
end
--Size
function ax.size( _width,_height )
    return ax.p(_width, _height)
end

--Rect
function ax.rect(_x,_y,_width,_height)
    return { x = _x, y = _y, width = _width, height = _height }
end

function ax.rectEqualToRect(rect1,rect2)
    if ((rect1.x >= rect2.x) or (rect1.y >= rect2.y) or
        ( rect1.x + rect1.width <= rect2.x + rect2.width) or
        ( rect1.y + rect1.height <= rect2.y + rect2.height)) then
        return false
    end

    return true
end

function ax.rectGetMaxX(rect)
    return rect.x + rect.width
end

function ax.rectGetMidX(rect)
    return rect.x + rect.width / 2.0
end

function ax.rectGetMinX(rect)
    return rect.x
end

function ax.rectGetMaxY(rect)
    return rect.y + rect.height
end

function ax.rectGetMidY(rect)
    return rect.y + rect.height / 2.0
end

function ax.rectGetMinY(rect)
    return rect.y
end

function ax.rectContainsPoint( rect, point )
    local ret = false

    if (point.x >= rect.x) and (point.x <= rect.x + rect.width) and
    (point.y >= rect.y) and (point.y <= rect.y + rect.height) then
        ret = true
    end

    return ret
end

function ax.rectIntersectsRect( rect1, rect2 )
    local intersect = not ( rect1.x > rect2.x + rect2.width or
        rect1.x + rect1.width < rect2.x         or
        rect1.y > rect2.y + rect2.height        or
        rect1.y + rect1.height < rect2.y )

    return intersect
end

function ax.rectUnion( rect1, rect2 )
    local rect = ax.rect(0, 0, 0, 0)
    rect.x = math.min(rect1.x, rect2.x)
    rect.y = math.min(rect1.y, rect2.y)
    rect.width = math.max(rect1.x + rect1.width, rect2.x + rect2.width) - rect.x
    rect.height = math.max(rect1.y + rect1.height, rect2.y + rect2.height) - rect.y
    return rect
end

function ax.rectIntersection( rect1, rect2 )
    local intersection = ax.rect(
        math.max(rect1.x, rect2.x),
        math.max(rect1.y, rect2.y),
        0, 0)

    intersection.width = math.min(rect1.x + rect1.width, rect2.x + rect2.width) - intersection.x
    intersection.height = math.min(rect1.y + rect1.height, rect2.y + rect2.height) - intersection.y
    return intersection
end

-- Color32 deprecated, use ax.color32 instead
function ax.color32( _r,_g,_b,_a )
    return { r = _r, g = _g, b = _b, a = _a }
end

-- ax::Color deprecated, use ax.color instead
function ax.c4f( _r,_g,_b,_a )
    return { r = _r, g = _g, b = _b, a = _a }
end

-- Color32
function ax.color32( _r,_g,_b,_a )
    return { r = _r, g = _g, b = _b, a = _a or 255 }
end

-- Color
function ax.color( _r,_g,_b,_a )
    return { r = _r, g = _g, b = _b, a = _a or 1.0 }
end

local function isFloatColor(c)
    return (c.r <= 1 and c.g <= 1 and c.b <= 1) and (math.ceil(c.r) ~= c.r or math.ceil(c.g) ~= c.g or math.ceil(c.b) ~= c.b)
end

function ax.convertColor(input, typ)
    assert(type(input) == "table" and input.r and input.g and input.b, "ax.convertColor() - invalid input color")
    local ret
    if typ == "3b" then
        if isFloatColor(input) then
            ret = {r = math.ceil(input.r * 255), g = math.ceil(input.g * 255), b = math.ceil(input.b * 255)}
        else
            ret = {r = input.r, g = input.g, b = input.b}
        end
    elseif typ == "4b" then
        if isFloatColor(input) then
            ret = {r = math.ceil(input.r * 255), g = math.ceil(input.g * 255), b = math.ceil(input.b * 255)}
        else
            ret = {r = input.r, g = input.g, b = input.b}
        end
        if input.a then
            if math.ceil(input.a) ~= input.a or input.a <= 1 then
                ret.a = input.a * 255
            else
                ret.a = input.a
            end
        else
            ret.a = 255
        end
    elseif typ == "4f" then
        if isFloatColor(input) then
            ret = {r = input.r, g = input.g, b = input.b}
        else
            ret = {r = input.r / 255, g = input.g / 255, b = input.b / 255}
        end
        if input.a then
            if math.ceil(input.a) ~= input.a or input.a <= 1 then
                ret.a = input.a
            else
                ret.a = input.a / 255
            end
        else
            ret.a = 255
        end
    else
        error(string.format("ax.convertColor() - invalid type %s", typ), 0)
    end
    return ret
end

--Vertex2F
function ax.vertex2F(_x,_y)
    return ax.vec2(_x, _y )
end

--Vertex3F
function ax.Vertex3F(_x,_y,_z)
    return ax.vec3(_x, _y, _z)
end

--Tex2F
function ax.tex2F(_u,_v)
    return ax.vec2( _u, _v )
end

--PointSprite
function ax.PointSprite(_pos,_color,_size)
    return { pos = _pos, color = _color, size = _size }
end

--Quad2
function ax.Quad2(_tl,_tr,_bl,_br)
    return { tl = _tl, tr = _tr, bl = _bl, br = _br }
end

--Quad3
function ax.Quad3(_tl, _tr, _bl, _br)
    return { tl = _tl, tr = _tr, bl = _bl, br = _br }
end

--V2F_T2F_C4F
function ax.V2F_T2F_C4F(_vertices, _colors, _texCoords)
    return { vertices = _vertices, colors = _colors, texCoords = _texCoords }
end

--V3F_T2F_C4F
function ax.V3F_T2F_C4F(_vertices, _colors, _texCoords)
    return { vertices = _vertices, colors = _colors, texCoords = _texCoords }
end

--V3F_T2F_C4F_Quad
function ax.V3F_T2F_C4F_Quad(_tl, _bl, _tr, _br)
    return { tl = _tl, bl = _bl, tr = _tr, br = _br }
end

--T2F_Quad
function ax.T2F_Quad(_bl, _br, _tl, _tr)
    return { bl = _bl, br = _br, tl = _tl, tr = _tr }
end

--AnimationFrameData
function ax.AnimationFrameData( _texCoords, _delay, _size)
    return { texCoords = _texCoords, delay = _delay, size = _size }
end

--PhysicsMaterial
function ax.PhysicsMaterial(_density, _restitution, _friction)
    return { density = _density, restitution = _restitution, friction = _friction }
end

function ax.vec3(_x, _y, _z)
    return nvec3( _x,  _y,  _z)
end

function ax.vec4(_x, _y, _z, _w)
    return nvec4(  _x,  _y,  _z, _w )
end

function ax.vec3add(vec3a, vec3b)
    return ax.vec3( vec3a.x + vec3b.x,  vec3a.y + vec3b.y,  vec3a.z + vec3b.z)
end

function ax.vec3sub(vec3a, vec3b)
    return ax.vec3( vec3a.x - vec3b.x,  vec3a.y - vec3b.y,  vec3a.z - vec3b.z)
end

function ax.vec3mul(vec3, factor)
    return ax.vec3( vec3.x * factor,  vec3.y * factor,  vec3.z * factor)
end

function ax.vec3dot(vec3a, vec3b)
    return vec3a.x * vec3b.x + vec3a.y * vec3b.y + vec3a.z * vec3b.z
end

function ax.vec3normalize(vec3)
    local n = ax.vec3dot(vec3, vec3)
    if n == 1.0 then
        return vec3
    end

    n = math.sqrt(n)

    if n < 2e-37 then
        return vec3
    end

    return ax.vec3mul(vec3, 1.0/n)
end

function ax.quaternion(_x, _y ,_z,_w)
    return ax.vec4(_x, _y, _z, _w)
end

function ax.quaternion_createFromAxisAngle(axis, angle)

    local  halfAngle = angle * 0.5
    local  sinHalfAngle = math.sin(halfAngle)

    local normal = ax.vec3(axis.x, axis.y, axis.z)
    normal = ax.vec3normalize(normal)
    local dst = ax.vec3(0.0, 0.0, 0.0)
    dst.x = normal.x * sinHalfAngle
    dst.y = normal.y * sinHalfAngle
    dst.z = normal.z * sinHalfAngle
    dst.w = math.cos(halfAngle)

    return dst
end

function ax.blendFunc(_src, _dst)
    return {src = _src, dst = _dst}
end

ax.mat4 = ax.mat4 or {}

function ax.mat4.new(...)
    local params = {...}
    local size   = #params
    local obj = {}

    if 1 == size then
        assert(type(params[1]) == "table" , "type of input params are wrong to new a mat4 when num of params is 1")
        for i= 1, 16 do
            if params[1][i] ~= nil then
                obj[i] = params[1][i]
            else
                obj[i] = 0
            end
        end
    elseif 16 == size then
        for i= 1, 16 do
            obj[i] = params[i]
        end
    end

    setmetatable(obj, {__index = ax.mat4})

    return obj
end

function ax.mat4.getInversed(self)
    return mat4_getInversed(self)
end

function ax.mat4.transformVector(...)
    return mat4_transformVector(...)
end

function ax.mat4.multiply(self, mat)
    return mat4_multiply(self, mat)
end

function ax.mat4.decompose(self, scale, rotation, translation)
    return mat4_decompose(self, scale ,rotation, translation)
end

function ax.mat4.createIdentity()
    return ax.mat4.new(1.0 ,0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0)
end

function ax.mat4.translate(self,vec3)
    return mat4_translate(self,vec3)
end

function ax.mat4.createRotationZ(self,angle)
    return mat4_createRotationZ(self,angle)
end

function ax.mat4.setIdentity(self)
    return mat4_setIdentity(self)
end

function ax.mat4.createTranslation(...)
    return mat4_createTranslation(...)
end

function ax.mat4.createRotation(...)
    return mat4_createRotation(...)
end
