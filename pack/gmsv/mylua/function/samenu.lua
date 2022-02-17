function FreeSaMenu(charaindex, tonidex,index1,index2,index3)
	local battletype = char.getWorkInt(charaindex,"战斗");
	parameter = {charaindex};
	if index1 == 1 then
		if index2 == 1 then--官放网站
			lssproto.SaMenu(charaindex, "http://www.test.com");
		elseif index2 == 2 then--赞助充值
			lssproto.SaMenu(charaindex, "http://www.test.com");
		elseif index2 == 4 then--原地遇敌
			if index3 == 0 then
				char.StopEncounter(charaindex);
			else
				char.Encounter(charaindex);
			end
		elseif index2 == 5 then--屏蔽玩家
			if index3 == 1 then
				char.setforpigcomlua(charaindex);--设置屏蔽
			elseif index3 == 0 then
				char.clearforpigcomlua(charaindex);--取消屏蔽
			end
			--print("\n状态:"..char.getforpigcomlua(charaindex));
		elseif index2 == 6 then--支票签发
			other.CallFunction("FreeZhiPiao", "mylua/npc/ZhiPiao/ZhiPiao.lua", parameter);
		elseif index2 == 7 then--叠加
			other.CallFunction("ItemBoxToItemBox", "mylua/function/chartalkfunction.lua", parameter);
		elseif index2 == 8 then--贴心传送
			other.CallFunction("FreeTieXin", "mylua/npc/Transfer/TieXin.lua", parameter);
		elseif index2 == 9 then--个人信息
			other.CallFunction("FreeFameShow", "mylua/function/fameshow.lua", parameter);
		elseif index2 == 10 then--防卡
			local fd = char.getFd(charaindex);--获取FD
			--lssproto.show(charaindex,char.getWorkInt(charaindex,"对象"));
			net.echosend(fd);
			--char.WarpToSpecificPoint(charaindex, char.getInt(charaindex, "地图号"), char.getInt(charaindex, "坐标X"), char.getInt(charaindex, "坐标Y"));
		elseif index2 == 11 then--召唤
			--other.CallFunction("Accompany", "mylua/item/Accompany.lua", parameter);
			char.TalkToCli(charaindex, -1, "暂未开放", "红色");
		end
	elseif index1 == 2 then
	elseif index1 == 3 then
		if index2 == 1 then--骑宠查看
			other.CallFunction("FreeRideQuery", "mylua/function/familyridefunction.lua", parameter);
		elseif index2 == 2 then--在线商城
			other.CallFunction("FreeShopVip", "mylua/npc/Shop/VipShop/VipShop.lua", parameter);
		elseif index2 == 3 then--每日签到
			other.CallFunction("ShowOneDay", "mylua/npc/OneDay/OneDay.lua", parameter);
		elseif index2 == 4 then--观看战斗
			battle.WatchEntry(charaindex,tonidex);--观战按钮
		elseif index2 == 5 then--邮件系统
			other.CallFunction("FreeEmail", "mylua/npc/Email/Email.lua", parameter);
		elseif index2 == 6 then--红包系统
			other.CallFunction("FreeRedBox", "mylua/npc/RedBox/RedBox.lua", parameter);
		end
	elseif index1 == 4 then
		if index2 == 1 then
			other.CallFunction("FreeSkin", "mylua/npc/PlayerSkin/PlayerSkin.lua", parameter);
		elseif index2 == 2 then
			other.CallFunction("FreeTitle", "mylua/npc/PlayerTitle/PlayerTitle.lua", parameter);
		elseif index2 == 3 then
			other.CallFunction("FreeHalo", "mylua/npc/PlayerHalo/PlayerHalo.lua", parameter);
		end
	elseif index1 == 5 then
		if index2 == 1 then--秀宠
			local petindex = other.atoi(index3);
			if char.check(petindex) == 1 then
				local PetA1 = char.getChar(petindex,"名字");
				local PetA2 = char.getInt(petindex,"原图像号");
				local PetA3 = char.getInt(petindex,"转数");
				local PetA4 = char.getInt(petindex,"等级");
				local PetA5 = char.getInt(petindex,"融合宠");
						
				local PetB1 = char.getInt(petindex,"宠初等级");
				local PetB2 = char.getInt(petindex, "宠初耐久");
				local PetB3 = char.getInt(petindex, "宠初攻击");
				local PetB4 = char.getInt(petindex, "宠初防御");
				local PetB5 = char.getInt(petindex, "宠初敏捷");
						
				local PetC1 = char.getWorkInt(petindex,"最大HP");
				local PetC2 = char.getWorkInt(petindex,"攻击力");
				local PetC3 = char.getWorkInt(petindex,"防御力");
				local PetC4 = char.getWorkInt(petindex,"敏捷力");
				
				local PetD1 = char.getInt(petindex,"地");
				local PetD2 = char.getInt(petindex,"水");
				local PetD3 = char.getInt(petindex,"火");
				local PetD4 = char.getInt(petindex,"风");
				
				local PetSK = {"","","","","","",""};
				for i=0,6 do
					local TMSKid = char.GetPetSkill( petindex, i);
					if TMSKid >= 0 then
						PetSK[i+1] = petskill.getChar( TMSKid, "名称"); 
					end
				end
				local PetE1 = "";
				for i=1,7 do
					if i == 7 then
						PetE1 = PetE1 .. PetSK[i];
					else
						PetE1 = PetE1 .. PetSK[i] .. "|";
					end
				end
				local retstr = PetA1.."|"..PetA2.."|"..PetA3.."|"..PetA4.."|"..PetA5.."\n"
							.. PetB1.."|"..PetB2.."|"..PetB3.."|"..PetB4.."|"..PetB5.."\n"
							.. PetC1.."|"..PetC2.."|"..PetC3.."|"..PetC4.."\n"
							.. PetD1.."|"..PetD2.."|"..PetD3.."|"..PetD4.."\n"
							.. PetE1.."\n";	
				--print(retstr);
				lssproto.windows(charaindex, 1016, 12, -1, char.getWorkInt( npcindex, "对象"), retstr);
			else
				char.TalkToCli(charaindex, -1, "该宠物已经下线或者不存在了！", "红色");
			end
		elseif index2 == 2 then--坐标
			print(index3);
		end
	else
		print(index1,index2,index3);
		char.TalkToCli(charaindex, -1, "暂未开放", "红色")
	end
end

function WindowTalked( meindex, talkerindex, seqno, select, data)
	if seqno == 0 then
		local num = tonumber(data);
		if num > 0 and num < 5 then
			if char.getInt(partytable[talkerindex][num],"类型") ~= 1 then
				char.TalkToCli(talkerindex, -1, "队长只能转让给玩家。", "黄色")
				return
			end
			char.DischargeParty(talkerindex,0);
			char.JoinParty(partytable[talkerindex][num],talkerindex,0);
			char.TalkToCli(partytable[talkerindex][num], -1, "您的队友【"..char.getChar(talkerindex,"名字").."】把队长权力交给您！", "黄色")
			char.TalkToCli(talkerindex, -1, "您成功把队长权力交给队友【"..char.getChar(partytable[talkerindex][num],"名字").."】", "黄色");
			for i =1,table.getn(partytable[talkerindex]) do
				if i ~= num then
					char.JoinParty(partytable[talkerindex][num],partytable[talkerindex][i],0);
				end
			end
			char.Encounter(partytable[talkerindex][num]);
		end
	end
end

function Create(name, metamo, floor, x, y, dir)
	npcindex = npc.CreateNpc(name, metamo, floor, x, y, dir)
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "")
end

function mydata()
	partytable={};

end

function main()
	mydata();
	Create("samenu窗口", 100000, 777, 32, 18, 6);
end

