require "std"
require "esi"


function onTitleScreen()
    World:reset()
end

function onLoadGame(id, data)
    World:load(data)
end

function onSaveGame(id)
    return World:save()
end

function onSceneDrawn(scene)
    World:update(scene)
    World:render(scene)
end
