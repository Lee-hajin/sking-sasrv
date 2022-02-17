function PetBoxCall(itemindex, charaindex, toindex, haveitemindex)
	local strdata = item.getChar(itemindex, "字段");
	local itemnum = char.GetEmptyItemNum(charaindex);--取得道具栏数量
	local petnum = char.GetEmptyPetNum(charaindex);--取得宠物栏数量
	if strdata == "一代1级MM" then
		if petnum < 1 then
			char.TalkToCli(charaindex,-1,"宠物栏必须空出一个位置！",6);
			return ;
		end
		local petindex = char.AddPet( charaindex, 1479,1);
		char.TalkToCli(charaindex, -1, "获得 ".. char.getChar(petindex,"名字") .."！", 4);
	elseif strdata == "二代1级MM" then
		if petnum < 1 then
			char.TalkToCli(charaindex,-1,"宠物栏必须空出一个位置！",6);
			return ;
		end
		local petindex = char.AddPet( charaindex, 2547,1);
		char.TalkToCli(charaindex, -1, "获得 ".. char.getChar(petindex,"名字") .."！", 4);
	else
		char.TalkToCli(charaindex, -1, "暂未开放！", 6);
		return;
	end
	char.DelItem(charaindex, haveitemindex);--删除道具
end

------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.TalkToCli(_charaindex, -1, "重读[PetBox]完成", 6);
	end
end

function mydata()
	
end

function main()
	mydata();
	item.addLUAListFunction( "ITEM_PETBOX", "PetBoxCall", "");
	magic.addLUAListFunction("重读PetBox", "reload", "", 3, "[重读PetBox]");
end
