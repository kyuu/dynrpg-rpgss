require "system.class"

Tween = class {
    __name = "Tween",
    
    __serialize = function(obj)
        local mfile = io.newMemoryFile()
        local writer = io.newWriter(mfile)

        -- write value
        writer:writeDouble(obj.value)

        -- write interpolation state
        writer:writeDouble(obj.t)
        writer:writeDouble(obj.b)
        writer:writeDouble(obj.c)
        writer:writeDouble(obj.d)

        -- write interpolation type
        writer:writeUint32(#obj.easing)
        writer:writeString(obj.easing)

        return mfile:copyBuffer()
    end,

    __deserialize = function(obj_data)
        local mfile = io.newMemoryFile(obj_data)
        local reader = io.newReader(mfile)

        -- create instance
        local obj = setmetatable({}, Tween)

        -- read value
        obj.value = reader:readDouble()

        -- read interpolation state
        obj.t = reader:readDouble()
        obj.b = reader:readDouble()
        obj.c = reader:readDouble()
        obj.d = reader:readDouble()

        -- read interpolation type
        obj.easing = reader:readString(reader:readUint32())

        -- check for EOF
        if mfile.eof then
            error("EOF")
        end

        return obj
    end,
    
    ease = {
        -- http://www.gizma.com/easing/
        -- https://gist.github.com/Metallix/628de265d0a24e0c4acb
        
        ["linear"] = function(t, b, c, d)
            return c*t/d + b
        end,
        
        ["quadratic in"] = function(t, b, c, d)
            t = t / d;
            return c*t*t + b
        end,

        ["quadratic out"] = function(t, b, c, d)
            t = t / d;
            return -c * t*(t-2) + b
        end,

        ["quadratic in/out"] = function(t, b, c, d)
            t = t / (d / 2)
            if t < 1 then
                return c/2*t*t + b
            else
                t = t - 1
                return -c/2 * (t*(t-2) - 1) + b
            end
        end,

        ["cubic in"] = function(t, b, c, d)
            t = t / d
            return c*t*t*t + b
        end,

        ["cubic out"] = function(t, b, c, d)
            t = (t / d) - 1
            return c*(t*t*t + 1) + b
        end,

        ["cubic in/out"] = function(t, b, c, d)
            t = t / (d / 2)
            if t < 1 then
                return c/2*t*t*t + b
            else
                t = t - 2
                return c/2*(t*t*t + 2) + b
            end
        end,

        ["sinusoidal in"] = function(t, b, c, d)
            return -c * math.cos(t/d * (math.pi/2)) + c + b
        end,

        ["sinusoidal out"] = function(t, b, c, d)
            return c * math.sin(t/d * (math.pi/2)) + b
        end,

        ["sinusoidal in/out"] = function(t, b, c, d)
            return -c/2 * (math.cos(math.pi*t/d) - 1) + b
        end,

        ["exponential in"] = function(t, b, c, d)
            return c * math.pow(2, 10 * (t/d - 1)) + b
        end,

        ["exponential out"] = function(t, b, c, d)
            return c * (-math.pow(2, -10 * t/d) + 1) + b
        end,

        ["exponential in/out"] = function(t, b, c, d)
            t = t / (d / 2)
            if t < 1 then
                return c/2 * math.pow(2, 10 * (t - 1)) + b
            else
                t = t - 1
                return c/2 * (-math.pow(2, -10 * t) + 2) + b
            end
        end,

        ["circular in"] = function(t, b, c, d)
            t = t / d
            return -c * (math.sqrt(1 - t*t) - 1) + b
        end,

        ["circular out"] = function(t, b, c, d)
            t = (t / d) - 1
            return c * math.sqrt(1 - t*t) + b
        end,

        ["circular in/out"] = function(t, b, c, d)
            t = t / (d / 2)
            if t < 1 then
                return -c/2 * (math.sqrt(1 - t * t) - 1) + b
            else
                t = t - 2
                return c/2 * (math.sqrt(1 - t * t) + 1) + b
            end
        end
    }
}

function Tween:__init(value)
    self.value = value
    
    -- interpolation state
    self.t = 0
    self.b = 0
    self.c = 0
    self.d = 0
    
    -- interpolation type
    self.easing = ""
end

function Tween:update(ms)
    if self.d > 0 then
        self.t = self.t + ms
        if self.t < self.d then
            -- interpolate
            self.value = Tween.ease[self.easing](self.t, self.b, self.c, self.d)
        else
            -- tween finished
            self:setValue(self.b + self.c)
            return true
        end
    end
    -- either no tween in progress or tween not finished yet
    return false
end

function Tween:getValue()
    return self.value
end

function Tween:setValue(value)
    self.value = value
    
    self.t = 0
    self.b = 0
    self.c = 0
    self.d = 0
    
    self.easing = ""
end

function Tween:tweenTo(value, duration, easing)
    if value ~= self.value and duration > 0 then
        self.t = 0
        self.b = self.value
        self.c = value - self.value
        self.d = duration
        
        self.easing = easing or "linear"
    else
        -- either we are already there or duration <= 0
        self:setValue(value)
    end
end
