
Tween = {
    __classname = "Tween",

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
Tween.__index = Tween

function Tween.new(value)
    local self = setmetatable({}, Tween)
    
    self._value = value
    self._working = false
    
    return self
end

function Tween.update(self, ms)
    if self._working then
        self._t = self._t + ms
        if self._t < self._d then
            self._value = self._ease(self._t, self._b, self._c, self._d)
        else
            self._value = self._b + self._c
            self._working = false
            -- we just finished working
            return true
        end
    end
    -- either not working or we are not finished yet
    return false
end

function Tween.get(self)
    return self._value
end

function Tween.set(self, value)
    self._value = value
    -- stop any ongoing work
    self._working = false
end

function Tween.tween_to(self, value, duration, easing)
    if value ~= self._value and duration > 0 then
        self._t = 0
        self._b = self._value
        self._c = value - self._value
        self._d = duration
        self._ease = easing and Tween.ease[easing] or Tween.ease["linear"]
        self._working = true
    else
        -- either we are already there or duration is <= 0
        self._value = value
        self._working = false
    end
end
