function startswith(sbig, slittle)
  return string.sub(sbig, 1, string.len(slittle)) == slittle
end

-- callback is called for each imported tga/bmp/jpg file, example for the arguments:
-- sourcefile=CMDPASBTNDiscoBall.tga
-- targetpath=ReplaceableTextures\CommandButtons\BTNDiscoBall.blp
-- width=64
-- height=64
-- bytesperpixel=4

function getcompressioninfo(sourcefile, targetpath, width, height, bytesperpixel)
  logf(sourcefile, targetpath, width, height, bytesperpixel) -- write parameters to log
  if (startswith(sourcefile, "CMD") or startswith(sourcefile, "PAS") or startswith(sourcefile, "AUT") or startswith(sourcefile, "infocard-") or startswith(sourcefile, "level-infocard-")) then
    return true, 256, 2 -- paletted, 256 colors, 2 mipmaps
  else
    return false, 85, 16 -- jpg-compressed, 85% quality, 16 mipmaps
  end
end

registercallback(getcompressioninfo) -- register the above callback
importdirectory("import")
importfile("CMDAUTPASBTNAcorn.tga", "Acorn.blp")
importfile("PASGem0.tga", "Gem0.blp")
importfile("infocard-Gem1.jpg", "Textures\\infocard-Gem1.blp")
importfile("level-infocard-Slot.bmp", "Textures\\level-infocard-Slot.blp")
importfile("MoreMusic.mdl", "MoreMusic.mdx")
importfile("CMDAUTPASBTNAcorn.tga", "NotConverted\\Acorn.tga")
importfile("PASGem0.tga", "NotConverted\\Gem0.tga")
importfile("infocard-Gem1.jpg", "NotConverted\\infocard-Gem1.jpg")
importfile("level-infocard-Slot.bmp", "NotConverted\\level-infocard-Slot.bmp")
importfile("MoreMusic.mdl", "NotConverted\\MoreMusic.mdl")
