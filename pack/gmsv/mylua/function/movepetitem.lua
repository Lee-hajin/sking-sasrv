--宠物骑乘中判断事件
--charaindex：玩家索引
--petindex：宠物索引
--charitemindex：要装备的道具索引
--petitemindex：要装备的位置当前已装备的道具索引
--fromindex：目标道具栏位置
--toindex：要装备的道具道具栏位置
function FreeMovePetItem(charaindex,petindex, charitemindex, petitemindex,fromindex,toindex)
	--print(charaindex,petindex, charitemindex, petitemindex,toindex,fromindex);
	--print(char.getInt(petindex,"宠ID"));
	if char.getInt(petindex, "融合类型") <= 0 then
		char.TalkToCli(charaindex, -1, "非融合宠，无法装载宠物装备!", "红色");
		return 0;
	end
	
	local petid = char.getInt(petindex,"宠ID");
	local rideindex = char.getCharPet(charaindex,char.getInt(charaindex,"骑宠"));--取得骑宠索引
	if rideindex == petindex then
		char.TalkToCli(charaindex, -1, "骑宠不可装装备！", "红色");
		return 0;
	end
	for i=1,table.getn(NoPetItem) do
		if petid == NoPetItem[i] then
			char.TalkToCli(charaindex, -1, "该宠物无法装备道具！", "红色");
			return 0;
		end
	end
	return 1;
end

function mydata()
	--不可装备道具的宠物enemybaseID
	NoPetItem = {};			 
end


function reload()
	print("重读宠装限制完成");
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读宠装限制", "reload", "", 1, "[重读宠装限制]");
end
