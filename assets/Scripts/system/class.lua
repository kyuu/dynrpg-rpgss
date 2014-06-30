--[[
    Copyright (c) 2012 - 2014 Roland Yonaba

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    
    For more information visit: https://github.com/Yonaba/30log
--]]

local assert       = assert
local pairs        = pairs
local type         = type
local tostring     = tostring
local setmetatable = setmetatable

local baseMt     = {}
local _instances = setmetatable({},{__mode='k'})
local _classes   = setmetatable({},{__mode='k'})
--local class

local function deep_copy(t, dest, aType)
  local t = t or {}
  local r = dest or {}
  
  for k,v in pairs(t) do
    if aType and type(v)==aType then 
      r[k] = v 
    elseif not aType then
      if type(v) == 'table' and k ~= "__index" then 
        r[k] = deep_copy(v) 
      else
        r[k] = v
      end
    end
  end
  
  return r
end

local function instantiate(self,...)
  assert(_classes[self], 'new() should be called from a class.')
  local instance = deep_copy(self)
  _instances[instance] = tostring(instance)
  setmetatable(instance,self)
  
  if self.__init then
    if type(self.__init) == 'table' then
      deep_copy(self.__init, instance)
    else
      self.__init(instance, ...)
    end
  end
  
  return instance
end

local function extends(self,extra_params)
  local heir = {}
  _classes[heir] = tostring(heir)
  deep_copy(extra_params, deep_copy(self, heir))
  heir.__index = heir
  heir.super = self
  return setmetatable(heir,self)
end

baseMt = {
  __call = function (self,...)
    return self:new(...)
  end,
  
  __tostring = function(self,...)
    if _instances[self] then
      return 
        ('object(of %s):<%s>')
          :format((rawget(getmetatable(self),'__name') or '?'), _instances[self])
    end
    
    return
      _classes[self] and 
      ('class(%s):<%s>')
        :format((rawget(self,'__name') or '?'),_classes[self]) or 
      self      
  end
}

class = function(attr)
  local c = deep_copy(attr)
  _classes[c] = tostring(c)
  c.include = function(self,include)
    assert(_classes[self], 'Mixins can only be used on classes.')
    return deep_copy(include, self, 'function')
  end
  
  c.new = instantiate
  c.extends = extends
  c.__index = c
  c.__call = baseMt.__call
  c.__tostring = baseMt.__tostring
  
  c.is = function(self, kind)
    local super
    while true do 
      super = getmetatable(super or self)
      if super == kind or super == nil then break end
    end
    return kind and (super == kind)
  end
  return setmetatable(c, baseMt)
end

--return class
