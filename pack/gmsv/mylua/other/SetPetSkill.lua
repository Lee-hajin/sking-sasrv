function SetPetSkillLua(charaindex,_data)
	local PetSkillId = other.atoi(other.getString(_data, " ", 1));--获取宠技ID
	local PetNum = other.atoi(other.getString(_data, " ", 2));--获取宠物栏位
	local PetSkillNum = other.atoi(other.getString(_data, " ", 3));--获取宠技栏位
	--给其他玩家设置
	local CdKey = other.getString(_data, " ", 4);--获取帐号
	local toindex = charaindex;
	if CdKey ~= "" then
		toindex = -1;
		local maxplayer = char.getPlayerMaxNum() - 1;
		for i = 0, maxplayer do
			if char.check(i) == 1 then
				if char.getChar(i, "账号") ==  CdKey then
					toindex = i;
					break ;
				end
			end
		end
	end
	
	local token = "";
	if char.check(toindex) > 0 then
		if PetNum < 1 and PetNum > 5 then
			char.TalkToCli(toindex, -1, "宠物栏位范围超出！", 6);
			return ;
		end
			
		if PetSkillNum < 1 and PetSkillNum > 7 then
			char.TalkToCli(toindex, -1, "技能栏位范围超出！", 6);
			return ;
		end
		local PetIndex = char.getCharPet(toindex, PetNum-1);--获取宠物编号
		if char.check(PetIndex) > 0 then
			char.setPetSkill(PetIndex, PetSkillNum - 1, PetSkillId);
			char.TalkToCli(toindex, -1, "设置成功！", 6);
			char.sendStatusString(toindex, "K" .. (PetNum-1));
			char.sendStatusString(toindex, "W" .. (PetNum-1));
			return ;
		else
			char.TalkToCli(toindex, -1, "要设置的宠物栏位没有宠物！["..PetNum.."]", 6);
			return ;
		end
	else
		char.TalkToCli(charaindex, -1, "要设置的帐号不在线！", 6);
		return ;
	end
	
end


function mydata()
	
end

function main()
	mydata();
	magic.addLUAListFunction("设置宠技", "SetPetSkillLua", "", 3, " ");
end
