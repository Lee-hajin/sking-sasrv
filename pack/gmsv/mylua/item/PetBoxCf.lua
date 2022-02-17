function checkEmptPetNum(charaindex)
	EmptyPetNum = 0
	for i = 0, 4 do
		if char.check(char.getCharPet(charaindex, i)) == 0 then
			EmptyPetNum = EmptyPetNum + 1
		end
	end
	return EmptyPetNum
end

function GiveMM(itemindex, charaindex, toindex, haveitemindex)
	local MAXVARIABLEAI = 100*100;
	local MINVARIABLEAI = -100*100;
	local LevelUpPoint = 0;
	local iWork = 0;
	if checkEmptPetNum(charaindex) == 0 then
		char.TalkToCli(charaindex, -1, "很抱歉，您的身上宠物已满！", "红色");
		return;
	end
		
	local itembuf = item.getChar(itemindex,"字段");
	if itembuf == "" then
		return;
	end
	
	petid = other.atoi(other.getString(itembuf,"|",1));
	lv = other.atoi(other.getString(itembuf,"|",2));
	lock = other.atoi(other.getString(itembuf,"|",3));
	
	hp = other.getString(itembuf,"|",4);
	str = other.getString(itembuf,"|",5);
	tgh = other.getString(itembuf,"|",6);
	dex = other.getString(itembuf,"|",7);
	
	if hp == "" or str == "" or tgh == "" or dex == "" then
		return;
	end

	local MMIndex = char.AddPet(charaindex,petid,1);

	while(char.getInt(MMIndex, "等级")<lv) do
		LevelUpPoint = other.NumLeftToNum(other.atoi(hp),24) + other.NumLeftToNum(other.atoi(str),16) + other.NumLeftToNum(other.atoi(tgh),8) + other.NumLeftToNum(other.atoi(dex),0);
		char.setInt(MMIndex, "能力值", LevelUpPoint);
		char.PetLevelUp(MMIndex);
		iWork = char.getInt(MMIndex,"可变AI") + 500;
		iWork = math.min(MAXVARIABLEAI,iWork);
		iWork = math.max(MINVARIABLEAI,iWork);
		char.setInt(MMIndex,"可变AI",iWork);
		char.setInt(MMIndex, "等级", char.getInt(MMIndex, "等级") + 1);
	end
	char.setInt(MMIndex,"可变AI",10000);
	char.complianceParameter(MMIndex);
	char.setInt( MMIndex, "HP", char.getWorkInt( MMIndex, "最大HP" ));
	if lock == 1 then
		char.setInt(MMIndex,"绑定",1);
	end
	char.TalkToCli(charaindex, -1, "得到一只 ".. char.getChar(MMIndex,"名字").. " 。", "黄色");
	for i = 0, 4 do
		local pindex = char.getCharPet( charaindex, i);
		if char.check(pindex) == 1 then
			if pindex == MMIndex then
				char.sendStatusString(charaindex, "K" .. i);
			end
		end
	end
	char.DelItem(charaindex, haveitemindex);
end


function main()
	--item.addLUAListFunction( "ITEM_MM", "GiveMM", "")
	item.addLUAListFunction( "ITEM_PETBOXCF", "GiveMM", "")
	
	
end
