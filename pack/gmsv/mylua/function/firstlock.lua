--交易后宠物进行处理
function FreeFirstLockPet( charaindex, petindex )
	for i = 1, table.getn(petlist1) do
		if char.getInt(petindex, "原图像号") == petlist1[i] then
			char.setInt(petindex, "绑定", 1);
			char.TalkToCli(charaindex, -1, char.getChar(petindex, "名字") .. "交易成功，系统自动将其进行绑定！", "绿色");
		end
	end
end

--交易后道具进行处理
function FreeFirstLockItem( itemindex )
	for i = 1, table.getn(itemlist1) do
		if item.getInt(petindex, "序号") == itemlist1[i] then
			item.setInt(itemindex, "绑定", 1);
			char.TalkToCli(charaindex, -1, item.getChar(itemlist1, "名字") .. "交易成功，系统自动将其进行绑定！", "绿色");
		end
	end
end

function data()
	petlist1 = {1,1,1,1,1};--交易后绑定的宠物图像
	itemlist1 = {};--交易后绑定的道具编号
end

function main()
	data();
end

