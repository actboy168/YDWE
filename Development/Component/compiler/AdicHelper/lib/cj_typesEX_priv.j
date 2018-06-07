/*
	cj_typesEX_priv.j

	Extended replacement of some constructs, dealing with game object creation.
	Private version, recommended for usage in imported libraries etc

	This file is part of cJass standard library
	© 2009 Van Damm \\ cjass.xgm.ru
*/

define {
    private <new hashtable> = InitHashtable()
    private <new timer> = CreateTimer()
    private <new group> = CreateGroup()
    private <new force> = CreateForce()
    private <new region> = CreateRegion()
    private <new trigger> = CreateTrigger()
    private <new dialog> = DialogCreate()
    private <new leaderboard> = CreateLeaderboard()
    private <new multiboard> = CreateMultiboard()
    private <new quest> = CreateQuest()
    private <new texttag> = CreateTextTag()
    private <new camerasetup> = CreateCameraSetup()
    private <new timerdialog>(t) = CreateTimerDialog(t)
    private <new gamecache>(file) = InitGameCache(file)
    private <new weathereffect>(where,effectID) = AddWeatherEffect(where,effectID)

    private <new player>() = GetLocalPlayer()
    private <new player>(id) = Player(id)

    private <new location>() = Location(0,0)
    private <new location>(x,y) = Location(x,y)

    private <new rect>() = Rect(0,0,0,0)
    private <new rect>(x1,y1,x2,y2) = Rect(x1,y1,x2,y2)

    private <new item>(itemid) = CreateItem(itemid,0,0)
    private <new item>(itemid,x,y) = CreateItem(itemid,x,y)

    private <new unit>(p,id) = CreateUnit(p,id,0,0,0)
    private <new unit>(p,id,x,y) = CreateUnit(p,id,x,y,0)
    private <new unit>(p,id,x,y,f) = CreateUnit(p,id,x,y,f)

    private <new effect>(modelName) = AddSpecialEffect(modelName,0,0)
    private <new effect>(modelName,x,y) = AddSpecialEffect(modelName,x,y)

    private <new destructable>(objectid,x,y) = CreateDestructable(objectid,x,y,1,0)
    private <new destructable>(objectid,x,y,z) = CreateDestructableZ(objectid,x,y,z,1,0)
    private <new destructable>(objectid,x,y,facescale,variation) = CreateDestructable(objectid,x,y,facescale,variation)
    private <new destructable>(objectid,x,y,z,facescale,variation) = CreateDestructableZ(objectid,x,y,z,facescale,variation)

    private <new trackable>(trackableModelPath,x,y,facing) = CreateTrackable(trackableModelPath,x,y,facing)

    private <new sound>(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting) = CreateSound(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting)

    private <new ubersplat>(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime) = CreateUbersplat(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime)

    private <new lightning>(codeName,checkVisibility,x1,y1,x2,y2) = AddLightning(codeName,checkVisibility,x1,y1,x2,y2)
    private <new lightning>(codeName,checkVisibility,x1,y1,z1,x2,y2,z2) = AddLightningEx(codeName,checkVisibility,x1,y1,z1,x2,y2,z2)

    private <new image>(file,sizeX,sizeY,sizez,posX,posY,posZ,originX,originY,originZ,imageType) = CreateImage(file,sizeX,sizeY,sizeZ,posX,posY,posz,originX,originY,originZ,imageType)
}