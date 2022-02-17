------------------------------- 杂类模块 -------------------------------
function StringSplit(_data, _splitint)--分割处理函数（返回数组）
	local StartIndex = 1;
	local SplitIndex = 1;
	local SplitArray = {};
	while true do
		local FindLastIndex = string.find(_data, _splitint, StartIndex);
		if not FindLastIndex then
			SplitArray[SplitIndex] = string.sub(_data, StartIndex, string.len(_data));
			break ;
		end
		SplitArray[SplitIndex] = string.sub(_data, StartIndex, FindLastIndex - 1);
		StartIndex = FindLastIndex + string.len(_splitint);
		SplitIndex = SplitIndex + 1;
	end
	return SplitArray;
end

--NPC战斗结束事件(NPC索引， 战斗索引，赢败)
function BattleOver(meindex, battleindex, iswin)--怪物战后事件
	local charaindex = battle.getCharOne(battleindex, 0, 0);--取出玩家索引
	
	local mysx = char.getWorkInt(charaindex, "临时整数1");
	local zcsx = char.getWorkInt(charaindex, "临时整数2");
	local qcsx = char.getWorkInt(charaindex, "临时整数3");
	local zcindex = char.getWorkInt(charaindex, "临时整数4");
	local qcindex = char.getWorkInt(charaindex, "临时整数5");
	
	local myd1 = char.getInt(charaindex, "地魔法抗性") - char.getRightTo8(mysx, 1);
	local mys1 = char.getInt(charaindex, "水魔法抗性") - char.getRightTo8(mysx, 2);
	local myh1 = char.getInt(charaindex, "火魔法抗性") - char.getRightTo8(mysx, 3);
	local myf1 = char.getInt(charaindex, "风魔法抗性") - char.getRightTo8(mysx, 4);
	local myd2,mys2,myh2,myf2 = 0;
	if zcindex > 0 then
		myd2 = char.getInt(zcindex, "地魔法抗性") - char.getRightTo8(zcsx, 1);
		mys2 = char.getInt(zcindex, "水魔法抗性") - char.getRightTo8(zcsx, 2);
		myh2 = char.getInt(zcindex, "火魔法抗性") - char.getRightTo8(zcsx, 3);
		myf2 = char.getInt(zcindex, "风魔法抗性") - char.getRightTo8(zcsx, 4);
	end
	local myd3,mys3,myh3,myf3 = 0;
	if qcindex > 0 then
		myd3 = char.getInt(qcindex, "地魔法抗性") - char.getRightTo8(qcsx, 1);
		mys3 = char.getInt(qcindex, "水魔法抗性") - char.getRightTo8(qcsx, 2);
		myh3 = char.getInt(qcindex, "火魔法抗性") - char.getRightTo8(qcsx, 3);
		myf3 = char.getInt(qcindex, "风魔法抗性") - char.getRightTo8(qcsx, 4);
	end
	
	local token = "魔法抗性提升：\n"
	char.TalkToCli(charaindex, -1, token, 5);
	
	token = string.format("地魔法：[人物]%-3d -> %-3d Exp:%-3d",char.getRightTo8(mysx, 1),char.getInt(charaindex, "地魔法抗性"),myd1);
	if zcindex > 0 then
		token = token .. string.format(" [战宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(zcsx, 1),char.getInt(zcindex, "地魔法抗性"),myd2);
	end
	if qcindex > 0 then
		token = token .. string.format(" [骑宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(qcsx, 1),char.getInt(qcindex, "地魔法抗性"),myd3);
	end
	char.TalkToCli(charaindex, -1, token, 6);
	token = string.format("水魔法：[人物]%-3d -> %-3d Exp:%-3d",char.getRightTo8(mysx, 2),char.getInt(charaindex, "水魔法抗性"),mys1);
	if zcindex > 0 then
		token = token .. string.format(" [战宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(zcsx, 2),char.getInt(zcindex, "水魔法抗性"),mys2);
	end
	if qcindex > 0 then
		token = token .. string.format(" [骑宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(qcsx, 2),char.getInt(qcindex, "水魔法抗性"),mys3);
	end
	char.TalkToCli(charaindex, -1, token, 6);
	token = string.format("火魔法：[人物]%-3d -> %-3d Exp:%-3d",char.getRightTo8(mysx, 3),char.getInt(charaindex, "火魔法抗性"),myh1);
	if zcindex > 0 then
		token = token .. string.format(" [战宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(zcsx, 3),char.getInt(zcindex, "火魔法抗性"),myh2);
	end
	if qcindex > 0 then
		token = token .. string.format(" [骑宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(qcsx, 3),char.getInt(qcindex, "火魔法抗性"),myh3);
	end
	char.TalkToCli(charaindex, -1, token, 6);
	token = string.format("风魔法：[人物]%-3d -> %-3d Exp:%-3d",char.getRightTo8(mysx, 4),char.getInt(charaindex, "风魔法抗性"),myf1);
	if zcindex > 0 then
		token = token .. string.format(" [战宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(zcsx, 4),char.getInt(zcindex, "风魔法抗性"),myf2);
	end
	if qcindex > 0 then
		token = token .. string.format(" [骑宠]%-3d -> %-3d Exp:%-3d",char.getRightTo8(qcsx, 4),char.getInt(qcindex, "风魔法抗性"),myf3);
	end
	char.TalkToCli(charaindex, -1, token, 6);
	
	
	char.setWorkInt(charaindex, "临时整数1",0);
	char.setWorkInt(charaindex, "临时整数2",0);
	char.setWorkInt(charaindex, "临时整数3",0);
	char.setWorkInt(charaindex, "临时整数4",0);
	char.setWorkInt(charaindex, "临时整数5",0);
end

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if (_seqno == 0) then
		if _data == "1" then
			local myd = char.getInt(_talkerindex, "地魔法抗性");
			local mys = char.getInt(_talkerindex, "水魔法抗性");
			local myh = char.getInt(_talkerindex, "火魔法抗性");
			local myf = char.getInt(_talkerindex, "风魔法抗性");
			local mysx = char.getLiftTo8(myd, 1) + char.getLiftTo8(mys, 2) + char.getLiftTo8(myh, 3) + char.getLiftTo8(myf, 4);
			char.setWorkInt(_talkerindex, "临时整数1",mysx);
			
			local petindex = char.getCharPet(_talkerindex, char.getInt(_talkerindex,"战宠"));
			local zcd,zcs,zch,zcf,zcsx = 0;
			if char.check(petindex) == 1 then
				zcd = char.getInt(petindex, "地魔法抗性");
				zcs = char.getInt(petindex, "水魔法抗性");
				zch = char.getInt(petindex, "火魔法抗性");
				zcf = char.getInt(petindex, "风魔法抗性");
				zcsx = char.getLiftTo8(zcd, 1) + char.getLiftTo8(zcs, 2) + char.getLiftTo8(zch, 3) + char.getLiftTo8(zcf, 4);
				char.setWorkInt(_talkerindex, "临时整数2",zcsx);
				char.setWorkInt(_talkerindex, "临时整数4",petindex);
			end
			petindex = char.getCharPet(_talkerindex, char.getInt(_talkerindex,"骑宠"));
			local qcd,qcs,qch,qcf,qcsx = 0;
			if char.check(petindex) == 1 then
				qcd = char.getInt(petindex, "地魔法抗性");
				qcs = char.getInt(petindex, "水魔法抗性");
				qch = char.getInt(petindex, "火魔法抗性");
				qcf = char.getInt(petindex, "风魔法抗性");
				qcsx = char.getLiftTo8(qcd, 1) + char.getLiftTo8(qcs, 2) + char.getLiftTo8(qch, 3) + char.getLiftTo8(qcf, 4);
				char.setWorkInt(_talkerindex, "临时整数3",qcsx);
				char.setWorkInt(_talkerindex, "临时整数5",petindex);
			end
			battle.CreateVsEnemy(_talkerindex, _meindex, {1690});
		elseif _data == "2" then
			local myd = char.getInt(_talkerindex, "地魔法抗性");
			local mys = char.getInt(_talkerindex, "水魔法抗性");
			local myh = char.getInt(_talkerindex, "火魔法抗性");
			local myf = char.getInt(_talkerindex, "风魔法抗性");
			local mysx = char.getLiftTo8(myd, 1) + char.getLiftTo8(mys, 2) + char.getLiftTo8(myh, 3) + char.getLiftTo8(myf, 4);
			char.setWorkInt(_talkerindex, "临时整数1",mysx);
			
			local petindex = char.getCharPet(_talkerindex, char.getInt(_talkerindex,"战宠"));
			local zcd,zcs,zch,zcf,zcsx = 0;
			if char.check(petindex) == 1 then
				zcd = char.getInt(petindex, "地魔法抗性");
				zcs = char.getInt(petindex, "水魔法抗性");
				zch = char.getInt(petindex, "火魔法抗性");
				zcf = char.getInt(petindex, "风魔法抗性");
				zcsx = char.getLiftTo8(zcd, 1) + char.getLiftTo8(zcs, 2) + char.getLiftTo8(zch, 3) + char.getLiftTo8(zcf, 4);
				char.setWorkInt(_talkerindex, "临时整数2",zcsx);
				char.setWorkInt(_talkerindex, "临时整数4",petindex);
			end
			
			
			petindex = char.getCharPet(_talkerindex, char.getInt(_talkerindex,"骑宠"));
			local qcd,qcs,qch,qcf,qcsx = 0;
			if char.check(petindex) == 1 then
				qcd = char.getInt(petindex, "地魔法抗性");
				qcs = char.getInt(petindex, "水魔法抗性");
				qch = char.getInt(petindex, "火魔法抗性");
				qcf = char.getInt(petindex, "风魔法抗性");
				qcsx = char.getLiftTo8(qcd, 1) + char.getLiftTo8(qcs, 2) + char.getLiftTo8(qch, 3) + char.getLiftTo8(qcf, 4);
				char.setWorkInt(_talkerindex, "临时整数3",qcsx);
				char.setWorkInt(_talkerindex, "临时整数5",petindex);
			end
			
			battle.CreateVsEnemy(_talkerindex, _meindex, {1690,1690});
		elseif _data == "3" then
			local petdata = {{-1,0,0,0,0}
							,{-1,0,0,0,0}
							,{-1,0,0,0,0}
							,{-1,0,0,0,0}
							,{-1,0,0,0,0}
							};
			for i=0,4 do
				local petindex = char.getCharPet(_talkerindex, i);
				if char.check(petindex) == 1 then
					petdata[i+1][1] = petindex;
					petdata[i+1][2] = char.getInt(petindex, "地魔法抗性");
					petdata[i+1][3] = char.getInt(petindex, "水魔法抗性");
					petdata[i+1][4] = char.getInt(petindex, "火魔法抗性");
					petdata[i+1][5] = char.getInt(petindex, "风魔法抗性");
					
				end
			end
			local token = "                  [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]\n"
						.."魔法抗性明细如下:\n"
						..string.format("%-16s  [style c=5]%-3s[/style]   [style c=1]%-3s[/style]   [style c=6]%-3s[/style]   [style c=4]%-3s[/style]\n","","地","水","火","风")
						..string.format("%-16s：[style c=5]%-3d[/style]   [style c=1]%-3d[/style]   [style c=6]%-3d[/style]   [style c=4]%-3d[/style]\n",char.getChar(_talkerindex,"名字"),char.getInt(_talkerindex, "地魔法抗性"), char.getInt(_talkerindex, "水魔法抗性"), char.getInt(_talkerindex, "火魔法抗性"), char.getInt(_talkerindex, "风魔法抗性"))
						.."[style c=7]------------------------------------------[/style]\n";
			for i=1,5 do
				if petdata[i][1] == -1 then
					token = token .. string.format("[style c=7]%-16s：该栏位没有宠物[/style]\n", "宠物栏"..i);
				else
					token = token .. string.format("%-16s：[style c=5]%-3d[/style]   [style c=1]%-3d[/style]   [style c=6]%-3d[/style]   [style c=4]%-3d[/style]\n",char.getChar(petdata[i][1],"名字"),petdata[i][2],petdata[i][3],petdata[i][4],petdata[i][5]);
				end
			end
			
			lssproto.windows(_talkerindex, 45, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "3\n                  [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n"
					.."\n                [练习魔法抗性([style c=5]普通[/style])]"
					.."\n                [练习魔法抗性([style c=5]高级[/style])]"
					.."\n                  [查看魔法抗性]";
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	char.setFunctionPointer(npcindex, "战后事件", "BattleOver", "");
end



------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	npcdata = {"练魔法抗性",100441,777,49,49,6,"LianMoKang"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
