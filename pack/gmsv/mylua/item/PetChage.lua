function StringMsg( _Data)--判断是否为数字和字母
	if( string.find(_Data,"'") or string.find(_Data,"\"") or string.find(_Data,"%c") or string.find(_Data,"%p") or string.find(_Data,"%s")) then
		return false;
	else
		return true;
	end
end

function ShowTalked(_meindex, _talkerindex)
	--char.setWorkInt(_talkerindex, "临时整数1",-1);
	lssproto.windows(_talkerindex, "宠物框", 8, 1, char.getWorkInt(_meindex, "对象"), "");
end

function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then
		ShowTalked(_meindex, _talkerindex);
	end
end

function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if (_select == 8) then
		return;
	end
	if(_seqno==1)then
		local setid = other.atoi(_data);--项目索引
		if setid == -1 then
			return;
		end
		--print(setid);
		local petindex = char.getCharPet(_talkerindex, setid-1);
		if char.getInt(petindex, "类型") ~= 3 then
			char.TalkToCli(_talkerindex, -1, "您选择的对像非宠物，无法进行回炉!", "红色");
			return;
		end
		--判断是否骑乘中
		for i = 0, 4 do
			if petindex == char.getCharPet(_talkerindex, i) then
				if char.getInt(_talkerindex, "骑宠") == i then
					char.TalkToCli(_talkerindex, -1, "骑乘中的宠物无法回炉!", "红色");
					return;
				end
			end
		end
		local enemyid = pet.GetEnemyId(petindex);
		local enemybaseid = pet.GetEnemyBaseId(petindex);--取得enemybaseid
		local token = "select * from `PetHuiLu` where `EnemyBase` = "..enemybaseid;
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			num = sasql.num_rows();
			if num == 0 then
				char.TalkToCli(_talkerindex, -1, "该宠物无法进行回炉!("..enemybaseid.."-"..enemyid..")", "红色");
				return;
			end
			sasql.fetch_row();
			local sqlenemyid = other.atoi(sasql.data(1));
			local sqlenemybaseid = other.atoi(sasql.data(2));
			local sqlpoint = other.atoi(sasql.data(3));
			local sqlname = sasql.data(4);
			if sqlname == nil or sqlname == "" then--如果宠物名字不存在的话，给宠物补足名字
				sqlname = pet.getDataChar(sqlenemyid, "名字");
				token = "UPDATE `PetHuiLu` SET Name='"..sqlname.."' where `EnemyBase` = "..enemybaseid;
				--print(token);
				sasql.query(token);
			end
			token = "　　　　　　  【回炉使用说明】\n"
					.. "\n宠物使用后可恢复随机初始0转1级状态！"
					.. "\n[style c=6]一旦回炉将无法复原哦！请慎重操作！[/style]"
					.. "\n"
					.. "\n[style c=1]选择宠物:"..sqlname.."[/style]"
					.. "\n[style c=5]回炉消耗:"..sqlpoint.."金币[/style]";
			lssproto.windows(_talkerindex, 0, 12, 2, char.getWorkInt(_meindex, "对象"), token);
			char.setWorkInt(_talkerindex, "临时整数2",setid-1);
			char.setWorkInt(_talkerindex, "临时整数3",petindex);
		end
	elseif (_seqno==2)then
		local haveitemindex = char.getWorkInt(_talkerindex, "临时整数1");
		local setid = char.getWorkInt(_talkerindex, "临时整数2");
		local petindex = char.getWorkInt(_talkerindex, "临时整数3");
		local petindex2 = char.getCharPet(_talkerindex, setid);
		if petindex ~= petindex2 then
			char.TalkToCli(_talkerindex, -1, "您选择的宠物消失？[多次使用封包丢宠进行回炉会封号哦]", 6);
			return ;
		end
		if char.getInt(petindex, "类型") ~= 3 then
			char.TalkToCli(_talkerindex, -1, "您选择的对像非宠物，无法进行回炉!", "红色");
			return;
		end
		--判断是否骑乘中
		for i = 0, 4 do
			if petindex == char.getCharPet(_talkerindex, i) then
				if char.getInt(_talkerindex, "骑宠") == i then
					char.TalkToCli(_talkerindex, -1, "骑乘中的宠物无法回炉!", "红色");
					return;
				end
			end
		end
		local enemyid = pet.GetEnemyId(petindex);
		local enemybaseid = pet.GetEnemyBaseId(petindex);--取得enemybaseid
		local token = "select * from `PetHuiLu` where `EnemyBase` = "..enemybaseid;
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			num = sasql.num_rows();
			if num == 0 then
				char.TalkToCli(_talkerindex, -1, "该宠物无法进行回炉!("..enemybaseid.."-"..enemyid..")", "红色");
				return;
			end
			sasql.fetch_row();
			local sqlenemyid = other.atoi(sasql.data(1));
			local sqlenemybaseid = other.atoi(sasql.data(2));
			local sqlpoint = other.atoi(sasql.data(3));
			local sqlname = sasql.data(4);
			if sqlname == nil or sqlname == "" then--如果宠物名字不存在的话，给宠物补足名字
				sqlname = pet.getDataChar(sqlenemyid, "名字");
				token = "UPDATE `PetHuiLu` SET Name='"..sqlname.."' where `EnemyBase` = "..enemybaseid;
				sasql.query(token);
			end
			local mypoint = sasql.getVipPoint(_talkerindex);
			if mypoint < sqlpoint then
				char.TalkToCli(_talkerindex, -1, "您的金币不足回炉该宠物，回炉该宠物需要"..sqlpoint.."金币", "红色");
				return;
			end
			if haveitemindex <= -1 then
				haveitemindex = char.FinditemHave(_talkerindex,2962);
			end
			if haveitemindex == -1 then
				char.TalkToCli(_talkerindex, -1, "您身上没有回炉神水，无法进行回炉！", "红色");
				return;
			end
			RePetChange(_talkerindex, petindex, haveitemindex,setid);
			sasql.setVipPoint(_talkerindex,mypoint-sqlpoint);
			char.TalkToCli(_talkerindex, -1, "回炉成功，扣除 ".. sqlpoint.." 金币！", 4);
		end
	end
end

function RePetChange(charaindex, toindex, haveitemindex,havepetindex)
	local enemyid = pet.GetEnemyId(toindex);
	local enemybaseid = pet.GetEnemyBaseId(toindex);--取得enemybaseid
	local array = char.getInt(toindex, "宠ID");
	local huilu = char.getInt(toindex, "宠回炉次");
	local tempno = pet.getEnemyTempArrayFromTempNo(array);
	if tempno > -1 then
		local abi = {0, 0, 0, 0};
		local vital = pet.getInt(tempno, "体力") + math.random(0, 2);
		local str = pet.getInt(tempno, "腕力") + math.random(0, 2);
		local tgh = pet.getInt(tempno, "耐力") + math.random(0, 2);
		local dex = pet.getInt(tempno, "速度") + math.random(0, 2);	
		initnum = pet.getInt(tempno, "初始值");
		ability = {0, 0, 0, 0};						
		for i=1, 10 do
			rnd = math.random(1,4);
			ability[rnd] = ability[rnd] + 1;
		end
		char.setInt(toindex, "能力值", char.getLiftTo8(vital, 1) + char.getLiftTo8(str, 2) + char.getLiftTo8(tgh, 3) + char.getLiftTo8(dex, 4));
		char.setInt(toindex, "体力", (vital + ability[1]) * initnum);
		char.setInt(toindex, "腕力", (str + ability[2]) * initnum);
		char.setInt(toindex, "耐力", (tgh + ability[3]) * initnum);
		char.setInt(toindex, "速度", (dex + ability[4]) * initnum);
		char.setInt(toindex, "宠回炉次",huilu + 1);
		char.setInt(toindex, "等级", 1);
		char.setInt(toindex, "经验", 0);
		char.setInt(toindex, "转数", 0);
		char.setInt(toindex, "宠初等级", 1);
		char.complianceParameter(toindex);
		char.setInt(toindex, "HP", char.getWorkInt(toindex, "最大HP"));
		char.complianceParameter(toindex);
		local petx = char.getWorkInt(toindex, "最大HP");
		local petg = char.getWorkInt(toindex, "攻击力");
		local petf = char.getWorkInt(toindex, "防御力");
		local petm = char.getWorkInt(toindex, "敏捷力");
		char.setInt(toindex,"宠初耐久",petx);
		char.setInt(toindex,"宠初攻击",petg);
		char.setInt(toindex,"宠初防御",petf);
		char.setInt(toindex,"宠初敏捷",petm);

		char.sendStatusString(charaindex, "K" .. havepetindex);
		if haveitemindex >= 0 then
			char.DelItem(charaindex, haveitemindex);
		end
	end
end

function PetChange(itemindex, charaindex, toindex, haveitemindex)
	char.setWorkInt(charaindex, "临时整数1", haveitemindex);
	ShowTalked(npcindex, charaindex);
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	cardgivejf = true;
	npcdata = {"宠物回炉",100000,777,34,16,6};
end

function reloadnpc()
	print("重读NPC信息",char.getChar(npcindex, "名字"));
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.ToAroundChar(npcindex);
end

function main()
	mydata()
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读回炉员", "reloadnpc", "", 1, "[重读回炉员]");
	item.addLUAListFunction( "ITEM_PETCHANGE", "PetChange", "");
end


