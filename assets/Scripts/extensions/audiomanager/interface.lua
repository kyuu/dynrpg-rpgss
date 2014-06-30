-------------------------------------------------------------------------------
-- Plays a background music indefinitely.
--
-- @param filename      (String) Filename of the background music to play. If
--                               empty (""), the currently playing BGM will be
--                               stopped and cleared.
-- @param volume        (Number) Optional volume of the BGM. Possible values
--                               range from 0 (silence) to 100 (maximum volume).
--                               Defaults to 100.
-- @param pitch         (Number) Optional pitch of the BGM. Possible values
--                               range from 50 to 200. Defaults to 100.
-------------------------------------------------------------------------------
function play_bgm(filename, volume, pitch)
    assert(type(filename) == "string", "invalid filename")
    volume = volume or 100
    pitch = pitch or 100
    AudioManager:playBgm(filename, volume / 100, pitch / 100)
end

-------------------------------------------------------------------------------
-- Stops the currently playing background music. The background music can be
-- resumed later on.
-------------------------------------------------------------------------------
function stop_bgm()
    AudioManager:stopBgm()
end

-------------------------------------------------------------------------------
-- Resumes the current background music.
-------------------------------------------------------------------------------
function resume_bgm()
    AudioManager:resumeBgm()
end

-------------------------------------------------------------------------------
-- Fades out the currently playing background music.
--
-- @param time          (Number) The fade-out time in milliseconds.
-------------------------------------------------------------------------------
function fade_bgm(time)
    AudioManager:fadeBgm(time)
end

-------------------------------------------------------------------------------
-- Memorizes the current background music, so it can be restored later on.
-------------------------------------------------------------------------------
function memorize_bgm()
    AudioManager:memorizeBgm()
end

-------------------------------------------------------------------------------
-- Restores (plays) a previously memorized background music (if any).
-------------------------------------------------------------------------------
function restore_bgm()
    AudioManager:restoreBgm()
end

-------------------------------------------------------------------------------
-- Plays a background sound indefinitely.
--
-- @param filename      (String) Filename of the background sound to play. If
--                               empty (""), the currently playing BGS will be
--                               stopped and cleared.
-- @param volume        (Number) Optional volume of the BGS. Possible values
--                               range from 0 (silence) to 100 (maximum volume).
--                               Defaults to 100.
-- @param pitch         (Number) Optional pitch of the BGS. Possible values
--                               range from 50 to 200. Defaults to 100.
-------------------------------------------------------------------------------
function play_bgs(filename, volume, pitch)
    assert(type(filename) == "string", "invalid filename")
    volume = volume or 100
    pitch = pitch or 100
    AudioManager:playBgs(filename, volume / 100, pitch / 100)
end

-------------------------------------------------------------------------------
-- Stops the currently playing background sound. The background sound can be
-- resumed later on.
-------------------------------------------------------------------------------
function stop_bgs()
    AudioManager:stopBgs()
end

-------------------------------------------------------------------------------
-- Resumes the current background sound.
-------------------------------------------------------------------------------
function resume_bgs()
    AudioManager:resumeBgs()
end

-------------------------------------------------------------------------------
-- Fades out the currently playing background sound.
--
-- @param time          (Number) The fade-out time in milliseconds.
-------------------------------------------------------------------------------
function fade_bgs(time)
    AudioManager:fadeBgs(time)
end

-------------------------------------------------------------------------------
-- Memorizes the current background sound, so it can be restored later on.
-------------------------------------------------------------------------------
function memorize_bgs()
    AudioManager:memorizeBgs()
end

-------------------------------------------------------------------------------
-- Restores (plays) a previously memorized background sound (if any).
-------------------------------------------------------------------------------
function restore_bgs()
    AudioManager:restoreBgs()
end

-------------------------------------------------------------------------------
-- Plays a music effect. The currently playing background music will be stopped
-- temporarily and automatically resumed once the music effect stopped.
--
-- @param filename      (String) Filename of the music effect to play. If
--                               empty (""), the currently playing ME will be
--                               stopped and cleared.
-- @param volume        (Number) Optional volume of the ME. Possible values
--                               range from 0 (silence) to 100 (maximum volume).
--                               Defaults to 100.
-- @param pitch         (Number) Optional pitch of the ME. Possible values
--                               range from 50 to 200. Defaults to 100.
-------------------------------------------------------------------------------
function play_me(filename, volume, pitch)
    assert(type(filename) == "string", "invalid filename")
    volume = volume or 100
    pitch = pitch or 100
    AudioManager:playMe(filename, volume / 100, pitch / 100)
end

-------------------------------------------------------------------------------
-- Stops the currently playing music effect and resumes the current background
-- music (if any).
-------------------------------------------------------------------------------
function stop_me()
    AudioManager:stopMe()
end

-------------------------------------------------------------------------------
-- Fades out the currently playing music effect and resumes the current
-- background music (if any), once the fade-out completed.
--
-- @param time          (Number) The fade-out time in milliseconds.
-------------------------------------------------------------------------------
function fade_me(time)
    AudioManager:fadeMe(time)
end

-------------------------------------------------------------------------------
-- Plays a sound effect.
--
-- @param filename      (String) Filename of the sound effect to play.
-------------------------------------------------------------------------------
function play_se(filename)
    assert(type(filename) == "string" and #filename > 0, "invalid filename")
    AudioManager:playSe(filename)
end

-------------------------------------------------------------------------------
-- Stops all currently playing sound effects.
-------------------------------------------------------------------------------
function stop_se()
    AudioManager:stopSe()
end
