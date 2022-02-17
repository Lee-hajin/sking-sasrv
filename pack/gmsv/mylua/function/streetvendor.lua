function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end
--摆摊判断，返回1则不允许摆摊，返回0则可摆摊
function FreeStreetVendorName( charaindex, streetname )
	if char.getInt(charaindex,"地图号") == 2005 or char.getInt(charaindex,"地图号") == 2006 then
		char.TalkToCli(charaindex, -1, "摆摊失败,该地图禁止摆摊！", "红色");
		return 1;
	end
	local nostreetname = {"gm","GM","Gm","坑","ＧＭ","不玩","退服","圾服","闪人","脱","遗产"}
	for i = 1, table.getn(nostreetname) do
		str, len = string.gsub(streetname, nostreetname[i], "")
		if len > 0 then
			char.TalkToCli(charaindex, -1, "[错误提示]非法店名，摆摊失败，摆摊广告名请带一些正能量，谢谢合作。", "红色")
			return 1
		end
	end
	return 0
end

function FreeStreetVendorPet( charaindex, petindex )
	if char.getInt(charaindex,"地图号") == 2005 or char.getInt(charaindex,"地图号") == 2006 then
		char.TalkToCli(charaindex, -1, "摆摊失败,该地图禁止摆摊！", "红色");
		return 1;
	end
	return 0
end

function FreeStreetVendorItem( charaindex, itemindex )
	if char.getInt(charaindex,"地图号") == 2005 or char.getInt(charaindex,"地图号") == 2006 then
		char.TalkToCli(charaindex, -1, "摆摊失败,该地图禁止摆摊！", "红色");
		return 1;
	end
	
	return 0
end

function data()

end

function main()
	data()
end

