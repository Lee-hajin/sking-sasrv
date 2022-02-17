function SetPetLv(itemindex, charaindex, toindex, haveitemindex)
	for i = 0, 4 do
		if toindex == char.getCharPet(charaindex, i) then
			level = item.getInt(itemindex, "敏");
			--print(level);
			if char.getInt(toindex, "限制等级") > 0 then
				levelup = math.min(char.getInt(toindex, "限制等级"), level);
			else
				levelup = math.min(level, 140);
			end

			if char.getInt(toindex, "等级") > levelup then
				char.TalkToCli(charaindex, -1, char.getChar(toindex, "名字") .. "等级高于该物品所提升等级，无需使用该物品！", "红色");
				return;
			end
			
			local lv = char.getInt(toindex, "等级");
			char.setInt(toindex, "等级", 0);
			for j = lv + 1, levelup do
				char.PetLevelUp(toindex);
			end

			char.setInt(toindex, "等级", levelup);
			char.complianceParameter(toindex);

			char.setInt(toindex, "HP", char.getWorkInt(toindex, "最大HP"));
			char.sendStatusString(charaindex, "K" .. i);
			char.TalkToCli(charaindex, -1, char.getChar(toindex, "名字") .. "等级上升至" .. levelup .. "级！", "黄色");
			char.DelItem(charaindex, haveitemindex);
			return;
		end
	end

	char.TalkToCli(charaindex, -1, "该物品只能给宠物使用！", "红色");
end

function main()
	item.addLUAListFunction( "ITEM_SETPETLV", "SetPetLv", "");
end
