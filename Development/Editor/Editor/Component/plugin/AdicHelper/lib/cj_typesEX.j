/*
    cj_typesEX.j

    Extended replacement of some constructs, dealing with game object creation.
    Private version, recommended for usage in imported libraries etc

    This file is part of cJass standard library
    © 2009 Van Damm \\ cjass.xgm.ru
*/

#guard cj_typesEX

define {
    <new hashtable> = InitHashtable()
    <new timer> = CreateTimer()
    <new group> = CreateGroup()
    <new force> = CreateForce()
    <new region> = CreateRegion()
    <new trigger> = CreateTrigger()
    <new dialog> = DialogCreate()
    <new leaderboard> = CreateLeaderboard()
    <new multiboard> = CreateMultiboard()
    <new quest> = CreateQuest()
    <new texttag> = CreateTextTag()
    <new camerasetup> = CreateCameraSetup()
    <new timerdialog>(t) = CreateTimerDialog(t)
    <new gamecache>(file) = InitGameCache(file)
    <new weathereffect>(where,effectID) = AddWeatherEffect(where,effectID)

    <new player>() = GetLocalPlayer()
    <new player>(id) = Player(id)

    <new location>() = Location(0,0)
    <new location>(x,y) = Location(x,y)

    <new rect>() = Rect(0,0,0,0)
    <new rect>(x1,y1,x2,y2) = Rect(x1,y1,x2,y2)

    <new item>(itemid) = CreateItem(itemid,0,0)
    <new item>(itemid,x,y) = CreateItem(itemid,x,y)

    <new unit>(p,id) = CreateUnit(p,id,0,0,0)
    <new unit>(p,id,x,y) = CreateUnit(p,id,x,y,0)
    <new unit>(p,id,x,y,f) = CreateUnit(p,id,x,y,f)

    <new effect>(modelName) = AddSpecialEffect(modelName,0,0)
    <new effect>(modelName,x,y) = AddSpecialEffect(modelName,x,y)

    <new destructable>(objectid,x,y) = CreateDestructable(objectid,x,y,1,0)
    <new destructable>(objectid,x,y,z) = CreateDestructableZ(objectid,x,y,z,1,0)
    <new destructable>(objectid,x,y,facescale,variation) = CreateDestructable(objectid,x,y,facescale,variation)
    <new destructable>(objectid,x,y,z,facescale,variation) = CreateDestructableZ(objectid,x,y,z,facescale,variation)

    <new trackable>(trackableModelPath,x,y,facing) = CreateTrackable(trackableModelPath,x,y,facing)

    <new sound>(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting) = CreateSound(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting)

    <new ubersplat>(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime) = CreateUbersplat(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime)

    <new lightning>(codeName,checkVisibility,x1,y1,x2,y2) = AddLightning(codeName,checkVisibility,x1,y1,x2,y2)
    <new lightning>(codeName,checkVisibility,x1,y1,z1,x2,y2,z2) = AddLightningEx(codeName,checkVisibility,x1,y1,z1,x2,y2,z2)

    <new image>(file,sizeX,sizeY,sizez,posX,posY,posZ,originX,originY,originZ,imageType) = CreateImage(file,sizeX,sizeY,sizeZ,posX,posY,posz,originX,originY,originZ,imageType)
}