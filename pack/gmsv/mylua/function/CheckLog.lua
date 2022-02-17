--判断道具是否记录LOG(返回1记录，返回0不记录)
function CheckItemLog( charaindex,itemindex, itemid, cdkey, uniquecode)
	local itemUse = item.getChar(itemindex,"使用函数名");--道具使用函数名
	--不记录LOG的道具函数名
	local NoLogUse = {"ITEM_useRecovery"
					 };
	for i=1,table.getn(NoLogUse) do
		if itemUse == NoLogUse[i] then
			return 0;
		end
	end
	return 1;
end

--判断宠物是否记录LOG(返回1记录，返回0不记录)
function CheckPetLog( charaindex,petindex, itemid, cdkey, uniquecode)
	local initlv = char.getInt(petindex,"宠初等级");
	local petlv = char.getInt(petindex,"等级");
	if initlv > 1 then--初始等于大于1
		if initlv == petlv then--并且初始等于和当前等级一样则不记录
			return 0;
		end
	end
	return 1;
end

function mydata()

end

function reload(charaindex, data)
	mydata();
end

function main()
	mydata();
end
