require "system"
require "utility"

function onInit()
    CallbackManager:onInit()
end

function onTitleScreen()
    CallbackManager:onTitleScreen()
end

function onNewGame()
    CallbackManager:onNewGame()
end

function onLoadGame(id, data)
    CallbackManager:onLoadGame(id, data)
end

function onSaveGame(id)
    return CallbackManager:onSaveGame(id)
end

function onSceneDrawn(scene)
    CallbackManager:onSceneDrawn(scene)
end

function onDrawCharacter(character, isHero)
    return CallbackManager:onDrawCharacter(character, isHero)
end

function onCharacterDrawn(character, isHero)
    CallbackManager:onCharacterDrawn(character, isHero)
end

function onDrawBattler(battler, isMonster, id)
    return CallbackManager:onDrawBattler(battler, isMonster, id)
end

function onBattlerDrawn(battler, isMonster, id)
    CallbackManager:onBattlerDrawn(battler, isMonster, id)
end
